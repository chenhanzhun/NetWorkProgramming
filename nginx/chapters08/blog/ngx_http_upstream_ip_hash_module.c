
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    /* the round robin data must be first */
    ngx_http_upstream_rr_peer_data_t   rrp;

    ngx_uint_t                         hash;

    /* IP 地址及其长度 */
    u_char                             addrlen;
    u_char                            *addr;

    /* 重试连接的次数 */
    u_char                             tries;

    ngx_event_get_peer_pt              get_rr_peer;
} ngx_http_upstream_ip_hash_peer_data_t;
/************** 该结构体不属于该文件 ***********/
typedef struct {
    /* 指向后端服务器列表 */
    ngx_http_upstream_rr_peers_t   *peers;
    /* 当前后端服务器 */
    ngx_uint_t                      current;
    /* 指向后端服务器列表位图 */
    uintptr_t                      *tried;
    /* 位图的实际存储位置 */
    uintptr_t                       data;
} ngx_http_upstream_rr_peer_data_t;
/*****************************************/

static ngx_int_t ngx_http_upstream_init_ip_hash_peer(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us);
static ngx_int_t ngx_http_upstream_get_ip_hash_peer(ngx_peer_connection_t *pc,
    void *data);
static char *ngx_http_upstream_ip_hash(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);


static ngx_command_t  ngx_http_upstream_ip_hash_commands[] = {

    { ngx_string("ip_hash"),
      NGX_HTTP_UPS_CONF|NGX_CONF_NOARGS,
      ngx_http_upstream_ip_hash,
      0,
      0,
      NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_upstream_ip_hash_module_ctx = {
    NULL,                                  /* preconfiguration */
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};


ngx_module_t  ngx_http_upstream_ip_hash_module = {
    NGX_MODULE_V1,
    &ngx_http_upstream_ip_hash_module_ctx, /* module context */
    ngx_http_upstream_ip_hash_commands,    /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static u_char ngx_http_upstream_ip_hash_pseudo_addr[3];


static ngx_int_t
ngx_http_upstream_init_ip_hash(ngx_conf_t *cf, ngx_http_upstream_srv_conf_t *us)
{
    /* 调用加权轮询策略的初始化函数 */
    if (ngx_http_upstream_init_round_robin(cf, us) != NGX_OK) {
        return NGX_ERROR;
    }

    /* 由于 ngx_http_upstream_init_round_robin 方法的选择后端服务器处理客户请求的初始化函数
     * 为 us->peer.init = ngx_http_upstream_init_round_robin_peer;
     */
    /* 重新设置 ngx_http_upstream_peer_t 结构体中 init 的回调方法为  ngx_http_upstream_init_ip_hash_peer */

    us->peer.init = ngx_http_upstream_init_ip_hash_peer;

    return NGX_OK;
}


static ngx_int_t
ngx_http_upstream_init_ip_hash_peer(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us)
{
    struct sockaddr_in                     *sin;
#if (NGX_HAVE_INET6)
    struct sockaddr_in6                    *sin6;
#endif
    ngx_http_upstream_ip_hash_peer_data_t  *iphp;

    /* 分配 ngx_http_upstream_ip_hash_peer_data_t 结构体内存空间 */
    iphp = ngx_palloc(r->pool, sizeof(ngx_http_upstream_ip_hash_peer_data_t));
    if (iphp == NULL) {
        return NGX_ERROR;
    }

    r->upstream->peer.data = &iphp->rrp;

    /* 调用加权轮询策略的初始化函数 ngx_http_upstream_init_round_robin_peer */
    if (ngx_http_upstream_init_round_robin_peer(r, us) != NGX_OK) {
        return NGX_ERROR;
    }

    /* 设置 IP hash 的决策函数  */
    r->upstream->peer.get = ngx_http_upstream_get_ip_hash_peer;

    switch (r->connection->sockaddr->sa_family) {

    /* 保存客户端 IP 地址 */

    /* IPv4 地址 */
    case AF_INET:
        sin = (struct sockaddr_in *) r->connection->sockaddr;
        iphp->addr = (u_char *) &sin->sin_addr.s_addr;
        iphp->addrlen = 3;
        break;

    /* IPv6 地址 */
#if (NGX_HAVE_INET6)
    case AF_INET6:
        sin6 = (struct sockaddr_in6 *) r->connection->sockaddr;
        iphp->addr = (u_char *) &sin6->sin6_addr.s6_addr;
        iphp->addrlen = 16;
        break;
#endif

    /* 非法地址 */
    default:
        iphp->addr = ngx_http_upstream_ip_hash_pseudo_addr;
        iphp->addrlen = 3;
    }

    /* 初始化 ngx_http_upstream_ip_hash_peer_data_t结构体成员 */
    iphp->hash = 89;
    iphp->tries = 0;
    /* 这个是设置为加权轮询策略的决策函数 */
    iphp->get_rr_peer = ngx_http_upstream_get_round_robin_peer;

    return NGX_OK;
}


/* 选择后端服务器处理请求 */
static ngx_int_t
ngx_http_upstream_get_ip_hash_peer(ngx_peer_connection_t *pc, void *data)
{
    ngx_http_upstream_ip_hash_peer_data_t  *iphp = data;

    time_t                        now;
    ngx_int_t                     w;
    uintptr_t                     m;
    ngx_uint_t                    i, n, p, hash;
    ngx_http_upstream_rr_peer_t  *peer;

    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, pc->log, 0,
                   "get ip hash peer, try: %ui", pc->tries);

    /* TODO: cached */

    /* 若重试连接的次数 tries 大于 20，或 只有一台后端服务器，则直接调用加权轮询策略选择当前后端服务器处理请求 */
    if (iphp->tries > 20 || iphp->rrp.peers->single) {
        return iphp->get_rr_peer(pc, &iphp->rrp);
    }

    now = ngx_time();

    pc->cached = 0;
    pc->connection = NULL;

    hash = iphp->hash;

    for ( ;; ) {

        /* 计算 IP 地址的 hash 值 */
        for (i = 0; i < (ngx_uint_t) iphp->addrlen; i++) {
            hash = (hash * 113 + iphp->addr[i]) % 6271;/* hash 函数 */
        }

        /* 以下是根据 hash 值选择合适的后端服务器来处理请求 */

        /* 若 ngx_http_upstream_rr_peers_t 结构体中 weighted 标志位为 1，
         * 表示所有后端服务器的总权重 与 后端服务器的数量 相等，
         * 则被选中的后端服务器在后端服务器列表中的位置为 hash 值与后端服务器数量的余数 p；
         */
        if (!iphp->rrp.peers->weighted) {
            p = hash % iphp->rrp.peers->number;

        } else {
            /* 若 ngx_http_upstream_rr_peers_t 结构体中 weighted 标志位为 0，
             * 首先计算 hash 值与后端服务器总权重的余数 w；
             * 将 w 值减去后端服务器的权重，直到有一个后端服务器使 w 值小于 0，
             * 则选中该后端服务器来处理请求，并记录在后端服务器列表中的位置 p；
             */
            w = hash % iphp->rrp.peers->total_weight;

            for (i = 0; i < iphp->rrp.peers->number; i++) {
                w -= iphp->rrp.peers->peer[i].weight;
                if (w < 0) {
                    break;
                }
            }

            p = i;
        }

        /* 计算被选中后端服务器在位图中的位置 n */
        n = p / (8 * sizeof(uintptr_t));
        m = (uintptr_t) 1 << p % (8 * sizeof(uintptr_t));

        /* 若当前被选中的后端服务器已经在位图记录过，则跳至 goto next 执行 */
        if (iphp->rrp.tried[n] & m) {
            goto next;
        }

        ngx_log_debug2(NGX_LOG_DEBUG_HTTP, pc->log, 0,
                       "get ip hash peer, hash: %ui %04XA", p, m);

        /* 获取当前被选中的后端服务器 */
        peer = &iphp->rrp.peers->peer[p];

        /* ngx_lock_mutex(iphp->rrp.peers->mutex); */

        /* 检查当前被选中后端服务器的 down 标志位，若该标志位为1，则跳至 goto next_try 执行 */
        if (peer->down) {
            goto next_try;
        }

        /* 若 down 标志位为 0，接着检查当前被选中后端服务器失败连接次数是否到达 max_fails，
         * 若已经达到 max_fails 次，并且睡眠时间还没到 fail_timeout，则跳至 goto next_try 执行；
         */
        if (peer->max_fails
            && peer->fails >= peer->max_fails
            && now - peer->checked <= peer->fail_timeout)
        {
            goto next_try;
        }

        /* 若不满足以上条件，则表示选择后方服务器成功 */
        break;

    next_try:

        /* 把当前后端服务器记录在位图中 */
        iphp->rrp.tried[n] |= m;

        /* ngx_unlock_mutex(iphp->rrp.peers->mutex); */

        /* 减少当前后端服务器重试连接的次数 */
        pc->tries--;

    next:

        /* tries 重试连接的次数加 1，并判断 tries 是否大于阈值 20，若大于，则采用加权轮询策略 */
        if (++iphp->tries >= 20) {
            return iphp->get_rr_peer(pc, &iphp->rrp);
        }
    }

    /* 到此已经成功选择了后端服务器来处理请求 */

    /* 记录当前后端服务器在后端服务器列表中的位置，该位置方便释放后端服务器调用 */
    iphp->rrp.current = p;

    /* 记录当前后端服务器的地址信息 */
    pc->sockaddr = peer->sockaddr;
    pc->socklen = peer->socklen;
    pc->name = &peer->name;

    if (now - peer->checked > peer->fail_timeout) {
        peer->checked = now;
    }

    /* ngx_unlock_mutex(iphp->rrp.peers->mutex); */

    /* 把当前后端服务器记录在位图相应的位置 */
    iphp->rrp.tried[n] |= m;
    /* 记录 hash 值 */
    iphp->hash = hash;

    return NGX_OK;
}


static char *
ngx_http_upstream_ip_hash(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_upstream_srv_conf_t  *uscf;

    uscf = ngx_http_conf_get_module_srv_conf(cf, ngx_http_upstream_module);

    if (uscf->peer.init_upstream) {
        ngx_conf_log_error(NGX_LOG_WARN, cf, 0,
                           "load balancing method redefined");
    }

    uscf->peer.init_upstream = ngx_http_upstream_init_ip_hash;

    uscf->flags = NGX_HTTP_UPSTREAM_CREATE
                  |NGX_HTTP_UPSTREAM_WEIGHT
                  |NGX_HTTP_UPSTREAM_MAX_FAILS
                  |NGX_HTTP_UPSTREAM_FAIL_TIMEOUT
                  |NGX_HTTP_UPSTREAM_DOWN;

    return NGX_CONF_OK;
}
