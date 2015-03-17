
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_EVENT_PIPE_H_INCLUDED_
#define _NGX_EVENT_PIPE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


typedef struct ngx_event_pipe_s  ngx_event_pipe_t;

/* 接收来自上游的响应包体的回调方法 */
typedef ngx_int_t (*ngx_event_pipe_input_filter_pt)(ngx_event_pipe_t *p,
                                                    ngx_buf_t *buf);
/* 发送响应包体给下游的回调方法 */
typedef ngx_int_t (*ngx_event_pipe_output_filter_pt)(void *data,
                                                     ngx_chain_t *chain);


struct ngx_event_pipe_s {
    /* Nginx与上游之间的TCP连接 */
    ngx_connection_t  *upstream;
    /* Nginx与下游之间的TCP连接 */
    ngx_connection_t  *downstream;

    /* 直接接收来自上游服务器响应的缓冲区链表 */
    ngx_chain_t       *free_raw_bufs;
    /* 表示接收到的上游响应缓冲区 */
    ngx_chain_t       *in;
    /* 指向最新接收到响应的缓冲区 */
    ngx_chain_t      **last_in;

    /* 保存着将要发送给下游的响应缓冲区链表 */
    ngx_chain_t       *out;
    /* 等待释放的缓冲区 */
    ngx_chain_t       *free;
    /* 上一次未发送完毕的响应缓冲区链表 */
    ngx_chain_t       *busy;

    /*
     * the input filter i.e. that moves HTTP/1.1 chunks
     * from the raw bufs to an incoming chain
     */

    /* 处理来自上游服务器的响应 */
    ngx_event_pipe_input_filter_pt    input_filter;
    void                             *input_ctx;

    /* 向下游发送响应的方法 */
    ngx_event_pipe_output_filter_pt   output_filter;
    void                             *output_ctx;

    /* 表示当前已经读取到上游的响应 */
    unsigned           read:1;
    /* 启用文件缓存 */
    unsigned           cacheable:1;
    /* 表示一次只能接收一个ngx_buf_t缓冲区的上游响应 */
    unsigned           single_buf:1;
    /* 当不再接收上游响应时，尽量及时释放缓冲区 */
    unsigned           free_bufs:1;
    /* 表示Nginx与上游之间的通信已经结束 */
    unsigned           upstream_done:1;
    /* 表示Nginx与上游之间的连接出错 */
    unsigned           upstream_error:1;
    /* 表示Nginx与上游之间的连接已经关闭 */
    unsigned           upstream_eof:1;
    /* 阻塞读取上游响应的操作 */
    unsigned           upstream_blocked:1;
    /* 表示Nginx与下游之间的通信已经结束 */
    unsigned           downstream_done:1;
    /* 表示Nginx与下游之间的连接出错 */
    unsigned           downstream_error:1;
    /* 尝试复用临时文件曾经使用过的空间 */
    unsigned           cyclic_temp_file:1;

    /* 表示已经分配的缓冲区数目 */
    ngx_int_t          allocated;
    /* 接收上游响应的缓冲区大小 */
    ngx_bufs_t         bufs;
    ngx_buf_tag_t      tag;

    /* 设置busy缓冲区中待发送的响应长度 */
    ssize_t            busy_size;

    /* 已经接收到的上游响应包体长度 */
    off_t              read_length;
    off_t              length;

    /* 临时文件的最大长度 */
    off_t              max_temp_file_size;
    /* 每次写入临时文件的最大长度 */
    ssize_t            temp_file_write_size;

    /* 读取上游响应的超时时间 */
    ngx_msec_t         read_timeout;
    /* 向下游发送响应的超时时间 */
    ngx_msec_t         send_timeout;
    ssize_t            send_lowat;

    /* 连接池 */
    ngx_pool_t        *pool;
    /* 记录日志 */
    ngx_log_t         *log;

    /* 表示在接收上游响应头部区间，已经读取到的响应包体 */
    ngx_chain_t       *preread_bufs;
    /* 表示在接收上游响应头部区间，已经读取到的响应包体长度 */
    size_t             preread_size;
    ngx_buf_t         *buf_to_file;

    /* 存放上游响应的临时文件 */
    ngx_temp_file_t   *temp_file;

    /* 已经使用的缓冲区数目 */
    /* STUB */ int     num;
};


ngx_int_t ngx_event_pipe(ngx_event_pipe_t *p, ngx_int_t do_write);
ngx_int_t ngx_event_pipe_copy_input_filter(ngx_event_pipe_t *p, ngx_buf_t *buf);
ngx_int_t ngx_event_pipe_add_free_buf(ngx_event_pipe_t *p, ngx_buf_t *b);


#endif /* _NGX_EVENT_PIPE_H_INCLUDED_ */
