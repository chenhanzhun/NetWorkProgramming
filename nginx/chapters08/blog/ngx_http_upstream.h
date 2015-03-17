
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HTTP_UPSTREAM_H_INCLUDED_
#define _NGX_HTTP_UPSTREAM_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <ngx_event_connect.h>
#include <ngx_event_pipe.h>
#include <ngx_http.h>


#define NGX_HTTP_UPSTREAM_FT_ERROR           0x00000002
#define NGX_HTTP_UPSTREAM_FT_TIMEOUT         0x00000004
#define NGX_HTTP_UPSTREAM_FT_INVALID_HEADER  0x00000008
#define NGX_HTTP_UPSTREAM_FT_HTTP_500        0x00000010
#define NGX_HTTP_UPSTREAM_FT_HTTP_502        0x00000020
#define NGX_HTTP_UPSTREAM_FT_HTTP_503        0x00000040
#define NGX_HTTP_UPSTREAM_FT_HTTP_504        0x00000080
#define NGX_HTTP_UPSTREAM_FT_HTTP_403        0x00000100
#define NGX_HTTP_UPSTREAM_FT_HTTP_404        0x00000200
#define NGX_HTTP_UPSTREAM_FT_UPDATING        0x00000400
#define NGX_HTTP_UPSTREAM_FT_BUSY_LOCK       0x00000800
#define NGX_HTTP_UPSTREAM_FT_MAX_WAITING     0x00001000
#define NGX_HTTP_UPSTREAM_FT_NOLIVE          0x40000000
#define NGX_HTTP_UPSTREAM_FT_OFF             0x80000000

#define NGX_HTTP_UPSTREAM_FT_STATUS          (NGX_HTTP_UPSTREAM_FT_HTTP_500  \
                                             |NGX_HTTP_UPSTREAM_FT_HTTP_502  \
                                             |NGX_HTTP_UPSTREAM_FT_HTTP_503  \
                                             |NGX_HTTP_UPSTREAM_FT_HTTP_504  \
                                             |NGX_HTTP_UPSTREAM_FT_HTTP_403  \
                                             |NGX_HTTP_UPSTREAM_FT_HTTP_404)

#define NGX_HTTP_UPSTREAM_INVALID_HEADER     40


#define NGX_HTTP_UPSTREAM_IGN_XA_REDIRECT    0x00000002
#define NGX_HTTP_UPSTREAM_IGN_XA_EXPIRES     0x00000004
#define NGX_HTTP_UPSTREAM_IGN_EXPIRES        0x00000008
#define NGX_HTTP_UPSTREAM_IGN_CACHE_CONTROL  0x00000010
#define NGX_HTTP_UPSTREAM_IGN_SET_COOKIE     0x00000020
#define NGX_HTTP_UPSTREAM_IGN_XA_LIMIT_RATE  0x00000040
#define NGX_HTTP_UPSTREAM_IGN_XA_BUFFERING   0x00000080
#define NGX_HTTP_UPSTREAM_IGN_XA_CHARSET     0x00000100


typedef struct {
    ngx_msec_t                       bl_time;
    ngx_uint_t                       bl_state;

    ngx_uint_t                       status;
    time_t                           response_sec;
    ngx_uint_t                       response_msec;
    off_t                            response_length;

    ngx_str_t                       *peer;
} ngx_http_upstream_state_t;


typedef struct {
    ngx_hash_t                       headers_in_hash;
    ngx_array_t                      upstreams;
                                             /* ngx_http_upstream_srv_conf_t */
} ngx_http_upstream_main_conf_t;

typedef struct ngx_http_upstream_srv_conf_s  ngx_http_upstream_srv_conf_t;

typedef ngx_int_t (*ngx_http_upstream_init_pt)(ngx_conf_t *cf,
    ngx_http_upstream_srv_conf_t *us);
typedef ngx_int_t (*ngx_http_upstream_init_peer_pt)(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us);


typedef struct {
    /* 负载均衡的类型 */
    ngx_http_upstream_init_pt        init_upstream;
    /* 负载均衡类型的初始化函数 */
    ngx_http_upstream_init_peer_pt   init;
    /* 指向 ngx_http_upstream_rr_peers_t 结构体 */
    void                            *data;
} ngx_http_upstream_peer_t;


/* 服务器结构体 */
typedef struct {
    /* 指向存储 IP 地址的数组，因为同一个域名可能会有多个 IP 地址 */
    ngx_addr_t                      *addrs;
    /* IP 地址数组中元素个数 */
    ngx_uint_t                       naddrs;
    /* 权重 */
    ngx_uint_t                       weight;
    /* 最大失败次数 */
    ngx_uint_t                       max_fails;
    /* 失败时间阈值 */
    time_t                           fail_timeout;

    /* 标志位，若为 1，表示不参与策略选择 */
    unsigned                         down:1;
    /* 标志位，若为 1，表示为备用服务器 */
    unsigned                         backup:1;
} ngx_http_upstream_server_t;


#define NGX_HTTP_UPSTREAM_CREATE        0x0001
#define NGX_HTTP_UPSTREAM_WEIGHT        0x0002
#define NGX_HTTP_UPSTREAM_MAX_FAILS     0x0004
#define NGX_HTTP_UPSTREAM_FAIL_TIMEOUT  0x0008
#define NGX_HTTP_UPSTREAM_DOWN          0x0010
#define NGX_HTTP_UPSTREAM_BACKUP        0x0020


struct ngx_http_upstream_srv_conf_s {
    ngx_http_upstream_peer_t         peer;
    void                           **srv_conf;

    ngx_array_t                     *servers;  /* ngx_http_upstream_server_t */

    ngx_uint_t                       flags;
    ngx_str_t                        host;
    u_char                          *file_name;
    ngx_uint_t                       line;
    in_port_t                        port;
    in_port_t                        default_port;
    ngx_uint_t                       no_port;  /* unsigned no_port:1 */
};


typedef struct {
    ngx_addr_t                      *addr;
    ngx_http_complex_value_t        *value;
} ngx_http_upstream_local_t;


/* ngx_http_upstream_conf_t 结构体 */
typedef struct {
    /*
     * 若在ngx_http_upstream_t结构体中没有实现resolved成员时，
     * upstream这个结构体才会生效，定义上游服务器的配置；
     */
    ngx_http_upstream_srv_conf_t    *upstream;

    /* 建立TCP连接的超时时间 */
    ngx_msec_t                       connect_timeout;
    /* 发送请求的超时时间 */
    ngx_msec_t                       send_timeout;
    /* 接收响应的超时时间 */
    ngx_msec_t                       read_timeout;
    ngx_msec_t                       timeout;

    /* TCP的SO_SNOLOWAT选项，表示发送缓冲区的下限 */
    size_t                           send_lowat;
    /* ngx_http_upstream_t中的buffer大小 */
    size_t                           buffer_size;

    size_t                           busy_buffers_size;
    /* 临时文件的最大长度 */
    size_t                           max_temp_file_size;
    /* 表示缓冲区中的响应写入到临时文件时一次写入字符流的最大长度 */
    size_t                           temp_file_write_size;

    size_t                           busy_buffers_size_conf;
    size_t                           max_temp_file_size_conf;
    size_t                           temp_file_write_size_conf;

    /* 以缓存响应的方式转发上游服务器的包体时所使用的内存大小 */
    ngx_bufs_t                       bufs;

    /* ignore_headers使得upstream在转发包头时跳过对某些头部的处理 */
    ngx_uint_t                       ignore_headers;
    /*
     * 以二进制位来处理错误码，若处理上游响应时发现这些错误码，
     * 那么在没有将响应转发给下游客户端时，将会选择一个上游服务器来重发请求；
     */
    ngx_uint_t                       next_upstream;
    /* 表示所创建的目录与文件的权限 */
    ngx_uint_t                       store_access;
    /* 转发响应方式的标志位，为1表示打开缓存，若为0，则启用固定内存大小缓存响应 */
    ngx_flag_t                       buffering;
    ngx_flag_t                       pass_request_headers;
    ngx_flag_t                       pass_request_body;

    /* 不检查Nginx与下游之间的连接是否断开 */
    ngx_flag_t                       ignore_client_abort;
    ngx_flag_t                       intercept_errors;
    /* 复用临时文件中已使用过的空间 */
    ngx_flag_t                       cyclic_temp_file;

    /* 存放临时文件的目录 */
    ngx_path_t                      *temp_path;

    /* 不转发的头部 */
    ngx_hash_t                       hide_headers_hash;
    /*
     * 当转发上游响应头部到下游客户端时，
     * 若不希望将某些头部转发，则设置在这个数组中
     */
    ngx_array_t                     *hide_headers;
    /*
     * 当转发上游响应头部到下游客户端时，
     * 若希望将某些头部转发，则设置在这个数组中
     */
    ngx_array_t                     *pass_headers;

    /* 连接上游服务器的本机地址 */
    ngx_http_upstream_local_t       *local;

#if (NGX_HTTP_CACHE)
    ngx_shm_zone_t                  *cache;

    ngx_uint_t                       cache_min_uses;
    ngx_uint_t                       cache_use_stale;
    ngx_uint_t                       cache_methods;

    ngx_flag_t                       cache_lock;
    ngx_msec_t                       cache_lock_timeout;

    ngx_flag_t                       cache_revalidate;

    ngx_array_t                     *cache_valid;
    ngx_array_t                     *cache_bypass;
    ngx_array_t                     *no_cache;
#endif

    /*
     * 当ngx_http_upstream_t 中的store标志位为1时，
     * 如果需要将上游的响应存放在文件中，
     * store_lengths表示存放路径的长度；
     * store_values表示存放路径；
     */
    ngx_array_t                     *store_lengths;
    ngx_array_t                     *store_values;

    /* 文件缓存的路径 */
    signed                           store:2;
    /* 直接将上游返回的404错误码转发给下游 */
    unsigned                         intercept_404:1;
    /* 根据返回的响应头部，动态决定是以上游网速还是下游网速优先 */
    unsigned                         change_buffering:1;

#if (NGX_HTTP_SSL)
    ngx_ssl_t                       *ssl;
    ngx_flag_t                       ssl_session_reuse;
#endif

    /* 使用upstream的模块名称，仅用于记录日志 */
    ngx_str_t                        module;
} ngx_http_upstream_conf_t;


typedef struct {
    ngx_str_t                        name;
    ngx_http_header_handler_pt       handler;
    ngx_uint_t                       offset;
    ngx_http_header_handler_pt       copy_handler;
    ngx_uint_t                       conf;
    ngx_uint_t                       redirect;  /* unsigned   redirect:1; */
} ngx_http_upstream_header_t;


typedef struct {
    ngx_list_t                       headers;

    ngx_uint_t                       status_n;
    ngx_str_t                        status_line;

    ngx_table_elt_t                 *status;
    ngx_table_elt_t                 *date;
    ngx_table_elt_t                 *server;
    ngx_table_elt_t                 *connection;

    ngx_table_elt_t                 *expires;
    ngx_table_elt_t                 *etag;
    ngx_table_elt_t                 *x_accel_expires;
    ngx_table_elt_t                 *x_accel_redirect;
    ngx_table_elt_t                 *x_accel_limit_rate;

    ngx_table_elt_t                 *content_type;
    ngx_table_elt_t                 *content_length;

    ngx_table_elt_t                 *last_modified;
    ngx_table_elt_t                 *location;
    ngx_table_elt_t                 *accept_ranges;
    ngx_table_elt_t                 *www_authenticate;
    ngx_table_elt_t                 *transfer_encoding;

#if (NGX_HTTP_GZIP)
    ngx_table_elt_t                 *content_encoding;
#endif

    off_t                            content_length_n;

    ngx_array_t                      cache_control;

    unsigned                         connection_close:1;
    unsigned                         chunked:1;
} ngx_http_upstream_headers_in_t;


typedef struct {
    /* 主机名称 */
    ngx_str_t                        host;
    /* 端口号 */
    in_port_t                        port;
    ngx_uint_t                       no_port; /* unsigned no_port:1 */

    /* 地址个数 */
    ngx_uint_t                       naddrs;
    /* 地址 */
    ngx_addr_t                      *addrs;

    /* 上游服务器地址 */
    struct sockaddr                 *sockaddr;
    /* 上游服务器地址长度 */
    socklen_t                        socklen;

    ngx_resolver_ctx_t              *ctx;
} ngx_http_upstream_resolved_t;


typedef void (*ngx_http_upstream_handler_pt)(ngx_http_request_t *r,
    ngx_http_upstream_t *u);


/* ngx_http_upstream_t 结构体 */
struct ngx_http_upstream_s {
    /* 处理读事件的回调方法，每一个阶段都有不同的 read_event_handler */
    ngx_http_upstream_handler_pt     read_event_handler;
    /* 处理写事件的回调方法，每一个阶段都有不同的 write_event_handler */
    ngx_http_upstream_handler_pt     write_event_handler;

    /* 表示主动向上游服务器发起的连接 */
    ngx_peer_connection_t            peer;

    /*
     * 当向 下游客户端转发响应时（此时，ngx_http_request_t 结构体中的subrequest_in_memory标志位为0），
     * 若已打开缓存且认为上游网速更快，此时会使用pipe成员来转发响应；
     * 使用这种方式转发响应时，在HTTP模块使用upstream机制前必须构造pipe结构体；
     */
    ngx_event_pipe_t                *pipe;

    /* 发送给上游服务器的请求，在实现create_request方法时需设置它 */
    ngx_chain_t                     *request_bufs;

    /* 定义了向下游发送响应的方式 */
    ngx_output_chain_ctx_t           output;
    ngx_chain_writer_ctx_t           writer;

    /* 指定upstream机制的运行方式 */
    ngx_http_upstream_conf_t        *conf;

    /*
     * HTTP模块实现process_header方法时，若希望upstream直接转发响应，
     * 则需把解析出来的响应头部适配为HTTP的响应头部，同时需要把包头中的
     * 信息设置到headers_in结构体中
     */
    ngx_http_upstream_headers_in_t   headers_in;

    /* 解析主机域名，用于直接指定的上游服务器地址 */
    ngx_http_upstream_resolved_t    *resolved;

    /* 接收客户信息的缓冲区 */
    ngx_buf_t                        from_client;

    /*
     * 接收上游服务器响应包头的缓冲区，当不直接把响应转发给客户端，
     * 或buffering标志位为0的情况转发包体时，接收包体的缓冲区仍然使用buffer
     */
    ngx_buf_t                        buffer;
    off_t                            length;

    /*
     * out_bufs有两种不同意义：
     * 1、当不需要转发包体，且默认使用input_filter方法处理包体时，
     *    out_bufs将会指向响应包体，out_bufs链表中产生多个ngx_but_t缓冲区，
     *    每个缓冲区都指向buffer缓存中的一部分，而这里只是调用recv方法接收到的一段TCP流；
     * 2、当需要向下游转发包体时，这个链表指向上一次向下游转发响应到现在这段时间内接收自上游的缓存响应；
     */
    ngx_chain_t                     *out_bufs;
    /*
     * 当需要向下游转发响应包体时，它表示上一次向下游转发响应时没有发送完的内容；
     */
    ngx_chain_t                     *busy_bufs;
    /*
     * 这个链表用于回收out_bufs中已经发送给下游的ngx_buf_t结构体；
     */
    ngx_chain_t                     *free_bufs;

    /*
     * 处理包体前的初始化方法；
     * 其中data参数用于传递用户数据结构，就是下面成员input_filter_ctx
     */
    ngx_int_t                      (*input_filter_init)(void *data);
    /*
     * 处理包体的方法；
     * 其中data参数用于传递用户数据结构，就是下面成员input_filter_ctx，
     * bytes表示本次接收到包体的长度；
     */
    ngx_int_t                      (*input_filter)(void *data, ssize_t bytes);
    /* 用于传递HTTP自定义的数据结构 */
    void                            *input_filter_ctx;

#if (NGX_HTTP_CACHE)
    ngx_int_t                      (*create_key)(ngx_http_request_t *r);
#endif
    /* HTTP模块实现的create_request方法用于构造发往上游服务器的请求 */
    ngx_int_t                      (*create_request)(ngx_http_request_t *r);
    /* 与上游服务器的通信失败后，若想再次向上游服务器发起连接，则调用该函数 */
    ngx_int_t                      (*reinit_request)(ngx_http_request_t *r);
    /*
     * 解析上游服务器返回的响应包头，该函数返回四个值中的一个：
     * NGX_AGAIN                            表示包头没有接收完整；
     * NGX_HTTP_UPSTREAM_INVALID_HEADER     表示包头不合法；
     * NGX_ERROR                            表示出现错误；
     * NGX_OK                               表示解析到完整的包头；
     */
    ngx_int_t                      (*process_header)(ngx_http_request_t *r);
    /* 当客户端放弃请求时被调用，由于系统会自动关闭连接，因此，该函数不会进行任何具体操作 */
    void                           (*abort_request)(ngx_http_request_t *r);
    /* 结束upstream请求时会调用该函数 */
    void                           (*finalize_request)(ngx_http_request_t *r,
                                         ngx_int_t rc);
    /*
     * 在上游返回的响应出现location或者refresh头部表示重定向时，
     * 会通过ngx_http_upstream_process_headers方法调用到可由HTTP模块
     * 实现的rewrite_redirect方法；
     */
    ngx_int_t                      (*rewrite_redirect)(ngx_http_request_t *r,
                                         ngx_table_elt_t *h, size_t prefix);
    ngx_int_t                      (*rewrite_cookie)(ngx_http_request_t *r,
                                         ngx_table_elt_t *h);

    ngx_msec_t                       timeout;

    /* 用于表示上游响应的状态：错误编码、包体长度等信息 */
    ngx_http_upstream_state_t       *state;

    ngx_str_t                        method;
    /* 用于记录日志 */
    ngx_str_t                        schema;
    ngx_str_t                        uri;

    /* 清理资源 */
    ngx_http_cleanup_pt             *cleanup;

    /* 以下是一些标志位 */

    /* 指定文件缓存路径 */
    unsigned                         store:1;
    /* 启用文件缓存 */
    unsigned                         cacheable:1;
    unsigned                         accel:1;
    /* 基于ssl协议访问上游服务器 */
    unsigned                         ssl:1;
#if (NGX_HTTP_CACHE)
    unsigned                         cache_status:3;
#endif

    /* 开启更大的内存及临时磁盘文件用于缓存来不及发送到下游的响应包体 */
    unsigned                         buffering:1;
    /* keepalive机制 */
    unsigned                         keepalive:1;
    unsigned                         upgrade:1;

    /* 表示是否已向上游服务器发送请求 */
    unsigned                         request_sent:1;
    /* 表示是否已经转发响应报头 */
    unsigned                         header_sent:1;
};


typedef struct {
    ngx_uint_t                      status;
    ngx_uint_t                      mask;
} ngx_http_upstream_next_t;


typedef struct {
    ngx_str_t   key;
    ngx_str_t   value;
    ngx_uint_t  skip_empty;
} ngx_http_upstream_param_t;


ngx_int_t ngx_http_upstream_header_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);

ngx_int_t ngx_http_upstream_create(ngx_http_request_t *r);
void ngx_http_upstream_init(ngx_http_request_t *r);
ngx_http_upstream_srv_conf_t *ngx_http_upstream_add(ngx_conf_t *cf,
    ngx_url_t *u, ngx_uint_t flags);
char *ngx_http_upstream_bind_set_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
char *ngx_http_upstream_param_set_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
ngx_int_t ngx_http_upstream_hide_headers_hash(ngx_conf_t *cf,
    ngx_http_upstream_conf_t *conf, ngx_http_upstream_conf_t *prev,
    ngx_str_t *default_hide_headers, ngx_hash_init_t *hash);


#define ngx_http_conf_upstream_srv_conf(uscf, module)                         \
    uscf->srv_conf[module.ctx_index]


extern ngx_module_t        ngx_http_upstream_module;
extern ngx_conf_bitmask_t  ngx_http_upstream_cache_method_mask[];
extern ngx_conf_bitmask_t  ngx_http_upstream_ignore_headers_masks[];


#endif /* _NGX_HTTP_UPSTREAM_H_INCLUDED_ */
