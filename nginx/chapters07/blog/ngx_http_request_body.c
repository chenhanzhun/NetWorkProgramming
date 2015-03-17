
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static void ngx_http_read_client_request_body_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_do_read_client_request_body(ngx_http_request_t *r);
static ngx_int_t ngx_http_write_request_body(ngx_http_request_t *r);
static ngx_int_t ngx_http_read_discarded_request_body(ngx_http_request_t *r);
static ngx_int_t ngx_http_discard_request_body_filter(ngx_http_request_t *r,
    ngx_buf_t *b);
static ngx_int_t ngx_http_test_expect(ngx_http_request_t *r);

static ngx_int_t ngx_http_request_body_filter(ngx_http_request_t *r,
    ngx_chain_t *in);
static ngx_int_t ngx_http_request_body_length_filter(ngx_http_request_t *r,
    ngx_chain_t *in);
static ngx_int_t ngx_http_request_body_chunked_filter(ngx_http_request_t *r,
    ngx_chain_t *in);
static ngx_int_t ngx_http_request_body_save_filter(ngx_http_request_t *r,
    ngx_chain_t *in);


/* 接收HTTP请求包体 */
ngx_int_t
ngx_http_read_client_request_body(ngx_http_request_t *r,
    ngx_http_client_body_handler_pt post_handler)
{
    size_t                     preread;
    ssize_t                    size;
    ngx_int_t                  rc;
    ngx_buf_t                 *b;
    ngx_chain_t                out, *cl;
    ngx_http_request_body_t   *rb;
    ngx_http_core_loc_conf_t  *clcf;

    /*
     * 当有逻辑开启流程时，引用计数会增加1，此流程结束时，引用计数将减1；
     * 在ngx_http_read_client_request_body方法中，首先将原始请求引用计数增加1，
     * 当遇到异常终止时，则在该函数返回前会将引用计数减1,；
     * 若正常结束时，引用计数由post_handler方法继续维护；
     */
    /* 原始请求的引用计数count加1 */
    r->main->count++;

#if (NGX_HTTP_SPDY)
    if (r->spdy_stream && r == r->main) {
        rc = ngx_http_spdy_read_request_body(r, post_handler);
        goto done;
    }
#endif

    /* HTTP请求包体未被处理时，request_body结构是不被分配的，只有处理时才会分配 */
    /*
     * 若当前HTTP请求不是原始请求，或HTTP请求包体已被读取或被丢弃；
     * 则直接执行HTTP模块的回调方法post_handler，并返回NGX_OK；
     */
    if (r != r->main || r->request_body || r->discard_body) {
        post_handler(r);
        return NGX_OK;
    }

    /*
     * ngx_http_test_expect 用于检查客户端是否发送Expect:100-continue头部，
     * 若客户端已发送该头部表示期望发送请求包体数据，则服务器回复HTTP/1.1 100 Continue；
     * 具体意义是：客户端期望发送请求包体，服务器允许客户端发送，
     * 该函数返回NGX_OK；
     */
    if (ngx_http_test_expect(r) != NGX_OK) {
        rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
        goto done;
    }

    /* 只有在确定要接收请求包体时才分配存储HTTP请求包体的结构体 ngx_http_request_body_t 空间 */
    rb = ngx_pcalloc(r->pool, sizeof(ngx_http_request_body_t));
    if (rb == NULL) {
        rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
        goto done;
    }

    /*
     * set by ngx_pcalloc():
     *
     *     rb->bufs = NULL;
     *     rb->buf = NULL;
     *     rb->free = NULL;
     *     rb->busy = NULL;
     *     rb->chunked = NULL;
     */

    /* 初始化存储请求包体结构成员 */
    rb->rest = -1;/* 待接收HTTP请求包体的大小 */
    rb->post_handler = post_handler;/* 接收完包体后的回调方法 */

    /* 令当前请求的post_body成员指向存储请求包体结构 */
    r->request_body = rb;

    /*
     * 若指定HTTP请求包体的content_length字段小于0，则表示不需要接收包体；
     * 执行post_handler方法，并返回；
     */
    if (r->headers_in.content_length_n < 0 && !r->headers_in.chunked) {
        post_handler(r);
        return NGX_OK;
    }

    /* 若指定HTTP请求包体的content_length字段大于0，则表示需要接收包体；*/

    /*
     * 在请求结构ngx_http_request_t 成员中header_in缓冲区保存的是HTTP请求头部，
     * 由于在处理HTTP请求之前，HTTP头部已被完整接收，所以若header_in缓冲区里面
     * 还存在未处理的数据，则证明在接收HTTP请求头部期间，已经预接收了HTTP请求包体；
     */
    preread = r->header_in->last - r->header_in->pos;

    /*
     * 若header_in缓冲区存在预接收的HTTP请求包体，
     * 则计算还需接收HTTP请求包体的大小rest；
     */
    if (preread) {

        /* there is the pre-read part of the request body */

        ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                       "http client request body preread %uz", preread);

        /* 将out的缓冲区指向header_in缓冲区中的请求包体数据 */
        out.buf = r->header_in;
        out.next = NULL;

        /*
         * 将预接收的HTTP请求包体数据添加到r->request_body->bufs中，
         * 即将请求包体存储在新分配的ngx_http_request_body_t rb 结构体的bufs中；
         */
        rc = ngx_http_request_body_filter(r, &out);

        if (rc != NGX_OK) {
            goto done;
        }

        /* 若ngx_http_request_body_filter返回NGX_OK，则继续执行以下程序 */

        /* 更新当前HTTP请求长度：包括请求头部与请求包体 */
        r->request_length += preread - (r->header_in->last - r->header_in->pos);

        /*
         * 若已接收的请求包体不完整，即rest大于0，表示需要继续接收请求包体；
         * 若此时header_in缓冲区仍然有足够的剩余空间接收剩余的请求包体长度，
         * 则不再分配缓冲区内存；
         */
        if (!r->headers_in.chunked
            && rb->rest > 0
            && rb->rest <= (off_t) (r->header_in->end - r->header_in->last))
        {
            /* the whole request body may be placed in r->header_in */

            b = ngx_calloc_buf(r->pool);
            if (b == NULL) {
                rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
                goto done;
            }

            b->temporary = 1;
            b->start = r->header_in->pos;
            b->pos = r->header_in->pos;
            b->last = r->header_in->last;
            b->end = r->header_in->end;

            rb->buf = b;

            /* 设置当前请求读事件的回调方法 */
            r->read_event_handler = ngx_http_read_client_request_body_handler;
            r->write_event_handler = ngx_http_request_empty_handler;

            /*
             * 真正开始接收请求包体数据；
             * 将TCP套接字连接缓冲区中当前的字符流全部读取出来，
             * 并判断是否需要写入临时文件，以及是否接收全部的请求包体，
             * 同时在接收到完整包体后执行回调方法post_handler；
             */
            rc = ngx_http_do_read_client_request_body(r);
            goto done;
        }

    } else {
        /*
         * 若在接收HTTP请求头部过程没有预接收HTTP请求包体数据，
         * 或者预接收了不完整的HTTP请求包体，但是header_in缓冲区不够继续存储剩余的包体；
         * 进一步计算待需接收HTTP请求的大小rest；
         */
        /* set rb->rest */

        if (ngx_http_request_body_filter(r, NULL) != NGX_OK) {
            rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
            goto done;
        }
    }

    /* 若rest为0，表示无需继续接收HTTP请求包体，即已接收到完整的HTTP请求包体 */
    if (rb->rest == 0) {/* 若已接收完整的HTTP请求包体 */
        /* the whole request body was pre-read */

        /*
         * 检查client_body_in_file_only配置项是否打开，若打开，
         * 则将r->request_body->bufs中的包体数据写入到临时文件；
         */
        if (r->request_body_in_file_only) {
            if (ngx_http_write_request_body(r) != NGX_OK) {
                rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
                goto done;
            }

            if (rb->temp_file->file.offset != 0) {

                cl = ngx_chain_get_free_buf(r->pool, &rb->free);
                if (cl == NULL) {
                    rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
                    goto done;
                }

                b = cl->buf;

                ngx_memzero(b, sizeof(ngx_buf_t));

                b->in_file = 1;
                b->file_last = rb->temp_file->file.offset;
                b->file = &rb->temp_file->file;

                rb->bufs = cl;

            } else {
                rb->bufs = NULL;
            }
        }

        /* 执行回调方法 */
        post_handler(r);

        return NGX_OK;
    }

    /* rest小于0表示出错 */
    if (rb->rest < 0) {
        ngx_log_error(NGX_LOG_ALERT, r->connection->log, 0,
                      "negative request body rest");
        rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
        goto done;
    }

    /* 若rest大于0，则表示需要继续接收HTTP请求包体数据，执行以下程序 */

    /* 获取ngx_http_core_module模块的loc级别配置项结构 */
    clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);

    /* 获取缓存请求包体的buffer缓冲区大小 */
    size = clcf->client_body_buffer_size;
    size += size >> 2;

    /* TODO: honor r->request_body_in_single_buf */

    if (!r->headers_in.chunked && rb->rest < size) {
        size = (ssize_t) rb->rest;

        if (r->request_body_in_single_buf) {
            size += preread;
        }

    } else {
        size = clcf->client_body_buffer_size;
    }

    rb->buf = ngx_create_temp_buf(r->pool, size);
    if (rb->buf == NULL) {
        rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
        goto done;
    }

    /* 设置当前请求读事件的回调方法 */
    r->read_event_handler = ngx_http_read_client_request_body_handler;
    r->write_event_handler = ngx_http_request_empty_handler;

    /* 接收请求包体 */
    rc = ngx_http_do_read_client_request_body(r);

done:

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        r->main->count--;
    }

    return rc;
}


static void
ngx_http_read_client_request_body_handler(ngx_http_request_t *r)
{
    ngx_int_t  rc;

    /* 检查连接上读事件timeout标志位是否超时，若超时则终止该请求 */
    if (r->connection->read->timedout) {
        r->connection->timedout = 1;
        ngx_http_finalize_request(r, NGX_HTTP_REQUEST_TIME_OUT);
        return;
    }

    /* 开始接收HTTP请求包体数据 */
    rc = ngx_http_do_read_client_request_body(r);

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        ngx_http_finalize_request(r, rc);
    }
}


/* 读取HTTP请求包体 */
static ngx_int_t
ngx_http_do_read_client_request_body(ngx_http_request_t *r)
{
    off_t                      rest;
    size_t                     size;
    ssize_t                    n;
    ngx_int_t                  rc;
    ngx_buf_t                 *b;
    ngx_chain_t               *cl, out;
    ngx_connection_t          *c;
    ngx_http_request_body_t   *rb;
    ngx_http_core_loc_conf_t  *clcf;

    /* 获取当前请求所对应的连接 */
    c = r->connection;
    /* 获取当前请求的请求包体结构体 */
    rb = r->request_body;

    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, c->log, 0,
                   "http read client request body");

    for ( ;; ) {
        for ( ;; ) {
            /* 若当前缓冲区buf已满 */
            if (rb->buf->last == rb->buf->end) {

                /* pass buffer to request body filter chain */

                out.buf = rb->buf;
                out.next = NULL;

                rc = ngx_http_request_body_filter(r, &out);

                if (rc != NGX_OK) {
                    return rc;
                }

                /* write to file */

                /* 将缓冲区的字符流写入文件 */
                if (ngx_http_write_request_body(r) != NGX_OK) {
                    return NGX_HTTP_INTERNAL_SERVER_ERROR;
                }

                /* update chains */

                rc = ngx_http_request_body_filter(r, NULL);

                if (rc != NGX_OK) {
                    return rc;
                }

                if (rb->busy != NULL) {
                    return NGX_HTTP_INTERNAL_SERVER_ERROR;
                }

                /* 由于已经将当前缓冲区的字符流写入到文件，则该缓冲区有空间继续使用 */
                rb->buf->pos = rb->buf->start;
                rb->buf->last = rb->buf->start;
            }

            /* 计算当前缓冲区剩余的可用空间size */
            size = rb->buf->end - rb->buf->last;
            /* 计算需要继续接收请求包体的大小rest */
            rest = rb->rest - (rb->buf->last - rb->buf->pos);

            /* 若当前缓冲区有足够的空间接收剩余的请求包体，则不需要再分配缓冲区 */
            if ((off_t) size > rest) {
                size = (size_t) rest;
            }

            /* 从TCP连接套接字读取请求包体，并保存到当前缓冲区 */
            n = c->recv(c, rb->buf->last, size);

            ngx_log_debug1(NGX_LOG_DEBUG_HTTP, c->log, 0,
                           "http client request body recv %z", n);

            /* 若连接上套接字字符流还未读取完整，则继续读取 */
            if (n == NGX_AGAIN) {
                break;
            }

            if (n == 0) {
                ngx_log_error(NGX_LOG_INFO, c->log, 0,
                              "client prematurely closed connection");
            }

            /* 读取错误，设置错误编码 */
            if (n == 0 || n == NGX_ERROR) {
                c->error = 1;
                return NGX_HTTP_BAD_REQUEST;
            }

            /* 调整当前缓冲区的使用情况 */
            rb->buf->last += n;
            /* 设置已接收HTTP请求长度 */
            r->request_length += n;

            /* 若已完整接收HTTP请求包体，则将该包体数据存储到r->request_body->bufs中 */
            if (n == rest) {
                /* pass buffer to request body filter chain */

                out.buf = rb->buf;
                out.next = NULL;

                /* 将已读取的请求包体数据挂载到r->request_body->bufs中，并重新计算rest值 */
                rc = ngx_http_request_body_filter(r, &out);

                if (rc != NGX_OK) {
                    return rc;
                }
            }

            if (rb->rest == 0) {
                break;
            }

            if (rb->buf->last < rb->buf->end) {
                break;
            }
        }

        ngx_log_debug1(NGX_LOG_DEBUG_HTTP, c->log, 0,
                       "http client request body rest %O", rb->rest);

        if (rb->rest == 0) {
            break;
        }

        /*
         * 若未接接收到完整的HTTP请求包体，且当前连接读事件未准备就绪，
         * 则需将读事件添加到定时器机制，注册到epoll事件机制中，等待可读事件发生；
         */
        if (!c->read->ready) {
            clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);
            ngx_add_timer(c->read, clcf->client_body_timeout);

            if (ngx_handle_read_event(c->read, 0) != NGX_OK) {
                return NGX_HTTP_INTERNAL_SERVER_ERROR;
            }

            return NGX_AGAIN;
        }
    }

    /* 到此，已经完整接收到HTTP请求，则需要将读事件从定时器机制中移除 */
    if (c->read->timer_set) {
        ngx_del_timer(c->read);
    }

    /* 若设置将请求包体保存到临时文件，则必须将缓冲区的请求包体数据写入到文件中 */
    if (rb->temp_file || r->request_body_in_file_only) {

        /* save the last part */

        /* 将缓冲区的请求包体数据写入到文件中 */
        if (ngx_http_write_request_body(r) != NGX_OK) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        if (rb->temp_file->file.offset != 0) {

            cl = ngx_chain_get_free_buf(r->pool, &rb->free);
            if (cl == NULL) {
                return NGX_HTTP_INTERNAL_SERVER_ERROR;
            }

            b = cl->buf;

            ngx_memzero(b, sizeof(ngx_buf_t));

            b->in_file = 1;
            b->file_last = rb->temp_file->file.offset;
            b->file = &rb->temp_file->file;

            rb->bufs = cl;

        } else {
            rb->bufs = NULL;
        }
    }

    /*
     * 由于已经完成请求包体的接收，则需重新设置读事件的回调方法；
     * read_event_handler 设置为 ngx_http_block_reading 表示阻塞读事件
     * 即再有读事件发生将不会做任何处理；
     */
    r->read_event_handler = ngx_http_block_reading;

    /* 接收HTTP请求包体完毕后，调用回调方法post_handler */
    rb->post_handler(r);

    return NGX_OK;
}


static ngx_int_t
ngx_http_write_request_body(ngx_http_request_t *r)
{
    ssize_t                    n;
    ngx_chain_t               *cl;
    ngx_temp_file_t           *tf;
    ngx_http_request_body_t   *rb;
    ngx_http_core_loc_conf_t  *clcf;

    rb = r->request_body;

    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "http write client request body, bufs %p", rb->bufs);

    if (rb->temp_file == NULL) {
        tf = ngx_pcalloc(r->pool, sizeof(ngx_temp_file_t));
        if (tf == NULL) {
            return NGX_ERROR;
        }

        clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);

        tf->file.fd = NGX_INVALID_FILE;
        tf->file.log = r->connection->log;
        tf->path = clcf->client_body_temp_path;
        tf->pool = r->pool;
        tf->warn = "a client request body is buffered to a temporary file";
        tf->log_level = r->request_body_file_log_level;
        tf->persistent = r->request_body_in_persistent_file;
        tf->clean = r->request_body_in_clean_file;

        if (r->request_body_file_group_access) {
            tf->access = 0660;
        }

        rb->temp_file = tf;

        if (rb->bufs == NULL) {
            /* empty body with r->request_body_in_file_only */

            if (ngx_create_temp_file(&tf->file, tf->path, tf->pool,
                                     tf->persistent, tf->clean, tf->access)
                != NGX_OK)
            {
                return NGX_ERROR;
            }

            return NGX_OK;
        }
    }

    if (rb->bufs == NULL) {
        return NGX_OK;
    }

    n = ngx_write_chain_to_temp_file(rb->temp_file, rb->bufs);

    /* TODO: n == 0 or not complete and level event */

    if (n == NGX_ERROR) {
        return NGX_ERROR;
    }

    rb->temp_file->offset += n;

    /* mark all buffers as written */

    for (cl = rb->bufs; cl; cl = cl->next) {
        cl->buf->pos = cl->buf->last;
    }

    rb->bufs = NULL;

    return NGX_OK;
}

/* 丢弃HTTP请求包体 */
ngx_int_t
ngx_http_discard_request_body(ngx_http_request_t *r)
{
    ssize_t       size;
    ngx_int_t     rc;
    ngx_event_t  *rev;

#if (NGX_HTTP_SPDY)
    if (r->spdy_stream && r == r->main) {
        r->spdy_stream->skip_data = NGX_SPDY_DATA_DISCARD;
        return NGX_OK;
    }
#endif

    /*
     * 若当前HTTP请求不是原始请求，或HTTP请求包体已被读取或被丢弃；
     * 则直接返回NGX_OK；
     */
    if (r != r->main || r->request_body || r->discard_body) {
        return NGX_OK;
    }

    /*
     * ngx_http_test_expect 用于检查客户端是否发送Expect:100-continue头部，
     * 若客户端已发送该头部表示期望发送请求包体数据，则服务器回复HTTP/1.1 100 Continue；
     * 具体意义是：客户端期望发送请求包体，服务器允许客户端发送，
     * 该函数返回NGX_OK；
     */
    if (ngx_http_test_expect(r) != NGX_OK) {
        rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    /* 获取当前连接的读事件 */
    rev = r->connection->read;

    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, rev->log, 0, "http set discard body");

    /* 若读事件在定时器机制中，则将其移除 */
    if (rev->timer_set) {
        ngx_del_timer(rev);
    }

    /* 若请求content-length头部字段小于0，直接返回NGX_OK */
    if (r->headers_in.content_length_n <= 0 && !r->headers_in.chunked) {
        return NGX_OK;
    }

    /* 获取当前请求header_in缓冲区中预接收请求包体数据 */
    size = r->header_in->last - r->header_in->pos;

    /* 若已经预接收了HTTP请求包体数据 */
    if (size || r->headers_in.chunked) {
        /*
         * 丢弃预接收请求包体数据，并根据预接收请求包体大小与请求content-length头部大小，
         * 重新计算content_length_n的值；
         */
        rc = ngx_http_discard_request_body_filter(r, r->header_in);

        /* 若rc不为NGX_OK表示预接收的请求包体数据不完整，需继续接收 */
        if (rc != NGX_OK) {
            return rc;
        }

        /* 若返回rc=NGX_OK，且待丢弃请求包体大小content-length_n为0，表示已丢弃完整的请求包体 */
        if (r->headers_in.content_length_n == 0) {
            return NGX_OK;
        }
    }

    /* 读取剩余的HTTP请求包体数据，并将其丢弃 */
    rc = ngx_http_read_discarded_request_body(r);

    /* 若已经读取到完整请求包体，则返回NGX_OK */
    if (rc == NGX_OK) {
        r->lingering_close = 0;/* 不需要延迟关闭请求 */
        return NGX_OK;
    }

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        return rc;
    }

    /* rc == NGX_AGAIN */

    /*
     * 若读取到的请求包体依然不完整，但此时已经没有剩余数据可读，
     * 则将当前请求读事件回调方法设置为ngx_http_discard_request_body_handler，
     * 并将读事件注册到epoll事件机制中，等待可读事件发生以便继续读取请求包体；
     */
    r->read_event_handler = ngx_http_discarded_request_body_handler;

    if (ngx_handle_read_event(rev, 0) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    /* 由于已将读事件注册到epoll事件机制中，则引用计数增加1，discard_body标志为1 */
    r->count++;
    r->discard_body = 1;

    return NGX_OK;
}


void
ngx_http_discarded_request_body_handler(ngx_http_request_t *r)
{
    ngx_int_t                  rc;
    ngx_msec_t                 timer;
    ngx_event_t               *rev;
    ngx_connection_t          *c;
    ngx_http_core_loc_conf_t  *clcf;

    c = r->connection;
    rev = c->read;

    /*
     * 判断读事件是否超时，若超时则调用ngx_http_finalize_request方法将引用计数减1，
     * 若此时引用计数是0，则直接终止该请求；
     */
    if (rev->timedout) {
        c->timedout = 1;
        c->error = 1;
        ngx_http_finalize_request(r, NGX_ERROR);
        return;
    }

    /* 若需要延迟关闭，则设置延迟关闭连接的时间 */
    if (r->lingering_time) {
        timer = (ngx_msec_t) r->lingering_time - (ngx_msec_t) ngx_time();

        if ((ngx_msec_int_t) timer <= 0) {
            r->discard_body = 0;
            r->lingering_close = 0;
            ngx_http_finalize_request(r, NGX_ERROR);
            return;
        }

    } else {
        timer = 0;
    }

    /* 读取剩余请求包体，并将其丢弃 */
    rc = ngx_http_read_discarded_request_body(r);

    /* 若返回rc=NGX_OK，则表示已接收到完整请求包体，并成功将其丢弃 */
    if (rc == NGX_OK) {
        r->discard_body = 0;
        r->lingering_close = 0;
        ngx_http_finalize_request(r, NGX_DONE);
        return;
    }

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        c->error = 1;
        ngx_http_finalize_request(r, NGX_ERROR);
        return;
    }

    /* rc == NGX_AGAIN */

    /* 若读取的请求包体依旧不完整，则再次将读事件注册到epoll事件机制中 */
    if (ngx_handle_read_event(rev, 0) != NGX_OK) {
        c->error = 1;
        ngx_http_finalize_request(r, NGX_ERROR);
        return;
    }

    /* 若设置了延迟，则将读事件添加到定时器事件机制中 */
    if (timer) {

        clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);

        timer *= 1000;

        if (timer > clcf->lingering_timeout) {
            timer = clcf->lingering_timeout;
        }

        ngx_add_timer(rev, timer);
    }
}


/* 读取请求包体，并将其丢弃 */
static ngx_int_t
ngx_http_read_discarded_request_body(ngx_http_request_t *r)
{
    size_t     size;
    ssize_t    n;
    ngx_int_t  rc;
    ngx_buf_t  b;
    u_char     buffer[NGX_HTTP_DISCARD_BUFFER_SIZE];

    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "http read discarded body");

    ngx_memzero(&b, sizeof(ngx_buf_t));

    b.temporary = 1;

    for ( ;; ) {
        /* 若待丢弃的请求包体大小content_length_n 为0，表示不需要接收请求包体 */
        if (r->headers_in.content_length_n == 0) {
            /* 重新设置读事件的回调方法，其实该回调方法不进行任何操作 */
            r->read_event_handler = ngx_http_block_reading;
            return NGX_OK;
        }

        /* 若当前连接的读事件未准备就绪，则不能读取数据，即返回NGX_AGAIN */
        if (!r->connection->read->ready) {
            return NGX_AGAIN;
        }

        /* 若需要读取请求包体数据，计算需要读取请求包体的大小size */
        size = (size_t) ngx_min(r->headers_in.content_length_n,
                                NGX_HTTP_DISCARD_BUFFER_SIZE);

        /* 从连接套接字缓冲区读取请求包体数据 */
        n = r->connection->recv(r->connection, buffer, size);

        if (n == NGX_ERROR) {
            r->connection->error = 1;
            return NGX_OK;
        }

        /* 若读取的请求包体数据不完整，则继续读取 */
        if (n == NGX_AGAIN) {
            return NGX_AGAIN;
        }

        /* 若n=0或n=NGX_ERROR表示读取失败，即该连接已关闭，则不需要丢弃包体 */
        if (n == 0) {
            return NGX_OK;
        }

        /* 若返回n=NGX_OK ，表示读取到完整的请求包体，则将其丢弃 */
        b.pos = buffer;
        b.last = buffer + n;

        /* 将读取的完整请求包体丢弃 */
        rc = ngx_http_discard_request_body_filter(r, &b);

        if (rc != NGX_OK) {
            return rc;
        }
    }
}


static ngx_int_t
ngx_http_discard_request_body_filter(ngx_http_request_t *r, ngx_buf_t *b)
{
    size_t                    size;
    ngx_int_t                 rc;
    ngx_http_request_body_t  *rb;

    if (r->headers_in.chunked) {

        rb = r->request_body;

        if (rb == NULL) {

            rb = ngx_pcalloc(r->pool, sizeof(ngx_http_request_body_t));
            if (rb == NULL) {
                return NGX_HTTP_INTERNAL_SERVER_ERROR;
            }

            rb->chunked = ngx_pcalloc(r->pool, sizeof(ngx_http_chunked_t));
            if (rb->chunked == NULL) {
                return NGX_HTTP_INTERNAL_SERVER_ERROR;
            }

            r->request_body = rb;
        }

        for ( ;; ) {

            rc = ngx_http_parse_chunked(r, b, rb->chunked);

            if (rc == NGX_OK) {

                /* a chunk has been parsed successfully */

                size = b->last - b->pos;

                if ((off_t) size > rb->chunked->size) {
                    b->pos += (size_t) rb->chunked->size;
                    rb->chunked->size = 0;

                } else {
                    rb->chunked->size -= size;
                    b->pos = b->last;
                }

                continue;
            }

            if (rc == NGX_DONE) {

                /* a whole response has been parsed successfully */

                r->headers_in.content_length_n = 0;
                break;
            }

            if (rc == NGX_AGAIN) {

                /* set amount of data we want to see next time */

                r->headers_in.content_length_n = rb->chunked->length;
                break;
            }

            /* invalid */

            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                          "client sent invalid chunked body");

            return NGX_HTTP_BAD_REQUEST;
        }

    } else {
        size = b->last - b->pos;

        if ((off_t) size > r->headers_in.content_length_n) {
            b->pos += (size_t) r->headers_in.content_length_n;
            r->headers_in.content_length_n = 0;

        } else {
            b->pos = b->last;
            r->headers_in.content_length_n -= size;
        }
    }

    return NGX_OK;
}


static ngx_int_t
ngx_http_test_expect(ngx_http_request_t *r)
{
    ngx_int_t   n;
    ngx_str_t  *expect;

    if (r->expect_tested
        || r->headers_in.expect == NULL
        || r->http_version < NGX_HTTP_VERSION_11)
    {
        return NGX_OK;
    }

    r->expect_tested = 1;

    expect = &r->headers_in.expect->value;

    if (expect->len != sizeof("100-continue") - 1
        || ngx_strncasecmp(expect->data, (u_char *) "100-continue",
                           sizeof("100-continue") - 1)
           != 0)
    {
        return NGX_OK;
    }

    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "send 100 Continue");

    n = r->connection->send(r->connection,
                            (u_char *) "HTTP/1.1 100 Continue" CRLF CRLF,
                            sizeof("HTTP/1.1 100 Continue" CRLF CRLF) - 1);

    if (n == sizeof("HTTP/1.1 100 Continue" CRLF CRLF) - 1) {
        return NGX_OK;
    }

    /* we assume that such small packet should be send successfully */

    return NGX_ERROR;
}


/* 解析并存储HTTP请求包体数据 */
static ngx_int_t
ngx_http_request_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
    /*
     * 若是chunked编码形式，则调用ngx_http_request_body_chunked_filter解析
     * 并存储chunked编码的请求包体数据；
     */
    if (r->headers_in.chunked) {
        return ngx_http_request_body_chunked_filter(r, in);

    } else {
        /*
         * 若不是chunked编码形式，则调用ngx_http_request_body_length_filter解析
         * 并存储长度已由content-length决定的请求包体数据；
         */
        return ngx_http_request_body_length_filter(r, in);
    }
}


static ngx_int_t
ngx_http_request_body_length_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
    size_t                     size;
    ngx_int_t                  rc;
    ngx_buf_t                 *b;
    ngx_chain_t               *cl, *tl, *out, **ll;
    ngx_http_request_body_t   *rb;

    /* 获取当前HTTP请求的包体结构 */
    rb = r->request_body;

    /* 若当前HTTP请求包体未被接收，rest设置为请求头部的content-length */
    if (rb->rest == -1) {
        ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                       "http request body content length filter");

        /* 计算待接收HTTP请求包体长度rest，即为content_length_n */
        rb->rest = r->headers_in.content_length_n;
    }

    out = NULL;
    ll = &out;

    /* 遍历输入参数in 的ngx_chain_t链表 */
    for (cl = in; cl; cl = cl->next) {

        /* 若不存在待接收HTTP请求包体，则跳出for循环 */
        if (rb->rest == 0) {
            break;
        }

        /* 从free链表中申请接收HTTP请求包体的链表缓冲区 */
        tl = ngx_chain_get_free_buf(r->pool, &rb->free);
        if (tl == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        /* b指向当前刚申请的链表缓冲区 */
        b = tl->buf;

        /* 初始化b所指向的缓冲区 */
        ngx_memzero(b, sizeof(ngx_buf_t));

        /* 表示该缓冲区buf所包含的内容在内存中可被filter处理变更 */
        b->temporary = 1;
        b->tag = (ngx_buf_tag_t) &ngx_http_read_client_request_body;
        b->start = cl->buf->pos;
        b->pos = cl->buf->pos;
        b->last = cl->buf->last;
        b->end = cl->buf->end;

        /* 计算当前链表缓冲区所保存数据的实际大小 */
        size = cl->buf->last - cl->buf->pos;

        /* 若size小于待接收HTTP请求包体长度 */
        if ((off_t) size < rb->rest) {
            /* 调整当前链表缓冲区的内存 */
            cl->buf->pos = cl->buf->last;
            /* 表示该size大小的包体被接收，重新计算待接收包体大小 rest */
            rb->rest -= size;

        } else {/* 若size大于待接收HTTP请求包体长度 */
            cl->buf->pos += (size_t) rb->rest;
            rb->rest = 0;/* 当前HTTP请求包体都已被接收 */
            b->last = cl->buf->pos;
            b->last_buf = 1;
        }

        *ll = tl;
        ll = &tl->next;
    }

    /* 将存储HTTP请求包体的链表缓冲区添加到bufs缓冲区中 */
    rc = ngx_http_request_body_save_filter(r, out);

    /* 更新free和busy链表的数据，即将刚存储HTTP请求包体的链表缓冲区添加到busy链表中 */
    ngx_chain_update_chains(r->pool, &rb->free, &rb->busy, &out,
                            (ngx_buf_tag_t) &ngx_http_read_client_request_body);

    return rc;
}


static ngx_int_t
ngx_http_request_body_chunked_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
    size_t                     size;
    ngx_int_t                  rc;
    ngx_buf_t                 *b;
    ngx_chain_t               *cl, *out, *tl, **ll;
    ngx_http_request_body_t   *rb;
    ngx_http_core_loc_conf_t  *clcf;

    rb = r->request_body;

    if (rb->rest == -1) {

        ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                       "http request body chunked filter");

        rb->chunked = ngx_pcalloc(r->pool, sizeof(ngx_http_chunked_t));
        if (rb->chunked == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        r->headers_in.content_length_n = 0;
        rb->rest = 3;
    }

    out = NULL;
    ll = &out;

    for (cl = in; cl; cl = cl->next) {

        for ( ;; ) {

            ngx_log_debug7(NGX_LOG_DEBUG_EVENT, r->connection->log, 0,
                           "http body chunked buf "
                           "t:%d f:%d %p, pos %p, size: %z file: %O, size: %z",
                           cl->buf->temporary, cl->buf->in_file,
                           cl->buf->start, cl->buf->pos,
                           cl->buf->last - cl->buf->pos,
                           cl->buf->file_pos,
                           cl->buf->file_last - cl->buf->file_pos);

            rc = ngx_http_parse_chunked(r, cl->buf, rb->chunked);

            if (rc == NGX_OK) {

                /* a chunk has been parsed successfully */

                clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);

                if (clcf->client_max_body_size
                    && clcf->client_max_body_size
                       - r->headers_in.content_length_n < rb->chunked->size)
                {
                    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                                  "client intended to send too large chunked "
                                  "body: %O+%O bytes",
                                  r->headers_in.content_length_n,
                                  rb->chunked->size);

                    r->lingering_close = 1;

                    return NGX_HTTP_REQUEST_ENTITY_TOO_LARGE;
                }

                tl = ngx_chain_get_free_buf(r->pool, &rb->free);
                if (tl == NULL) {
                    return NGX_HTTP_INTERNAL_SERVER_ERROR;
                }

                b = tl->buf;

                ngx_memzero(b, sizeof(ngx_buf_t));

                b->temporary = 1;
                b->tag = (ngx_buf_tag_t) &ngx_http_read_client_request_body;
                b->start = cl->buf->pos;
                b->pos = cl->buf->pos;
                b->last = cl->buf->last;
                b->end = cl->buf->end;

                *ll = tl;
                ll = &tl->next;

                size = cl->buf->last - cl->buf->pos;

                if ((off_t) size > rb->chunked->size) {
                    cl->buf->pos += (size_t) rb->chunked->size;
                    r->headers_in.content_length_n += rb->chunked->size;
                    rb->chunked->size = 0;

                } else {
                    rb->chunked->size -= size;
                    r->headers_in.content_length_n += size;
                    cl->buf->pos = cl->buf->last;
                }

                b->last = cl->buf->pos;

                continue;
            }

            if (rc == NGX_DONE) {

                /* a whole response has been parsed successfully */

                rb->rest = 0;

                tl = ngx_chain_get_free_buf(r->pool, &rb->free);
                if (tl == NULL) {
                    return NGX_HTTP_INTERNAL_SERVER_ERROR;
                }

                b = tl->buf;

                ngx_memzero(b, sizeof(ngx_buf_t));

                b->last_buf = 1;

                *ll = tl;
                ll = &tl->next;

                break;
            }

            if (rc == NGX_AGAIN) {

                /* set rb->rest, amount of data we want to see next time */

                rb->rest = rb->chunked->length;

                break;
            }

            /* invalid */

            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                          "client sent invalid chunked body");

            return NGX_HTTP_BAD_REQUEST;
        }
    }

    rc = ngx_http_request_body_save_filter(r, out);

    ngx_chain_update_chains(r->pool, &rb->free, &rb->busy, &out,
                            (ngx_buf_tag_t) &ngx_http_read_client_request_body);

    return rc;
}


static ngx_int_t
ngx_http_request_body_save_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
#if (NGX_DEBUG)
    ngx_chain_t               *cl;
#endif
    ngx_http_request_body_t   *rb;

    rb = r->request_body;

#if (NGX_DEBUG)

    for (cl = rb->bufs; cl; cl = cl->next) {
        ngx_log_debug7(NGX_LOG_DEBUG_EVENT, r->connection->log, 0,
                       "http body old buf t:%d f:%d %p, pos %p, size: %z "
                       "file: %O, size: %z",
                       cl->buf->temporary, cl->buf->in_file,
                       cl->buf->start, cl->buf->pos,
                       cl->buf->last - cl->buf->pos,
                       cl->buf->file_pos,
                       cl->buf->file_last - cl->buf->file_pos);
    }

    for (cl = in; cl; cl = cl->next) {
        ngx_log_debug7(NGX_LOG_DEBUG_EVENT, r->connection->log, 0,
                       "http body new buf t:%d f:%d %p, pos %p, size: %z "
                       "file: %O, size: %z",
                       cl->buf->temporary, cl->buf->in_file,
                       cl->buf->start, cl->buf->pos,
                       cl->buf->last - cl->buf->pos,
                       cl->buf->file_pos,
                       cl->buf->file_last - cl->buf->file_pos);
    }

#endif

    /* TODO: coalesce neighbouring buffers */

    if (ngx_chain_add_copy(r->pool, &rb->bufs, in) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    return NGX_OK;
}
