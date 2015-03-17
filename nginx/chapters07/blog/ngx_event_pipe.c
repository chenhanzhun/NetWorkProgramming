
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <ngx_event_pipe.h>


static ngx_int_t ngx_event_pipe_read_upstream(ngx_event_pipe_t *p);
static ngx_int_t ngx_event_pipe_write_to_downstream(ngx_event_pipe_t *p);

static ngx_int_t ngx_event_pipe_write_chain_to_temp_file(ngx_event_pipe_t *p);
static ngx_inline void ngx_event_pipe_remove_shadow_links(ngx_buf_t *buf);
static ngx_int_t ngx_event_pipe_drain_chains(ngx_event_pipe_t *p);


/* 转发响应的ngx_event_pipe_t结构体 */
ngx_int_t
ngx_event_pipe(ngx_event_pipe_t *p, ngx_int_t do_write)
{
    u_int         flags;
    ngx_int_t     rc;
    ngx_event_t  *rev, *wev;

    for ( ;; ) {
        if (do_write) {/* 若 do_write标志位为1，表示向下游转发响应 */
            p->log->action = "sending to client";

            /* 调用ngx_event_pipe_write_to_downstream方法向下游转发响应 */
            rc = ngx_event_pipe_write_to_downstream(p);

            if (rc == NGX_ABORT) {
                return NGX_ABORT;
            }

            if (rc == NGX_BUSY) {
                return NGX_OK;
            }
        }

        /* 若do_write标志位为0，则接收上游响应 */
        p->read = 0;
        p->upstream_blocked = 0;

        p->log->action = "reading upstream";

        /* 调用ngx_event_pipe_read_upstream方法读取上游响应 */
        if (ngx_event_pipe_read_upstream(p) == NGX_ABORT) {
            return NGX_ABORT;
        }

        /*
         * 若标志位read和upstream_blocked为0，
         * 则没有可读的响应数据，break退出for循环；
         */
        if (!p->read && !p->upstream_blocked) {
            break;
        }

        /* 否则，设置do_write标志位为1继续进行for循环操作 */
        do_write = 1;
    }

    /*
     * 将上游读事件添加到定时器机制中，注册到epoll事件机制中；
     */
    if (p->upstream->fd != (ngx_socket_t) -1) {
        rev = p->upstream->read;

        flags = (rev->eof || rev->error) ? NGX_CLOSE_EVENT : 0;

        if (ngx_handle_read_event(rev, flags) != NGX_OK) {
            return NGX_ABORT;
        }

        if (rev->active && !rev->ready) {
            ngx_add_timer(rev, p->read_timeout);

        } else if (rev->timer_set) {
            ngx_del_timer(rev);
        }
    }

    /*
     * 将下游写事件添加到定时器机制中，注册到epoll事件机制中；
     */
    if (p->downstream->fd != (ngx_socket_t) -1
        && p->downstream->data == p->output_ctx)
    {
        wev = p->downstream->write;
        if (ngx_handle_write_event(wev, p->send_lowat) != NGX_OK) {
            return NGX_ABORT;
        }

        if (!wev->delayed) {
            if (wev->active && !wev->ready) {
                ngx_add_timer(wev, p->send_timeout);

            } else if (wev->timer_set) {
                ngx_del_timer(wev);
            }
        }
    }

    return NGX_OK;
}


/* 读取上游响应 */
static ngx_int_t
ngx_event_pipe_read_upstream(ngx_event_pipe_t *p)
{
    ssize_t       n, size;
    ngx_int_t     rc;
    ngx_buf_t    *b;
    ngx_chain_t  *chain, *cl, *ln;

    /*
     * 若Nginx与上游之间的通信已经结束、
     * 或Nginx与上游之间的连接出错、
     * 或Nginx与上游之间的连接已经关闭；
     * 则直接return NGX_OK 从当前函数返回；
     */
    if (p->upstream_eof || p->upstream_error || p->upstream_done) {
        return NGX_OK;
    }

    ngx_log_debug1(NGX_LOG_DEBUG_EVENT, p->log, 0,
                   "pipe read upstream: %d", p->upstream->read->ready);

    /* 开始接收上游响应包体，并调用input_filter方法进行处理 */
    for ( ;; ) {

        /*
         * 若Nginx与上游之间的通信已经结束、
         * 或Nginx与上游之间的连接出错、
         * 或Nginx与上游之间的连接已经关闭；
         * 则直接break 从当前for循环退出；
         */
        if (p->upstream_eof || p->upstream_error || p->upstream_done) {
            break;
        }

        /*
         * 若preread_bufs链表缓冲区为空(表示在接收响应头部区间，未预接收响应包体)，
         * 且上游读事件未准备就绪，即没有可读的响应包体；
         * break从for循环退出；
         */
        if (p->preread_bufs == NULL && !p->upstream->read->ready) {
            break;
        }

        /*
         * 若preread_bufs链表缓冲区有未处理的数据，需要把它挂载到chain链表中
         * 即该数据是接收响应头部区间，预接收的响应包体；
         */
        if (p->preread_bufs) {/* the preread_bufs is not empty */

            /* use the pre-read bufs if they exist */

            chain = p->preread_bufs;/* 将预接收响应包体缓冲区添加到chain链表尾端 */
            p->preread_bufs = NULL;/* 使该缓冲区指向NULL，表示没有响应包体 */
            n = p->preread_size;/* 计算预接收响应包体的长度 n */

            ngx_log_debug1(NGX_LOG_DEBUG_EVENT, p->log, 0,
                           "pipe preread: %z", n);

            /*
             * 若preread_bufs链表缓冲区不为空，
             * 则设置read标志位为1，表示当前已经接收了响应包体；
             */
            if (n) {
                p->read = 1;
            }

        } else {/* the preread_bufs is NULL */
        /*
         * 若preread_bufs链表缓冲区没有未处理的响应包体，
         * 则需要有缓冲区来接收上游响应包体；
         */

#if (NGX_HAVE_KQUEUE)

            /*
             * kqueue notifies about the end of file or a pending error.
             * This test allows not to allocate a buf on these conditions
             * and not to call c->recv_chain().
             */

            if (p->upstream->read->available == 0
                && p->upstream->read->pending_eof)
            {
                p->upstream->read->ready = 0;
                p->upstream->read->eof = 1;
                p->upstream_eof = 1;
                p->read = 1;

                if (p->upstream->read->kq_errno) {
                    p->upstream->read->error = 1;
                    p->upstream_error = 1;
                    p->upstream_eof = 0;

                    ngx_log_error(NGX_LOG_ERR, p->log,
                                  p->upstream->read->kq_errno,
                                  "kevent() reported that upstream "
                                  "closed connection");
                }

                break;
            }
#endif

            /*
             * 若 free_raw_bufs不为空，则使用该链表缓冲区接收上游响应包体；
             * free_raw_bufs链表缓冲区用来保存调用一次ngx_event_pipe_read_upstream方法所接收到的上游响应包体；
             */
            if (p->free_raw_bufs) {

                /* use the free bufs if they exist */

                chain = p->free_raw_bufs;/* 将接收响应包体缓冲区添加到chain链表中 */
                if (p->single_buf) {/* 表示每一次只能接收一个ngx_buf_t缓冲区的响应包体 */
                    p->free_raw_bufs = p->free_raw_bufs->next;
                    chain->next = NULL;
                } else {
                    p->free_raw_bufs = NULL;
                }

            } else if (p->allocated < p->bufs.num) {
                /*
                 * 若 free_raw_bufs为空，且已分配的缓冲区数目allocated小于缓冲区数目bufs.num；
                 * 则需要分配一个新的缓冲区来接收上游响应包体；
                 */

                /* allocate a new buf if it's still allowed */

                /* 分配新的缓冲区来接收上游响应 */
                b = ngx_create_temp_buf(p->pool, p->bufs.size);
                if (b == NULL) {
                    return NGX_ABORT;
                }

                p->allocated++;

                /* 分配一个ngx_chain_t 链表缓冲区 */
                chain = ngx_alloc_chain_link(p->pool);
                if (chain == NULL) {
                    return NGX_ABORT;
                }

                /* 把新分配接收响应包体的缓冲区添加到chain链表中 */
                chain->buf = b;
                chain->next = NULL;

            } else if (!p->cacheable
                       && p->downstream->data == p->output_ctx
                       && p->downstream->write->ready
                       && !p->downstream->write->delayed)
            {
                /* 若free_raw_bufs为空，且allocated大于bufs.num，若cacheable标志位为0，即不启用文件缓存，
                 * 检查Nginx与下游之间连接，并检查该连接上写事件是否准备就绪，
                 * 若已准备就绪，即表示可以向下游发送响应包体；
                 */
                /*
                 * if the bufs are not needed to be saved in a cache and
                 * a downstream is ready then write the bufs to a downstream
                 */

                /*
                 * 设置upstream_blocked标志位为1，表示阻塞读取上游响应，
                 * 因为没有缓冲区或文件缓存来接收响应包体，则应该阻塞读取上游响应包体；
                 * 并break退出for循环，此时会向下游转发响应，释放缓冲区，以便再次接收上游响应包体；
                 */
                p->upstream_blocked = 1;

                ngx_log_debug0(NGX_LOG_DEBUG_EVENT, p->log, 0,
                               "pipe downstream ready");

                break;/* 退出for循环 */

            } else if (p->cacheable
                       || p->temp_file->offset < p->max_temp_file_size)
            {/* 若cacheable标志位为1，即开启了文件缓存，则检查临时文件是否达到最大长度，若未达到最大长度 */

                /*
                 * if it is allowed, then save some bufs from p->in
                 * to a temporary file, and add them to a p->out chain
                 */

                /* 将上游响应写入到临时文件中，此时free_raw_bufs有缓冲区空间来接收上游 响应包体 */
                rc = ngx_event_pipe_write_chain_to_temp_file(p);

                ngx_log_debug1(NGX_LOG_DEBUG_EVENT, p->log, 0,
                               "pipe temp offset: %O", p->temp_file->offset);

                if (rc == NGX_BUSY) {
                    break;
                }

                if (rc == NGX_AGAIN) {
                    if (ngx_event_flags & NGX_USE_LEVEL_EVENT
                        && p->upstream->read->active
                        && p->upstream->read->ready)
                    {
                        if (ngx_del_event(p->upstream->read, NGX_READ_EVENT, 0)
                            == NGX_ERROR)
                        {
                            return NGX_ABORT;
                        }
                    }
                }

                if (rc != NGX_OK) {
                    return rc;
                }

                chain = p->free_raw_bufs;
                if (p->single_buf) {
                    p->free_raw_bufs = p->free_raw_bufs->next;
                    chain->next = NULL;
                } else {
                    p->free_raw_bufs = NULL;
                }

            } else {
                /* 若没有缓冲区或文件缓存接收上游响应包体，则暂时不收受上游响应包体，break退出循环 */

                /* there are no bufs to read in */

                ngx_log_debug0(NGX_LOG_DEBUG_EVENT, p->log, 0,
                               "no pipe bufs to read in");

                break;
            }
            /* end of check the free_raw_bufs */

            /*
             * 若有缓冲区接收上游响应包体，则调用recv_chain方法接收上游响应包体；
             * 把接收到的上游响应包体缓冲区添加到free_raw_bufs链表的尾端；
             */
            n = p->upstream->recv_chain(p->upstream, chain);

            ngx_log_debug1(NGX_LOG_DEBUG_EVENT, p->log, 0,
                           "pipe recv chain: %z", n);

            /* 将保存接收到上游响应包体的缓冲区添加到free_raw_bufs链表尾端 */
            if (p->free_raw_bufs) {
                chain->next = p->free_raw_bufs;
            }
            p->free_raw_bufs = chain;

            /* 下面根据所接收上游响应包体的返回值n来进行判断 */

            /*
             * n = NGX_ERROR，表示发生错误，
             * 则设置upstream_error标志位为1，
             * 并return NGX_ERROR从当前函数返回；
             */
            if (n == NGX_ERROR) {
                p->upstream_error = 1;
                return NGX_ERROR;
            }

            /*
             * n = NGX_AGAIN，表示没有读取到上游响应包体，
             * 则break跳出for循环；
             */
            if (n == NGX_AGAIN) {
                if (p->single_buf) {
                    ngx_event_pipe_remove_shadow_links(chain->buf);
                }

                break;
            }

            /*
             * n 大于0，表示已经接收到上游响应包体，
             * 则设置read标志位为1；
             */
            p->read = 1;

            /*
             * n = 0，表示上游服务器主动关闭连接，
             * 则设置upstream_eof标志位为1，表示已关闭连接；
             * 并break退出for循环；
             */
            if (n == 0) {
                p->upstream_eof = 1;
                break;
            }
        }

        /* checking the preread_bufs is end */


        /* 下面开始处理已接收到的上游响应包体数据 */
        p->read_length += n;
        cl = chain;
        p->free_raw_bufs = NULL;

        /* 遍历待处理缓冲区链表chain中的ngx_buf_t缓冲区 */
        while (cl && n > 0) {

            /* 调用该方法将当前ngx_buf_t缓冲区中的shadow域释放 */
            ngx_event_pipe_remove_shadow_links(cl->buf);

            /* 计算当前缓冲区剩余的空间大小 */
            size = cl->buf->end - cl->buf->last;

            /* 若本次接收到上游响应包体的长度大于缓冲区剩余的空间，表示当前缓冲区已满 */
            if (n >= size) {
                cl->buf->last = cl->buf->end;

                /* STUB */ cl->buf->num = p->num++;

                /* 调用input_filter方法处理当前缓冲区响应包体，把其挂载到in链表中 */
                if (p->input_filter(p, cl->buf) == NGX_ERROR) {
                    return NGX_ABORT;
                }

                n -= size;
                ln = cl;
                cl = cl->next;
                ngx_free_chain(p->pool, ln);

            } else {
            /*
             * 若本次接收到上游响应包体的长度小于缓冲区剩余的空间，
             * 表示当前缓冲区还有剩余空间接收上游响应包体；
             * 则先把本次接收到的响应包体缓冲区添加到free_raw_bufs链表尾端；
             */
                cl->buf->last += n;
                n = 0;
            }
        }

        if (cl) {
            for (ln = cl; ln->next; ln = ln->next) { /* void */ }

            ln->next = p->free_raw_bufs;
            p->free_raw_bufs = cl;
        }
    }

    /* end of the For cycle */

#if (NGX_DEBUG)

    for (cl = p->busy; cl; cl = cl->next) {
        ngx_log_debug8(NGX_LOG_DEBUG_EVENT, p->log, 0,
                       "pipe buf busy s:%d t:%d f:%d "
                       "%p, pos %p, size: %z "
                       "file: %O, size: %z",
                       (cl->buf->shadow ? 1 : 0),
                       cl->buf->temporary, cl->buf->in_file,
                       cl->buf->start, cl->buf->pos,
                       cl->buf->last - cl->buf->pos,
                       cl->buf->file_pos,
                       cl->buf->file_last - cl->buf->file_pos);
    }

    for (cl = p->out; cl; cl = cl->next) {
        ngx_log_debug8(NGX_LOG_DEBUG_EVENT, p->log, 0,
                       "pipe buf out  s:%d t:%d f:%d "
                       "%p, pos %p, size: %z "
                       "file: %O, size: %z",
                       (cl->buf->shadow ? 1 : 0),
                       cl->buf->temporary, cl->buf->in_file,
                       cl->buf->start, cl->buf->pos,
                       cl->buf->last - cl->buf->pos,
                       cl->buf->file_pos,
                       cl->buf->file_last - cl->buf->file_pos);
    }

    for (cl = p->in; cl; cl = cl->next) {
        ngx_log_debug8(NGX_LOG_DEBUG_EVENT, p->log, 0,
                       "pipe buf in   s:%d t:%d f:%d "
                       "%p, pos %p, size: %z "
                       "file: %O, size: %z",
                       (cl->buf->shadow ? 1 : 0),
                       cl->buf->temporary, cl->buf->in_file,
                       cl->buf->start, cl->buf->pos,
                       cl->buf->last - cl->buf->pos,
                       cl->buf->file_pos,
                       cl->buf->file_last - cl->buf->file_pos);
    }

    for (cl = p->free_raw_bufs; cl; cl = cl->next) {
        ngx_log_debug8(NGX_LOG_DEBUG_EVENT, p->log, 0,
                       "pipe buf free s:%d t:%d f:%d "
                       "%p, pos %p, size: %z "
                       "file: %O, size: %z",
                       (cl->buf->shadow ? 1 : 0),
                       cl->buf->temporary, cl->buf->in_file,
                       cl->buf->start, cl->buf->pos,
                       cl->buf->last - cl->buf->pos,
                       cl->buf->file_pos,
                       cl->buf->file_last - cl->buf->file_pos);
    }

    ngx_log_debug1(NGX_LOG_DEBUG_EVENT, p->log, 0,
                   "pipe length: %O", p->length);

#endif

    /* 若free_raw_bufs不为空 */
    if (p->free_raw_bufs && p->length != -1) {
        cl = p->free_raw_bufs;

        if (cl->buf->last - cl->buf->pos >= p->length) {

            p->free_raw_bufs = cl->next;

            /* STUB */ cl->buf->num = p->num++;

            /* 调用input_filter方法处理free_raw_bufs缓冲区 */
            if (p->input_filter(p, cl->buf) == NGX_ERROR) {
                 return NGX_ABORT;
            }

            /* 释放已被处理的chain缓冲区数据 */
            ngx_free_chain(p->pool, cl);
        }
    }

    if (p->length == 0) {
        p->upstream_done = 1;
        p->read = 1;
    }

    /*
     * 检查upstream_eof或upstream_error标志位是否为1，若其中一个为1，表示连接已经关闭，
     * 若连接已经关闭，且free_raw_bufs缓冲区不为空；
     */
    if ((p->upstream_eof || p->upstream_error) && p->free_raw_bufs) {

        /* STUB */ p->free_raw_bufs->buf->num = p->num++;

        /* 再次调用input_filter方法处理free_raw_bufs缓冲区的响应包体数据 */
        if (p->input_filter(p, p->free_raw_bufs->buf) == NGX_ERROR) {
            return NGX_ABORT;
        }

        p->free_raw_bufs = p->free_raw_bufs->next;

        /* 检查free_bufs标志位，若为1，则释放shadow域为空的缓冲区 */
        if (p->free_bufs && p->buf_to_file == NULL) {
            for (cl = p->free_raw_bufs; cl; cl = cl->next) {
                if (cl->buf->shadow == NULL) {
                    ngx_pfree(p->pool, cl->buf->start);
                }
            }
        }
    }

    if (p->cacheable && p->in) {
        if (ngx_event_pipe_write_chain_to_temp_file(p) == NGX_ABORT) {
            return NGX_ABORT;
        }
    }

    /* 返回NGX_OK，结束当前函数 */
    return NGX_OK;
}


/* 向下游服务器转发响应包体 */
static ngx_int_t
ngx_event_pipe_write_to_downstream(ngx_event_pipe_t *p)
{
    u_char            *prev;
    size_t             bsize;
    ngx_int_t          rc;
    ngx_uint_t         flush, flushed, prev_last_shadow;
    ngx_chain_t       *out, **ll, *cl;
    ngx_connection_t  *downstream;

    /* 获取Nginx与下游服务器之间的连接 */
    downstream = p->downstream;

    ngx_log_debug1(NGX_LOG_DEBUG_EVENT, p->log, 0,
                   "pipe write downstream: %d", downstream->write->ready);

    flushed = 0;

    for ( ;; ) {
        /* downstream_error标志位为1，表示与下游之间的连接出现错误 */
        if (p->downstream_error) {
            return ngx_event_pipe_drain_chains(p);
        }

        /* 检查与上游之间的连接是否关闭，若已关闭 */
        if (p->upstream_eof || p->upstream_error || p->upstream_done) {

            /* pass the p->out and p->in chains to the output filter */

            for (cl = p->busy; cl; cl = cl->next) {
                cl->buf->recycled = 0;
            }

            /* 调用output_filter方法将out链表中的缓冲区响应包体转发给下游 */
            if (p->out) {
                ngx_log_debug0(NGX_LOG_DEBUG_EVENT, p->log, 0,
                               "pipe write downstream flush out");

                for (cl = p->out; cl; cl = cl->next) {
                    cl->buf->recycled = 0;
                }

                rc = p->output_filter(p->output_ctx, p->out);

                if (rc == NGX_ERROR) {
                    p->downstream_error = 1;
                    return ngx_event_pipe_drain_chains(p);
                }

                p->out = NULL;
            }

            /* 调用output_filter方法将in链表中的缓冲区响应包体转发给下游 */
            if (p->in) {
                ngx_log_debug0(NGX_LOG_DEBUG_EVENT, p->log, 0,
                               "pipe write downstream flush in");

                for (cl = p->in; cl; cl = cl->next) {
                    cl->buf->recycled = 0;
                }

                rc = p->output_filter(p->output_ctx, p->in);

                if (rc == NGX_ERROR) {
                    p->downstream_error = 1;
                    return ngx_event_pipe_drain_chains(p);
                }

                p->in = NULL;
            }

            if (p->cacheable && p->buf_to_file) {
                ngx_log_debug0(NGX_LOG_DEBUG_EVENT, p->log, 0,
                               "pipe write chain");

                if (ngx_event_pipe_write_chain_to_temp_file(p) == NGX_ABORT) {
                    return NGX_ABORT;
                }
            }

            ngx_log_debug0(NGX_LOG_DEBUG_EVENT, p->log, 0,
                           "pipe write downstream done");

            /* TODO: free unused bufs */

            p->downstream_done = 1;
            break;
        }

        /*
         * 若上游连接没有关闭，则检查下游连接上的写事件是否准备就绪；
         * 若准备就绪，则表示可以向下游转发响应包体；
         * 若未准备就绪，则break退出for循环，return NGX_OK从当前函数返回；
         */
        if (downstream->data != p->output_ctx
            || !downstream->write->ready
            || downstream->write->delayed)
        {
            break;
        }

        /* bsize is the size of the busy recycled bufs */

        prev = NULL;
        bsize = 0;

        /* 计算busy链表缓冲区中待发送响应包体的长度bsize */
        for (cl = p->busy; cl; cl = cl->next) {

            if (cl->buf->recycled) {
                if (prev == cl->buf->start) {
                    continue;
                }

                bsize += cl->buf->end - cl->buf->start;
                prev = cl->buf->start;
            }
        }

        ngx_log_debug1(NGX_LOG_DEBUG_EVENT, p->log, 0,
                       "pipe write busy: %uz", bsize);

        out = NULL;

        /* 检查bsize是否大于busy_size配置项 */
        if (bsize >= (size_t) p->busy_size) {
            flush = 1;
            goto flush;
        }

        /* 若bsize小于busy_size配置项 */
        flush = 0;
        ll = NULL;
        prev_last_shadow = 1;

        /*
         * 检查in、out链表缓冲区是否为空，若不为空；
         * 将out、in链表首个缓冲区作为发送内容；
         */
        for ( ;; ) {
            if (p->out) {/* out链表不为空，则取出首个缓冲区作为发送响应内容 */
                cl = p->out;

                if (cl->buf->recycled) {
                    ngx_log_error(NGX_LOG_ALERT, p->log, 0,
                                  "recycled buffer in pipe out chain");
                }

                p->out = p->out->next;

            } else if (!p->cacheable && p->in) {
                /* 若out为空，检查in是否为空，若in不为空，则首个缓冲区作为发送内容 */
                cl = p->in;

                ngx_log_debug3(NGX_LOG_DEBUG_EVENT, p->log, 0,
                               "pipe write buf ls:%d %p %z",
                               cl->buf->last_shadow,
                               cl->buf->pos,
                               cl->buf->last - cl->buf->pos);

                if (cl->buf->recycled && prev_last_shadow) {
                    /* 判断待发送响应包体长度加上本次缓冲区的长度是否大于busy_size */
                    if (bsize + cl->buf->end - cl->buf->start > p->busy_size) {
                        flush = 1;
                        break;
                    }

                    bsize += cl->buf->end - cl->buf->start;
                }

                prev_last_shadow = cl->buf->last_shadow;

                p->in = p->in->next;

            } else {
                break;
            }

            cl->next = NULL;

            if (out) {
                *ll = cl;
            } else {
                out = cl;
            }
            ll = &cl->next;
        }

    flush:

        ngx_log_debug2(NGX_LOG_DEBUG_EVENT, p->log, 0,
                       "pipe write: out:%p, f:%d", out, flush);

        if (out == NULL) {

            if (!flush) {
                break;
            }

            /* a workaround for AIO */
            if (flushed++ > 10) {
                return NGX_BUSY;
            }
        }

        /* 调用output_filter方法发送out链表缓冲区 */
        rc = p->output_filter(p->output_ctx, out);

        /* 更新free、busy、out链表缓冲区 */
        ngx_chain_update_chains(p->pool, &p->free, &p->busy, &out, p->tag);

        if (rc == NGX_ERROR) {
            p->downstream_error = 1;
            return ngx_event_pipe_drain_chains(p);
        }

        /* 遍历free链表，释放缓冲区中的shadow域 */
        for (cl = p->free; cl; cl = cl->next) {

            if (cl->buf->temp_file) {
                if (p->cacheable || !p->cyclic_temp_file) {
                    continue;
                }

                /* reset p->temp_offset if all bufs had been sent */

                if (cl->buf->file_last == p->temp_file->offset) {
                    p->temp_file->offset = 0;
                }
            }

            /* TODO: free buf if p->free_bufs && upstream done */

            /* add the free shadow raw buf to p->free_raw_bufs */

            if (cl->buf->last_shadow) {
                if (ngx_event_pipe_add_free_buf(p, cl->buf->shadow) != NGX_OK) {
                    return NGX_ABORT;
                }

                cl->buf->last_shadow = 0;
            }

            cl->buf->shadow = NULL;
        }
    }

    return NGX_OK;
}


static ngx_int_t
ngx_event_pipe_write_chain_to_temp_file(ngx_event_pipe_t *p)
{
    ssize_t       size, bsize, n;
    ngx_buf_t    *b;
    ngx_uint_t    prev_last_shadow;
    ngx_chain_t  *cl, *tl, *next, *out, **ll, **last_out, **last_free, fl;

    if (p->buf_to_file) {
        fl.buf = p->buf_to_file;
        fl.next = p->in;
        out = &fl;

    } else {
        out = p->in;
    }

    if (!p->cacheable) {

        size = 0;
        cl = out;
        ll = NULL;
        prev_last_shadow = 1;

        ngx_log_debug1(NGX_LOG_DEBUG_EVENT, p->log, 0,
                       "pipe offset: %O", p->temp_file->offset);

        do {
            bsize = cl->buf->last - cl->buf->pos;

            ngx_log_debug4(NGX_LOG_DEBUG_EVENT, p->log, 0,
                           "pipe buf ls:%d %p, pos %p, size: %z",
                           cl->buf->last_shadow, cl->buf->start,
                           cl->buf->pos, bsize);

            if (prev_last_shadow
                && ((size + bsize > p->temp_file_write_size)
                    || (p->temp_file->offset + size + bsize
                        > p->max_temp_file_size)))
            {
                break;
            }

            prev_last_shadow = cl->buf->last_shadow;

            size += bsize;
            ll = &cl->next;
            cl = cl->next;

        } while (cl);

        ngx_log_debug1(NGX_LOG_DEBUG_EVENT, p->log, 0, "size: %z", size);

        if (ll == NULL) {
            return NGX_BUSY;
        }

        if (cl) {
           p->in = cl;
           *ll = NULL;

        } else {
           p->in = NULL;
           p->last_in = &p->in;
        }

    } else {
        p->in = NULL;
        p->last_in = &p->in;
    }

    n = ngx_write_chain_to_temp_file(p->temp_file, out);

    if (n == NGX_ERROR) {
        return NGX_ABORT;
    }

    if (p->buf_to_file) {
        p->temp_file->offset = p->buf_to_file->last - p->buf_to_file->pos;
        n -= p->buf_to_file->last - p->buf_to_file->pos;
        p->buf_to_file = NULL;
        out = out->next;
    }

    if (n > 0) {
        /* update previous buffer or add new buffer */

        if (p->out) {
            for (cl = p->out; cl->next; cl = cl->next) { /* void */ }

            b = cl->buf;

            if (b->file_last == p->temp_file->offset) {
                p->temp_file->offset += n;
                b->file_last = p->temp_file->offset;
                goto free;
            }

            last_out = &cl->next;

        } else {
            last_out = &p->out;
        }

        cl = ngx_chain_get_free_buf(p->pool, &p->free);
        if (cl == NULL) {
            return NGX_ABORT;
        }

        b = cl->buf;

        ngx_memzero(b, sizeof(ngx_buf_t));

        b->tag = p->tag;

        b->file = &p->temp_file->file;
        b->file_pos = p->temp_file->offset;
        p->temp_file->offset += n;
        b->file_last = p->temp_file->offset;

        b->in_file = 1;
        b->temp_file = 1;

        *last_out = cl;
    }

free:

    for (last_free = &p->free_raw_bufs;
         *last_free != NULL;
         last_free = &(*last_free)->next)
    {
        /* void */
    }

    for (cl = out; cl; cl = next) {
        next = cl->next;

        cl->next = p->free;
        p->free = cl;

        b = cl->buf;

        if (b->last_shadow) {

            tl = ngx_alloc_chain_link(p->pool);
            if (tl == NULL) {
                return NGX_ABORT;
            }

            tl->buf = b->shadow;
            tl->next = NULL;

            *last_free = tl;
            last_free = &tl->next;

            b->shadow->pos = b->shadow->start;
            b->shadow->last = b->shadow->start;

            ngx_event_pipe_remove_shadow_links(b->shadow);
        }
    }

    return NGX_OK;
}


/* the copy input filter */

ngx_int_t
ngx_event_pipe_copy_input_filter(ngx_event_pipe_t *p, ngx_buf_t *buf)
{
    ngx_buf_t    *b;
    ngx_chain_t  *cl;

    if (buf->pos == buf->last) {
        return NGX_OK;
    }

    cl = ngx_chain_get_free_buf(p->pool, &p->free);
    if (cl == NULL) {
        return NGX_ERROR;
    }

    b = cl->buf;

    ngx_memcpy(b, buf, sizeof(ngx_buf_t));
    b->shadow = buf;
    b->tag = p->tag;
    b->last_shadow = 1;
    b->recycled = 1;
    buf->shadow = b;

    ngx_log_debug1(NGX_LOG_DEBUG_EVENT, p->log, 0, "input buf #%d", b->num);

    if (p->in) {
        *p->last_in = cl;
    } else {
        p->in = cl;
    }
    p->last_in = &cl->next;

    if (p->length == -1) {
        return NGX_OK;
    }

    p->length -= b->last - b->pos;

    return NGX_OK;
}


static ngx_inline void
ngx_event_pipe_remove_shadow_links(ngx_buf_t *buf)
{
    ngx_buf_t  *b, *next;

    b = buf->shadow;

    if (b == NULL) {
        return;
    }

    while (!b->last_shadow) {
        next = b->shadow;

        b->temporary = 0;
        b->recycled = 0;

        b->shadow = NULL;
        b = next;
    }

    b->temporary = 0;
    b->recycled = 0;
    b->last_shadow = 0;

    b->shadow = NULL;

    buf->shadow = NULL;
}


ngx_int_t
ngx_event_pipe_add_free_buf(ngx_event_pipe_t *p, ngx_buf_t *b)
{
    ngx_chain_t  *cl;

    cl = ngx_alloc_chain_link(p->pool);
    if (cl == NULL) {
        return NGX_ERROR;
    }

    if (p->buf_to_file && b->start == p->buf_to_file->start) {
        b->pos = p->buf_to_file->last;
        b->last = p->buf_to_file->last;

    } else {
        b->pos = b->start;
        b->last = b->start;
    }

    b->shadow = NULL;

    cl->buf = b;

    if (p->free_raw_bufs == NULL) {
        p->free_raw_bufs = cl;
        cl->next = NULL;

        return NGX_OK;
    }

    if (p->free_raw_bufs->buf->pos == p->free_raw_bufs->buf->last) {

        /* add the free buf to the list start */

        cl->next = p->free_raw_bufs;
        p->free_raw_bufs = cl;

        return NGX_OK;
    }

    /* the first free buf is partially filled, thus add the free buf after it */

    cl->next = p->free_raw_bufs->next;
    p->free_raw_bufs->next = cl;

    return NGX_OK;
}


static ngx_int_t
ngx_event_pipe_drain_chains(ngx_event_pipe_t *p)
{
    ngx_chain_t  *cl, *tl;

    for ( ;; ) {
        if (p->busy) {
            cl = p->busy;
            p->busy = NULL;

        } else if (p->out) {
            cl = p->out;
            p->out = NULL;

        } else if (p->in) {
            cl = p->in;
            p->in = NULL;

        } else {
            return NGX_OK;
        }

        while (cl) {
            if (cl->buf->last_shadow) {
                if (ngx_event_pipe_add_free_buf(p, cl->buf->shadow) != NGX_OK) {
                    return NGX_ABORT;
                }

                cl->buf->last_shadow = 0;
            }

            cl->buf->shadow = NULL;
            tl = cl->next;
            cl->next = p->free;
            p->free = cl;
            cl = tl;
        }
    }
}
