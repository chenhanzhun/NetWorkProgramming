/* 创建内存池，该函数定义于 src/core/ngx_palloc.c 文件中 */
ngx_pool_t *
ngx_create_pool(size_t size, ngx_log_t *log)
{
    ngx_pool_t  *p; /* 执行内存池头部 */

    /* 分配大小为 size 的内存 */
    /* ngx_memalign 函数实现于 src/os/unix/ngx_alloc.c 文件中 */
    p = ngx_memalign(NGX_POOL_ALIGNMENT, size, log);
    if (p == NULL) {
        return NULL;
    }

    /* 以下是初始化 ngx_pool_t 结构信息 */

    p->d.last = (u_char *) p + sizeof(ngx_pool_t);
    p->d.end = (u_char *) p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(ngx_pool_t);   /* 可供分配的空间大小 */
    /* 不能超过最大的限定值 4096B */
    p->max = (size < NGX_MAX_ALLOC_FROM_POOL) ? size : NGX_MAX_ALLOC_FROM_POOL;

    p->current = p; /* 指向当前的内存池 */
    p->chain = NULL;
    p->large = NULL;
    p->cleanup = NULL;
    p->log = log;

    return p;
}

/* 销毁内存池 */

void
ngx_destroy_pool(ngx_pool_t *pool)
{
    ngx_pool_t          *p, *n;
    ngx_pool_large_t    *l;
    ngx_pool_cleanup_t  *c;

    /* 若注册了cleanup，则遍历该链表结构，依次调用handler函数清理数据 */
    for (c = pool->cleanup; c; c = c->next) {
        if (c->handler) {
            ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, pool->log, 0,
                           "run cleanup: %p", c);
            c->handler(c->data);
        }
    }

    /* 遍历 large 链表，释放大块内存 */
    for (l = pool->large; l; l = l->next) {

        ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, pool->log, 0, "free: %p", l->alloc);

        if (l->alloc) {
            ngx_free(l->alloc); /* 释放内存 */
        }
    }

    /* 在debug模式下执行 if 和 endif 之间的代码；
     * 主要是用于log记录，跟踪函数销毁时日志信息
     */
#if (NGX_DEBUG)

    /*
     * we could allocate the pool->log from this pool
     * so we cannot use this log while free()ing the pool
     */

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        ngx_log_debug2(NGX_LOG_DEBUG_ALLOC, pool->log, 0,
                       "free: %p, unused: %uz", p, p->d.end - p->d.last);

        if (n == NULL) {
            break;
        }
    }

#endif

    /* 遍历所有分配的内存池，释放内存池结构 */
    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        ngx_free(p);

        if (n == NULL) {
            break;
        }
    }
}

/* 重置内存池
 * 定义于 src/core/ngx_palloc.c 文件中
 */
void
ngx_reset_pool(ngx_pool_t *pool)
{
    ngx_pool_t        *p;
    ngx_pool_large_t  *l;

    /* 遍历大块内存链表，释放大块内存 */
    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            ngx_free(l->alloc);
        }
    }

    for (p = pool; p; p = p->d.next) {
        p->d.last = (u_char *) p + sizeof(ngx_pool_t);
        p->d.failed = 0;
    }

    pool->current = pool;
    pool->chain = NULL;
    pool->large = NULL;
}

/* 分配内存 */

void *
ngx_palloc(ngx_pool_t *pool, size_t size)
{
    u_char      *m;
    ngx_pool_t  *p;

    /* 若请求的内存大小size小于内存池最大内存值max，
     * 则进程小内存分配，从current开始遍历pool链表
     */
    if (size <= pool->max) {

        p = pool->current;

        do {
            /* 执行对齐操作 */
            m = ngx_align_ptr(p->d.last, NGX_ALIGNMENT);

            /* 检查现有内存池是否有足够的内存空间，
             * 若有足够的内存空间，则移动last指针位置，
             * 并返回所分配的内存地址的起始地址
             */
            if ((size_t) (p->d.end - m) >= size) {
                p->d.last = m + size;   /* 在该节点指向的内存块中分配size大小的内存 */

                return m;
            }

            /* 若不满足，则查找下一个内存池 */
            p = p->d.next;

        } while (p);

        /* 若遍历所有现有内存池链表都没有可用的内存空间，
         * 则分配一个新的内存池，并将该内存池连接到现有内存池链表中
         * 同时，返回分配内存的起始地址
         */
        return ngx_palloc_block(pool, size);
    }

    /* 若所请求的内存大小size大于max则调用大块内存分配函数 */
    return ngx_palloc_large(pool, size);
}

static void *
ngx_palloc_block(ngx_pool_t *pool, size_t size)
{
    u_char      *m;
    size_t       psize;
    ngx_pool_t  *p, *new, *current;

    /* 计算pool的大小，即需要分配新的block的大小 */
    psize = (size_t) (pool->d.end - (u_char *) pool);

    /* NGX_POOL_ALIGNMENT对齐操作 */
    m = ngx_memalign(NGX_POOL_ALIGNMENT, psize, pool->log);
    if (m == NULL) {
        return NULL;
    }
    /* 计算需要分配的block的大小 */
    new = (ngx_pool_t *) m;
    new->d.end = m + psize;
    new->d.next = NULL;
    new->d.failed = 0;
    /* 初始化新的内存池 */
    /* 让m指向该块内存ngx_pool_data_t结构体之后数据区起始位置 */
    m += sizeof(ngx_pool_data_t);
    /* 在数据区分配size大小的内存并设置last指针 */
    m = ngx_align_ptr(m, NGX_ALIGNMENT);
    new->d.last = m + size;

    current = pool->current;
    for (p = current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            /* 失败4次以上移动current指针 */
            current = p->d.next;
        }
    }

    /* 将分配的block连接到现有的内存池  */
    p->d.next = new;

    /* 如果是第一次为内存池分配block，这current将指向新分配的block */
    pool->current = current ? current : new;

    return m;
}


/* 直接调用palloc函数，再进行一次0初始化操作 */
void *
ngx_pcalloc(ngx_pool_t *pool, size_t size)
{
    void *p;

    p = ngx_palloc(pool, size);
    if (p) {
        ngx_memzero(p, size);
    }

    return p;
}

/* 按照alignment对齐分配size内存，然后将其挂到large字段，当做大块内存处理 */
void *
ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment)
{
    void              *p;
    ngx_pool_large_t  *large;

    p = ngx_memalign(alignment, size, pool->log);
    if (p == NULL) {
        return NULL;
    }

    large = ngx_palloc(pool, sizeof(ngx_pool_large_t));
    if (large == NULL) {
        ngx_free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}
/* 分配大块内存 */
static void *
ngx_palloc_large(ngx_pool_t *pool, size_t size)
{
    void              *p;
    ngx_uint_t         n;
    ngx_pool_large_t  *large;

    /* 分配内存 */
    p = ngx_alloc(size, pool->log);
    if (p == NULL) {
        return NULL;
    }

    n = 0;

    /* 若在该pool之前已经分配了large字段，
     * 则将所分配的大块内存挂载到内存池的large字段中
     */
    for (large = pool->large; large; large = large->next) {
        if (large->alloc == NULL) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {
            break;
        }
    }

    /* 若在该pool之前并未分配large字段，
     * 则执行分配ngx_pool_large_t 结构体，分配large字段内存，
     * 再将大块内存挂载到pool的large字段中
     */
    large = ngx_palloc(pool, sizeof(ngx_pool_large_t));
    if (large == NULL) {
        ngx_free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

void *
ngx_alloc(size_t size, ngx_log_t *log)
{
    void  *p;

    p = malloc(size);

    if (p == NULL) {
        ngx_log_error(NGX_LOG_EMERG, log, ngx_errno,
            "malloc() %uz bytes failed", size);
    }

    ngx_log_debug2(NGX_LOG_DEBUG_ALLOC, log, 0, "malloc: %p:%uz", p, size);
    return p;
}

/* 释放大块内存 */
ngx_int_t
ngx_pfree(ngx_pool_t *pool, void *p)
{
    ngx_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, pool->log, 0,
                           "free: %p", l->alloc);
            ngx_free(l->alloc);
            l->alloc = NULL;

            return NGX_OK;
        }
    }

    return NGX_DECLINED;
}

/* 注册cleanup；
 * size 是 data 字段所指向的资源的大小；
 */
ngx_pool_cleanup_t * ngx_pool_cleanup_add(ngx_pool_t *p, size_t size);

/* 对内存池进行文件清理操作,即执行handler,此时handler==ngx_pool_cleanup_file */
 void ngx_pool_run_cleanup_file(ngx_pool_t *p, ngx_fd_t fd);

/* 关闭data指定的文件句柄 */
 void ngx_pool_cleanup_file(void *data);

/* 删除data指定的文件 */
 void ngx_pool_delete_file(void *data);

 /* 注册cleanup */
ngx_pool_cleanup_t *
ngx_pool_cleanup_add(ngx_pool_t *p, size_t size)
{
    ngx_pool_cleanup_t  *c;

    c = ngx_palloc(p, sizeof(ngx_pool_cleanup_t));
    if (c == NULL) {
        return NULL;
    }

    if (size) {
        c->data = ngx_palloc(p, size);
        if (c->data == NULL) {
            return NULL;
        }

    } else {
        c->data = NULL;
    }

    c->handler = NULL;
    c->next = p->cleanup;

    p->cleanup = c;

    ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, p->log, 0, "add cleanup: %p", c);

    return c;
}


/* 清理内存池的文件 */
void
ngx_pool_run_cleanup_file(ngx_pool_t *p, ngx_fd_t fd)
{
    ngx_pool_cleanup_t       *c;
    ngx_pool_cleanup_file_t  *cf;

    /* 遍历cleanup结构链表，并执行handler */
    for (c = p->cleanup; c; c = c->next) {
        if (c->handler == ngx_pool_cleanup_file) {

            cf = c->data;

            if (cf->fd == fd) {
                c->handler(cf);
                c->handler = NULL;
                return;
            }
        }
    }
}


/* 关闭data指定的文件句柄 */
void
ngx_pool_cleanup_file(void *data)
{
    ngx_pool_cleanup_file_t  *c = data; /* 指向data所指向的文件句柄 */

    ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, c->log, 0, "file cleanup: fd:%d",
                   c->fd);

    /* 关闭指定文件 */
    if (ngx_close_file(c->fd) == NGX_FILE_ERROR) {
        ngx_log_error(NGX_LOG_ALERT, c->log, ngx_errno,
                      ngx_close_file_n " \"%s\" failed", c->name);
    }
}

/* 删除data所指向的文件 */
void
ngx_pool_delete_file(void *data)
{
    ngx_pool_cleanup_file_t  *c = data;

    ngx_err_t  err;

    ngx_log_debug2(NGX_LOG_DEBUG_ALLOC, c->log, 0, "file cleanup: fd:%d %s",
                   c->fd, c->name);

    /* 删除data所指向的文件 */
    if (ngx_delete_file(c->name) == NGX_FILE_ERROR) {
        err = ngx_errno;

        if (err != NGX_ENOENT) {
            ngx_log_error(NGX_LOG_CRIT, c->log, err,
                          ngx_delete_file_n " \"%s\" failed", c->name);
        }
    }

    /* 关闭文件句柄 */
    if (ngx_close_file(c->fd) == NGX_FILE_ERROR) {
        ngx_log_error(NGX_LOG_ALERT, c->log, ngx_errno,
                      ngx_close_file_n " \"%s\" failed", c->name);
    }
}
