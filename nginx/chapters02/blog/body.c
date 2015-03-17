/*
 * Let's take a simple example.
 * Suppose we want to insert the text "<l!-- Served by Nginx -->" to the end of every request.
 * First, we need to figure out if the response's final buffer is included in the buffer chain we were given.
 * Like I said, there's not a fancy API, so we'll be rolling our own for loop:
 */

    ngx_chain_t *chain_link;
    int chain_contains_last_buffer = 0;

    chain_link = in;
    for ( ; ; ) {
        if (chain_link->buf->last_buf)
            chain_contains_last_buffer = 1;
        if (chain_link->next == NULL)
            break;
        chain_link = chain_link->next;
    }
/*
 * Now let's bail out if we don't have that last buffer:
 */

    if (!chain_contains_last_buffer)
        return ngx_http_next_body_filter(r, in);
/*
 * Super, now the last buffer is stored in chain_link.
 * Now we allocate a new buffer:
 */

    ngx_buf_t    *b;
    b = ngx_calloc_buf(r->pool);
    if (b == NULL) {
        return NGX_ERROR;
    }
/*
 * And put some data in it:
 */

    b->pos = (u_char *) "<!-- Served by Nginx -->";
    b->last = b->pos + sizeof("<!-- Served by Nginx -->") - 1;
/*
 * And hook the buffer into a new chain link:
 */

    ngx_chain_t   *added_link;

    added_link = ngx_alloc_chain_link(r->pool);
    if (added_link == NULL)
        return NGX_ERROR;

    added_link->buf = b;
    added_link->next = NULL;
/*
 * Finally, hook the new chain link to the final chain link we found before:
 */

    chain_link->next = added_link;
/*
 * And reset the "last_buf" variables to reflect reality:
 */

    chain_link->buf->last_buf = 0;
    added_link->buf->last_buf = 1;
/*
 * And pass along the modified chain to the next output filter:
 */

    return ngx_http_next_body_filter(r, in);

/*
 * The resulting function takes much more effort than what you'd do with, say, mod_perl ($response->body =~ s/$/<!-- Served by mod_perl -->/),
 * but the buffer chain is a very powerful construct, allowing programmers to process data incrementally so that the client gets something as soon as possible.
 * However, in my opinion, the buffer chain desperately needs a cleaner interface so that programmers can't leave the chain in an inconsistent state.
 * For now, manipulate it at your own risk.
 */
ngx_int_t
ngx_http_send_header(ngx_http_request_t *r)
{
        ...

        return ngx_http_top_header_filter(r);
}

ngx_int_t
ngx_http_output_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
    ngx_int_t          rc;
    ngx_connection_t  *c;

    c = r->connection;

    rc = ngx_http_top_body_filter(r, in);

    if (rc == NGX_ERROR) {
        /* NGX_ERROR may be returned by any filter */
        c->error = 1;
    }

    return rc;
}

typedef struct ngx_chain_s ngx_chain_t;

        struct ngx_chain_s {
                ngx_buf_t    *buf;
                ngx_chain_t  *next;
        };

struct ngx_buf_s {
        u_char          *pos;       /* 当前buffer真实内容的起始位置 */
        u_char          *last;      /* 当前buffer真实内容的结束位置 */
        off_t            file_pos;  /* 在文件中真实内容的起始位置   */
        off_t            file_last; /* 在文件中真实内容的结束位置   */

        u_char          *start;    /* buffer内存的开始分配的位置 */
        u_char          *end;      /* buffer内存的结束分配的位置 */
        ngx_buf_tag_t    tag;      /* buffer属于哪个模块的标志 */
        ngx_file_t      *file;     /* buffer所引用的文件 */

        /* 用来引用替换过后的buffer，以便当所有buffer输出以后，
         * 这个影子buffer可以被释放。
         */
        ngx_buf_t       *shadow;

        /* the buf's content could be changed */
        unsigned         temporary:1;

        /*
         * the buf's content is in a memory cache or in a read only memory
         * and must not be changed
         */
        unsigned         memory:1;

        /* the buf's content is mmap()ed and must not be changed */
        unsigned         mmap:1;

        unsigned         recycled:1; /* 内存可以被输出并回收 */
        unsigned         in_file:1;  /* buffer的内容在文件中 */
        /* 马上全部输出buffer的内容, gzip模块里面用得比较多 */
        unsigned         flush:1;
        /* 基本上是一段输出链的最后一个buffer带的标志，标示可以输出，
         * 有些零长度的buffer也可以置该标志
         */
        unsigned         sync:1;
        /* 所有请求里面最后一块buffer，包含子请求 */
        unsigned         last_buf:1;
        /* 当前请求输出链的最后一块buffer         */
        unsigned         last_in_chain:1;
        /* shadow链里面的最后buffer，可以释放buffer了 */
        unsigned         last_shadow:1;
        /* 是否是暂存文件 */
        unsigned         temp_file:1;

        /* 统计用，表示使用次数 */
        /* STUB */ int   num;
};


/*
 * "config" for filter modules:
 */

ngx_addon_name=ngx_http_<your module>_module
HTTP_AUX_FILTER_MODULES="$HTTP_AUX_FILTER_MODULES ngx_http_<your module>_module"
NGX_ADDON_SRCS="$NGX_ADDON_SRCS $ngx_addon_dir/ngx_http_<your module>_module.c"

/*
 * "config" for other modules:
 */

ngx_addon_name=ngx_http_<your module>_module
HTTP_MODULES="$HTTP_MODULES ngx_http_<your module>_module"
NGX_ADDON_SRCS="$NGX_ADDON_SRCS $ngx_addon_dir/ngx_http_<your module>_module.c"


./configure --add-module=path/to/your/new/module/directory
make
make install
