
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HTTP_UPSTREAM_ROUND_ROBIN_H_INCLUDED_
#define _NGX_HTTP_UPSTREAM_ROUND_ROBIN_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    /* 后端服务器 IP 地址 */
    struct sockaddr                *sockaddr;
    /* 后端服务器 IP 地址的长度 */
    socklen_t                       socklen;
    /* 后端服务器的名称 */
    ngx_str_t                       name;

    /* 后端服务器当前的权重 */
    ngx_int_t                       current_weight;
    /* 后端服务器有效权重 */
    ngx_int_t                       effective_weight;
    /* 配置项所指定的权重 */
    ngx_int_t                       weight;

    /* 已经失败的次数 */
    ngx_uint_t                      fails;
    /* 访问时间 */
    time_t                          accessed;
    time_t                          checked;

    /* 最大失败次数 */
    ngx_uint_t                      max_fails;
    /* 失败时间阈值 */
    time_t                          fail_timeout;

    /* 后端服务器是否参与策略，若为1，表示不参与 */
    ngx_uint_t                      down;          /* unsigned  down:1; */

#if (NGX_HTTP_SSL)
    ngx_ssl_session_t              *ssl_session;   /* local to a process */
#endif
} ngx_http_upstream_rr_peer_t;


typedef struct ngx_http_upstream_rr_peers_s  ngx_http_upstream_rr_peers_t;

struct ngx_http_upstream_rr_peers_s {
    /* 竞选队列中后端服务器的数量 */
    ngx_uint_t                      number;

 /* ngx_mutex_t                    *mutex; */

    /* 所有后端服务器总的权重 */
    ngx_uint_t                      total_weight;

    /* 标志位，若为 1，表示后端服务器仅有一台，此时不需要选择策略 */
    unsigned                        single:1;
    /* 标志位，若为 1，表示所有后端服务器总的权重等于服务器的数量 */
    unsigned                        weighted:1;

    ngx_str_t                      *name;

    /* 后端服务器的链表 */
    ngx_http_upstream_rr_peers_t   *next;

    /* 特定的后端服务器 */
    ngx_http_upstream_rr_peer_t     peer[1];
};


typedef struct {
    ngx_http_upstream_rr_peers_t   *peers;
    ngx_uint_t                      current;
    uintptr_t                      *tried;
    uintptr_t                       data;
} ngx_http_upstream_rr_peer_data_t;


ngx_int_t ngx_http_upstream_init_round_robin(ngx_conf_t *cf,
    ngx_http_upstream_srv_conf_t *us);
ngx_int_t ngx_http_upstream_init_round_robin_peer(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us);
ngx_int_t ngx_http_upstream_create_round_robin_peer(ngx_http_request_t *r,
    ngx_http_upstream_resolved_t *ur);
ngx_int_t ngx_http_upstream_get_round_robin_peer(ngx_peer_connection_t *pc,
    void *data);
void ngx_http_upstream_free_round_robin_peer(ngx_peer_connection_t *pc,
    void *data, ngx_uint_t state);

#if (NGX_HTTP_SSL)
ngx_int_t
    ngx_http_upstream_set_round_robin_peer_session(ngx_peer_connection_t *pc,
    void *data);
void ngx_http_upstream_save_round_robin_peer_session(ngx_peer_connection_t *pc,
    void *data);
#endif


#endif /* _NGX_HTTP_UPSTREAM_ROUND_ROBIN_H_INCLUDED_ */
