
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HTTP_REQUEST_H_INCLUDED_
#define _NGX_HTTP_REQUEST_H_INCLUDED_


#define NGX_HTTP_MAX_URI_CHANGES           10
#define NGX_HTTP_MAX_SUBREQUESTS           200

/* must be 2^n */
#define NGX_HTTP_LC_HEADER_LEN             32


#define NGX_HTTP_DISCARD_BUFFER_SIZE       4096
#define NGX_HTTP_LINGERING_BUFFER_SIZE     4096


#define NGX_HTTP_VERSION_9                 9
#define NGX_HTTP_VERSION_10                1000
#define NGX_HTTP_VERSION_11                1001

#define NGX_HTTP_UNKNOWN                   0x0001
#define NGX_HTTP_GET                       0x0002
#define NGX_HTTP_HEAD                      0x0004
#define NGX_HTTP_POST                      0x0008
#define NGX_HTTP_PUT                       0x0010
#define NGX_HTTP_DELETE                    0x0020
#define NGX_HTTP_MKCOL                     0x0040
#define NGX_HTTP_COPY                      0x0080
#define NGX_HTTP_MOVE                      0x0100
#define NGX_HTTP_OPTIONS                   0x0200
#define NGX_HTTP_PROPFIND                  0x0400
#define NGX_HTTP_PROPPATCH                 0x0800
#define NGX_HTTP_LOCK                      0x1000
#define NGX_HTTP_UNLOCK                    0x2000
#define NGX_HTTP_PATCH                     0x4000
#define NGX_HTTP_TRACE                     0x8000

#define NGX_HTTP_CONNECTION_CLOSE          1
#define NGX_HTTP_CONNECTION_KEEP_ALIVE     2


#define NGX_NONE                           1


#define NGX_HTTP_PARSE_HEADER_DONE         1

#define NGX_HTTP_CLIENT_ERROR              10
#define NGX_HTTP_PARSE_INVALID_METHOD      10
#define NGX_HTTP_PARSE_INVALID_REQUEST     11
#define NGX_HTTP_PARSE_INVALID_09_METHOD   12

#define NGX_HTTP_PARSE_INVALID_HEADER      13


/* unused                                  1 */
#define NGX_HTTP_SUBREQUEST_IN_MEMORY      2
#define NGX_HTTP_SUBREQUEST_WAITED         4
#define NGX_HTTP_LOG_UNSAFE                8


#define NGX_HTTP_CONTINUE                  100
#define NGX_HTTP_SWITCHING_PROTOCOLS       101
#define NGX_HTTP_PROCESSING                102

#define NGX_HTTP_OK                        200
#define NGX_HTTP_CREATED                   201
#define NGX_HTTP_ACCEPTED                  202
#define NGX_HTTP_NO_CONTENT                204
#define NGX_HTTP_PARTIAL_CONTENT           206

#define NGX_HTTP_SPECIAL_RESPONSE          300
#define NGX_HTTP_MOVED_PERMANENTLY         301
#define NGX_HTTP_MOVED_TEMPORARILY         302
#define NGX_HTTP_SEE_OTHER                 303
#define NGX_HTTP_NOT_MODIFIED              304
#define NGX_HTTP_TEMPORARY_REDIRECT        307

#define NGX_HTTP_BAD_REQUEST               400
#define NGX_HTTP_UNAUTHORIZED              401
#define NGX_HTTP_FORBIDDEN                 403
#define NGX_HTTP_NOT_FOUND                 404
#define NGX_HTTP_NOT_ALLOWED               405
#define NGX_HTTP_REQUEST_TIME_OUT          408
#define NGX_HTTP_CONFLICT                  409
#define NGX_HTTP_LENGTH_REQUIRED           411
#define NGX_HTTP_PRECONDITION_FAILED       412
#define NGX_HTTP_REQUEST_ENTITY_TOO_LARGE  413
#define NGX_HTTP_REQUEST_URI_TOO_LARGE     414
#define NGX_HTTP_UNSUPPORTED_MEDIA_TYPE    415
#define NGX_HTTP_RANGE_NOT_SATISFIABLE     416


/* Our own HTTP codes */

/* The special code to close connection without any response */
#define NGX_HTTP_CLOSE                     444

#define NGX_HTTP_NGINX_CODES               494

#define NGX_HTTP_REQUEST_HEADER_TOO_LARGE  494

#define NGX_HTTPS_CERT_ERROR               495
#define NGX_HTTPS_NO_CERT                  496

/*
 * We use the special code for the plain HTTP requests that are sent to
 * HTTPS port to distinguish it from 4XX in an error page redirection
 */
#define NGX_HTTP_TO_HTTPS                  497

/* 498 is the canceled code for the requests with invalid host name */

/*
 * HTTP does not define the code for the case when a client closed
 * the connection while we are processing its request so we introduce
 * own code to log such situation when a client has closed the connection
 * before we even try to send the HTTP header to it
 */
#define NGX_HTTP_CLIENT_CLOSED_REQUEST     499


#define NGX_HTTP_INTERNAL_SERVER_ERROR     500
#define NGX_HTTP_NOT_IMPLEMENTED           501
#define NGX_HTTP_BAD_GATEWAY               502
#define NGX_HTTP_SERVICE_UNAVAILABLE       503
#define NGX_HTTP_GATEWAY_TIME_OUT          504
#define NGX_HTTP_INSUFFICIENT_STORAGE      507


#define NGX_HTTP_LOWLEVEL_BUFFERED         0xf0
#define NGX_HTTP_WRITE_BUFFERED            0x10
#define NGX_HTTP_GZIP_BUFFERED             0x20
#define NGX_HTTP_SSI_BUFFERED              0x01
#define NGX_HTTP_SUB_BUFFERED              0x02
#define NGX_HTTP_COPY_BUFFERED             0x04


typedef enum {
    NGX_HTTP_INITING_REQUEST_STATE = 0,
    NGX_HTTP_READING_REQUEST_STATE,
    NGX_HTTP_PROCESS_REQUEST_STATE,

    NGX_HTTP_CONNECT_UPSTREAM_STATE,
    NGX_HTTP_WRITING_UPSTREAM_STATE,
    NGX_HTTP_READING_UPSTREAM_STATE,

    NGX_HTTP_WRITING_REQUEST_STATE,
    NGX_HTTP_LINGERING_CLOSE_STATE,
    NGX_HTTP_KEEPALIVE_STATE
} ngx_http_state_e;


typedef struct {
    ngx_str_t                         name;
    ngx_uint_t                        offset;
    ngx_http_header_handler_pt        handler;
} ngx_http_header_t;


typedef struct {
    ngx_str_t                         name;
    ngx_uint_t                        offset;
} ngx_http_header_out_t;


typedef struct {
    ngx_list_t                        headers;

    ngx_table_elt_t                  *host;
    ngx_table_elt_t                  *connection;
    ngx_table_elt_t                  *if_modified_since;
    ngx_table_elt_t                  *if_unmodified_since;
    ngx_table_elt_t                  *if_match;
    ngx_table_elt_t                  *if_none_match;
    ngx_table_elt_t                  *user_agent;
    ngx_table_elt_t                  *referer;
    ngx_table_elt_t                  *content_length;
    ngx_table_elt_t                  *content_type;

    ngx_table_elt_t                  *range;
    ngx_table_elt_t                  *if_range;

    ngx_table_elt_t                  *transfer_encoding;
    ngx_table_elt_t                  *expect;
    ngx_table_elt_t                  *upgrade;

#if (NGX_HTTP_GZIP)
    ngx_table_elt_t                  *accept_encoding;
    ngx_table_elt_t                  *via;
#endif

    ngx_table_elt_t                  *authorization;

    ngx_table_elt_t                  *keep_alive;

#if (NGX_HTTP_X_FORWARDED_FOR)
    ngx_array_t                       x_forwarded_for;
#endif

#if (NGX_HTTP_REALIP)
    ngx_table_elt_t                  *x_real_ip;
#endif

#if (NGX_HTTP_HEADERS)
    ngx_table_elt_t                  *accept;
    ngx_table_elt_t                  *accept_language;
#endif

#if (NGX_HTTP_DAV)
    ngx_table_elt_t                  *depth;
    ngx_table_elt_t                  *destination;
    ngx_table_elt_t                  *overwrite;
    ngx_table_elt_t                  *date;
#endif

    ngx_str_t                         user;
    ngx_str_t                         passwd;

    ngx_array_t                       cookies;

    ngx_str_t                         server;
    off_t                             content_length_n;
    time_t                            keep_alive_n;

    unsigned                          connection_type:2;
    unsigned                          chunked:1;
    unsigned                          msie:1;
    unsigned                          msie6:1;
    unsigned                          opera:1;
    unsigned                          gecko:1;
    unsigned                          chrome:1;
    unsigned                          safari:1;
    unsigned                          konqueror:1;
} ngx_http_headers_in_t;


typedef struct {
    ngx_list_t                        headers;

    ngx_uint_t                        status;
    ngx_str_t                         status_line;

    ngx_table_elt_t                  *server;
    ngx_table_elt_t                  *date;
    ngx_table_elt_t                  *content_length;
    ngx_table_elt_t                  *content_encoding;
    ngx_table_elt_t                  *location;
    ngx_table_elt_t                  *refresh;
    ngx_table_elt_t                  *last_modified;
    ngx_table_elt_t                  *content_range;
    ngx_table_elt_t                  *accept_ranges;
    ngx_table_elt_t                  *www_authenticate;
    ngx_table_elt_t                  *expires;
    ngx_table_elt_t                  *etag;

    ngx_str_t                        *override_charset;

    size_t                            content_type_len;
    ngx_str_t                         content_type;
    ngx_str_t                         charset;
    u_char                           *content_type_lowcase;
    ngx_uint_t                        content_type_hash;

    ngx_array_t                       cache_control;

    off_t                             content_length_n;
    time_t                            date_time;
    time_t                            last_modified_time;
} ngx_http_headers_out_t;


typedef void (*ngx_http_client_body_handler_pt)(ngx_http_request_t *r);

typedef struct {
    ngx_temp_file_t                  *temp_file;
    ngx_chain_t                      *bufs;
    ngx_buf_t                        *buf;
    off_t                             rest;
    ngx_chain_t                      *free;
    ngx_chain_t                      *busy;
    ngx_http_chunked_t               *chunked;
    ngx_http_client_body_handler_pt   post_handler;
} ngx_http_request_body_t;


typedef struct ngx_http_addr_conf_s  ngx_http_addr_conf_t;

typedef struct {
    ngx_http_addr_conf_t             *addr_conf;
    ngx_http_conf_ctx_t              *conf_ctx;

#if (NGX_HTTP_SSL && defined SSL_CTRL_SET_TLSEXT_HOSTNAME)
    ngx_str_t                        *ssl_servername;
#if (NGX_PCRE)
    ngx_http_regex_t                 *ssl_servername_regex;
#endif
#endif

    ngx_buf_t                       **busy;
    ngx_int_t                         nbusy;

    ngx_buf_t                       **free;
    ngx_int_t                         nfree;

#if (NGX_HTTP_SSL)
    unsigned                          ssl:1;
#endif
    unsigned                          proxy_protocol:1;
} ngx_http_connection_t;


typedef void (*ngx_http_cleanup_pt)(void *data);

typedef struct ngx_http_cleanup_s  ngx_http_cleanup_t;

struct ngx_http_cleanup_s {
    ngx_http_cleanup_pt               handler;
    void                             *data;
    ngx_http_cleanup_t               *next;
};


typedef ngx_int_t (*ngx_http_post_subrequest_pt)(ngx_http_request_t *r,
    void *data, ngx_int_t rc);

typedef struct {
    ngx_http_post_subrequest_pt       handler;
    void                             *data;
} ngx_http_post_subrequest_t;


typedef struct ngx_http_postponed_request_s  ngx_http_postponed_request_t;

struct ngx_http_postponed_request_s {
    ngx_http_request_t               *request;
    ngx_chain_t                      *out;
    ngx_http_postponed_request_t     *next;
};


typedef struct ngx_http_posted_request_s  ngx_http_posted_request_t;

struct ngx_http_posted_request_s {
    ngx_http_request_t               *request;
    ngx_http_posted_request_t        *next;
};


typedef ngx_int_t (*ngx_http_handler_pt)(ngx_http_request_t *r);
typedef void (*ngx_http_event_handler_pt)(ngx_http_request_t *r);


struct ngx_http_request_s {
    uint32_t                          signature;         /* "HTTP" */

    /* 客户端连接 */
    ngx_connection_t                 *connection;

    /*
     * 以下四个成员是保存模块对应的上下文结构指针；
     * ctx 对应的是自定义的上下文结构指针；
     * main_conf 对应的是main级别配置结构体的指针；
     * srv_conf 对应的是server级别配置结构体的指针；
     * loc_conf 对应的是location级别配置结构体的指针；
     */
    void                            **ctx;
    void                            **main_conf;
    void                            **srv_conf;
    void                            **loc_conf;

    /*
     * 以下两个是处理http请求；
     * 当http头部接收完毕，第一次在业务上处理http请求时，http框架提供的处理方法是ngx_http_process_request;
     * 若该方法无法一次性处理完该请求的全部业务时，当控制权归还给epoll事件模块后，该请求再次被回调，
     * 此时，将通过ngx_http_request_handler方法进行处理，而这个方法中对于可读或可写事件的处理就是由函数
     * read_event_handler或write_event_handler 来处理请求；
     */
    ngx_http_event_handler_pt         read_event_handler;
    ngx_http_event_handler_pt         write_event_handler;

#if (NGX_HTTP_CACHE)
    ngx_http_cache_t                 *cache;
#endif

    /* 若使用upstream机制，则需要以下的结构体 */
    ngx_http_upstream_t              *upstream;
    ngx_array_t                      *upstream_states;
                                         /* of ngx_http_upstream_state_t */

    /* 内存池 */
    ngx_pool_t                       *pool;
    /* 主要用于接收http请求头部内容的缓冲区 */
    ngx_buf_t                        *header_in;


    /*
     * 调用函数ngx_http_request_headers 接收并解析http请求头部完毕后，
     * 则把解析完成的每一个http头部加入到结构体headers_in的成员headers链表中，
     * 同时初始化该结构体的其他成员；
     */
    ngx_http_headers_in_t             headers_in;
    /*
     * http模块将待发送的http相应的信息存放在headers_out中，
     * 并期望http框架将headers_out中的成员序列化为http响应包体发送个客户端；
     */
    ngx_http_headers_out_t            headers_out;

    /* 接收请求包体的数据结构 */
    ngx_http_request_body_t          *request_body;

    /* 延迟关闭连接的时间 */
    time_t                            lingering_time;
    /* 当前请求初始化的时间 */
    time_t                            start_sec;
    ngx_msec_t                        start_msec;

    /*
     * 以下的 9 个成员是函数ngx_http_process_request_line在接收、解析http请求行时解析出的信息 */
    ngx_uint_t                        method;       /* 方法名称 */
    ngx_uint_t                        http_version; /* 协议版本 */

    ngx_str_t                         request_line; /* 请求行 */
    ngx_str_t                         uri;          /* 客户请求中的uri */
    ngx_str_t                         args;         /* uri 中的参数 */
    ngx_str_t                         exten;        /* 客户请求的文件扩展名 */
    ngx_str_t                         unparsed_uri; /* 没经过URI 解码的原始请求 */

    ngx_str_t                         method_name;  /* 方法名称字符串 */
    ngx_str_t                         http_protocol;/* 其data成员指向请求中http的起始地址 */

    /*
     * 存储待发送给客户的http响应；
     * out保存着由headers_out序列化后的表示http头部的TCP流；
     * 调用ngx_http_output_filter方法后，out还保存这待发送的http包体；
     */
    ngx_chain_t                      *out;
    /*
     * 当前请求可能是用户请求，或是派生的子请求；
     * main标识一序列相关的派生子请求的原始请求；
     * 即通过main与当前请求的地址对比来判断是用户请求还是派生子请求；
     */
    ngx_http_request_t               *main;
    /*
     * 当前请求的父亲请求，但不一定是原始请求 */
    ngx_http_request_t               *parent;
    /* 以下两个是与subrequest子请求相关的功能 */
    ngx_http_postponed_request_t     *postponed;
    ngx_http_post_subrequest_t       *post_subrequest;
    /* 连接子请求的链表 */
    ngx_http_posted_request_t        *posted_requests;

    /*
     * 全局结构体ngx_http_phase_engine_t定义了一个ngx_http_phase_handler_t回调方法的数组；
     * 而这里的phase_handler作为该数组的序列号表示指定数组中的回调方法，相当于数组的下标；
     */
    ngx_int_t                         phase_handler;
    /*
     * 表示NGX_HTTP_CONTENT_PHASE阶段提供给http模块请求的一种方式，它指向http模块实现的请求处理方法 */
    ngx_http_handler_pt               content_handler;
    /*
     * 在NGX——HTTP_CONTENT_PHASE阶段需要判断请求是否具有访问权限时，
     * 可通过access_code来传递http模块的handler回调方法的返回值来判断，
     * 若为0表示具备权限，否则不具备；
     */
    ngx_uint_t                        access_code;

    ngx_http_variable_value_t        *variables;

#if (NGX_PCRE)
    ngx_uint_t                        ncaptures;
    int                              *captures;
    u_char                           *captures_data;
#endif

    /* 限制当前请求的发送的速率 */
    size_t                            limit_rate;
    size_t                            limit_rate_after;

    /* http响应的长度，不包括http响应头部 */
    /* used to learn the Apache compatible response length without a header */
    size_t                            header_size;

    /* http请求的长度，包括http请求头部、http请求包体 */
    off_t                             request_length;

    /* 表示错误状态标志 */
    ngx_uint_t                        err_status;

    /* http 连接 */
    ngx_http_connection_t            *http_connection;
#if (NGX_HTTP_SPDY)
    ngx_http_spdy_stream_t           *spdy_stream;
#endif

    /* http日志处理函数 */
    ngx_http_log_handler_pt           log_handler;

    /* 释放资源 */
    ngx_http_cleanup_t               *cleanup;

    /* 以下都是一些标志位 */
    /* 派生子请求 */
    unsigned                          subrequests:8;
    /* 作为原始请求的引用计数，每派生一个子请求，原始请求的成员count会增加1 */
    unsigned                          count:8;
    /* 阻塞标志位 */
    unsigned                          blocked:8;

    /* 标志位：为1表示当前请求是异步IO方式 */
    unsigned                          aio:1;

    unsigned                          http_state:4;

    /* URI with "/." and on Win32 with "//" */
    unsigned                          complex_uri:1;

    /* URI with "%" */
    unsigned                          quoted_uri:1;

    /* URI with "+" */
    unsigned                          plus_in_uri:1;

    /* URI with " " */
    unsigned                          space_in_uri:1;

    unsigned                          invalid_header:1;

    unsigned                          add_uri_to_alias:1;
    unsigned                          valid_location:1;
    unsigned                          valid_unparsed_uri:1;
    /* 标志位：为1表示URI已经被重写 */
    unsigned                          uri_changed:1;
    /* 表示URI被重写的次数 */
    unsigned                          uri_changes:4;

    unsigned                          request_body_in_single_buf:1;
    unsigned                          request_body_in_file_only:1;
    unsigned                          request_body_in_persistent_file:1;
    unsigned                          request_body_in_clean_file:1;
    unsigned                          request_body_file_group_access:1;
    unsigned                          request_body_file_log_level:3;

    unsigned                          subrequest_in_memory:1;
    unsigned                          waited:1;

#if (NGX_HTTP_CACHE)
    unsigned                          cached:1;
#endif

#if (NGX_HTTP_GZIP)
    unsigned                          gzip_tested:1;
    unsigned                          gzip_ok:1;
    unsigned                          gzip_vary:1;
#endif

    unsigned                          proxy:1;
    unsigned                          bypass_cache:1;
    unsigned                          no_cache:1;

    /*
     * instead of using the request context data in
     * ngx_http_limit_conn_module and ngx_http_limit_req_module
     * we use the single bits in the request structure
     */
    unsigned                          limit_conn_set:1;
    unsigned                          limit_req_set:1;

#if 0
    unsigned                          cacheable:1;
#endif

    unsigned                          pipeline:1;
    unsigned                          chunked:1;
    unsigned                          header_only:1;
    unsigned                          keepalive:1;
    unsigned                          lingering_close:1;
    unsigned                          discard_body:1;
    unsigned                          internal:1;
    unsigned                          error_page:1;
    unsigned                          ignore_content_encoding:1;
    unsigned                          filter_finalize:1;
    unsigned                          post_action:1;
    unsigned                          request_complete:1;
    unsigned                          request_output:1;
    unsigned                          header_sent:1;
    unsigned                          expect_tested:1;
    unsigned                          root_tested:1;
    unsigned                          done:1;
    unsigned                          logged:1;

    unsigned                          buffered:4;

    unsigned                          main_filter_need_in_memory:1;
    unsigned                          filter_need_in_memory:1;
    unsigned                          filter_need_temporary:1;
    unsigned                          allow_ranges:1;
    unsigned                          single_range:1;

#if (NGX_STAT_STUB)
    unsigned                          stat_reading:1;
    unsigned                          stat_writing:1;
#endif

    /* used to parse HTTP headers */

    /* 当前的解析状态 */
    ngx_uint_t                        state;

    ngx_uint_t                        header_hash;
    ngx_uint_t                        lowcase_index;
    u_char                            lowcase_header[NGX_HTTP_LC_HEADER_LEN];

    u_char                           *header_name_start;
    u_char                           *header_name_end;
    u_char                           *header_start;
    u_char                           *header_end;

    /*
     * a memory that can be reused after parsing a request line
     * via ngx_http_ephemeral_t
     */

    u_char                           *uri_start;
    u_char                           *uri_end;
    u_char                           *uri_ext;
    u_char                           *args_start;
    u_char                           *request_start;
    u_char                           *request_end;
    u_char                           *method_end;
    u_char                           *schema_start;
    u_char                           *schema_end;
    u_char                           *host_start;
    u_char                           *host_end;
    u_char                           *port_start;
    u_char                           *port_end;

    unsigned                          http_minor:16;
    unsigned                          http_major:16;
};


typedef struct {
    ngx_http_posted_request_t         terminal_posted_request;
#if (NGX_HAVE_AIO_SENDFILE)
    u_char                            aio_preload;
#endif
} ngx_http_ephemeral_t;


extern ngx_http_header_t       ngx_http_headers_in[];
extern ngx_http_header_out_t   ngx_http_headers_out[];


#define ngx_http_set_connection_log(c, l)                                     \
                                                                              \
    c->log->file = l->file;                                                   \
    c->log->next = l->next;                                                   \
    if (!(c->log->log_level & NGX_LOG_DEBUG_CONNECTION)) {                    \
        c->log->log_level = l->log_level;                                     \
    }


#endif /* _NGX_HTTP_REQUEST_H_INCLUDED_ */
