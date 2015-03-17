
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_EVENT_CONNECT_H_INCLUDED_
#define _NGX_EVENT_CONNECT_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


#define NGX_PEER_KEEPALIVE           1
#define NGX_PEER_NEXT                2
#define NGX_PEER_FAILED              4


typedef struct ngx_peer_connection_s  ngx_peer_connection_t;

typedef ngx_int_t (*ngx_event_get_peer_pt)(ngx_peer_connection_t *pc,
    void *data);
typedef void (*ngx_event_free_peer_pt)(ngx_peer_connection_t *pc, void *data,
    ngx_uint_t state);
#if (NGX_SSL)

typedef ngx_int_t (*ngx_event_set_peer_session_pt)(ngx_peer_connection_t *pc,
    void *data);
typedef void (*ngx_event_save_peer_session_pt)(ngx_peer_connection_t *pc,
    void *data);
#endif


/* 主动连接的结构体 */
struct ngx_peer_connection_s {
    /* 这里是对ngx_connection_t连接结构体的引用 */
    ngx_connection_t                *connection;

    /* 远端服务器的socket的地址sockaddr信息 */
    struct sockaddr                 *sockaddr;
    socklen_t                        socklen;
    /* 远端服务器的名称 */
    ngx_str_t                       *name;

    /* 连接重试的次数 */
    ngx_uint_t                       tries;

    /* 获取连接的方法 */
    ngx_event_get_peer_pt            get;
    /* 释放连接的方法 */
    ngx_event_free_peer_pt           free;
    /* 配合get、free使用 */
    void                            *data;

#if (NGX_SSL)
    ngx_event_set_peer_session_pt    set_session;
    ngx_event_save_peer_session_pt   save_session;
#endif

#if (NGX_THREADS)
    ngx_atomic_t                    *lock;
#endif

    /* 本地地址信息 */
    ngx_addr_t                      *local;

    /* 接收缓冲区 */
    int                              rcvbuf;

    /* 记录日志 */
    ngx_log_t                       *log;

    /* 标志位，为1表示connection连接已经缓存 */
    unsigned                         cached:1;

                                     /* ngx_connection_log_error_e */
    unsigned                         log_error:2;
};


ngx_int_t ngx_event_connect_peer(ngx_peer_connection_t *pc);
ngx_int_t ngx_event_get_peer(ngx_peer_connection_t *pc, void *data);


#endif /* _NGX_EVENT_CONNECT_H_INCLUDED_ */