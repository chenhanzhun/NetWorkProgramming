
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


#if (NGX_THREADS)
ngx_mutex_t  *ngx_event_timer_mutex;
#endif


ngx_thread_volatile ngx_rbtree_t  ngx_event_timer_rbtree;
static ngx_rbtree_node_t          ngx_event_timer_sentinel;

/*
 * the event timer rbtree may contain the duplicate keys, however,
 * it should not be a problem, because we use the rbtree to find
 * a minimum timer value only
 */

/* 定时器事件初始化 */
ngx_int_t
ngx_event_timer_init(ngx_log_t *log)
{
    /* 初始化红黑树 */
    ngx_rbtree_init(&ngx_event_timer_rbtree, &ngx_event_timer_sentinel,
                    ngx_rbtree_insert_timer_value);

    /* 下面是针对多线程环境 */
#if (NGX_THREADS)

    if (ngx_event_timer_mutex) {
        ngx_event_timer_mutex->log = log;
        return NGX_OK;
    }

    ngx_event_timer_mutex = ngx_mutex_init(log, 0);
    if (ngx_event_timer_mutex == NULL) {
        return NGX_ERROR;
    }

#endif

    return NGX_OK;
}


/* 找出定时器红黑树最左边的节点 */
ngx_msec_t
ngx_event_find_timer(void)
{
    ngx_msec_int_t      timer;
    ngx_rbtree_node_t  *node, *root, *sentinel;

    /* 若红黑树为空 */
    if (ngx_event_timer_rbtree.root == &ngx_event_timer_sentinel) {
        return NGX_TIMER_INFINITE;
    }

    ngx_mutex_lock(ngx_event_timer_mutex);

    root = ngx_event_timer_rbtree.root;
    sentinel = ngx_event_timer_rbtree.sentinel;

    /* 找出红黑树最小的节点，即最左边的节点 */
    node = ngx_rbtree_min(root, sentinel);

    ngx_mutex_unlock(ngx_event_timer_mutex);

    /* 计算最左节点键值与当前时间的差值timer，当timer大于0表示不超时，不大于0表示超时 */
    timer = (ngx_msec_int_t) (node->key - ngx_current_msec);

    /*
     * 若timer大于0，则事件不超时，返回该值；
     * 若timer不大于0，则事件超时，返回0，标志触发超时事件；
     */
    return (ngx_msec_t) (timer > 0 ? timer : 0);
}


/* 检查定时器中所有事件 */
void
ngx_event_expire_timers(void)
{
    ngx_event_t        *ev;
    ngx_rbtree_node_t  *node, *root, *sentinel;

    sentinel = ngx_event_timer_rbtree.sentinel;

    /* 循环检查 */
    for ( ;; ) {

        ngx_mutex_lock(ngx_event_timer_mutex);

        root = ngx_event_timer_rbtree.root;

        /* 若定时器红黑树为空，则直接返回，不做任何处理 */
        if (root == sentinel) {
            return;
        }

        /* 找出定时器红黑树最左边的节点，即最小的节点，同时也是最有可能超时的事件对象 */
        node = ngx_rbtree_min(root, sentinel);

        /* node->key <= ngx_current_time */

        /* 若检查到的当前事件已超时 */
        if ((ngx_msec_int_t) (node->key - ngx_current_msec) <= 0) {
            /* 获取超时的具体事件 */
            ev = (ngx_event_t *) ((char *) node - offsetof(ngx_event_t, timer));

            /* 下面是针对多线程 */
#if (NGX_THREADS)

            if (ngx_threaded && ngx_trylock(ev->lock) == 0) {

                /*
                 * We cannot change the timer of the event that is being
                 * handled by another thread.  And we cannot easy walk
                 * the rbtree to find next expired timer so we exit the loop.
                 * However, it should be a rare case when the event that is
                 * being handled has an expired timer.
                 */

                ngx_log_debug1(NGX_LOG_DEBUG_EVENT, ev->log, 0,
                               "event %p is busy in expire timers", ev);
                break;
            }
#endif

            ngx_log_debug2(NGX_LOG_DEBUG_EVENT, ev->log, 0,
                           "event timer del: %d: %M",
                           ngx_event_ident(ev->data), ev->timer.key);

            /* 将已超时事件对象从现有定时器红黑树中移除 */
            ngx_rbtree_delete(&ngx_event_timer_rbtree, &ev->timer);

            ngx_mutex_unlock(ngx_event_timer_mutex);

#if (NGX_DEBUG)
            ev->timer.left = NULL;
            ev->timer.right = NULL;
            ev->timer.parent = NULL;
#endif

            /* 设置事件的在定时器红黑树中的监控标志位 */
            ev->timer_set = 0;/* 0表示不受监控 */

            /* 多线程环境 */
#if (NGX_THREADS)
            if (ngx_threaded) {
                ev->posted_timedout = 1;

                ngx_post_event(ev, &ngx_posted_events);

                ngx_unlock(ev->lock);

                continue;
            }
#endif

            /* 设置事件的超时标志位 */
            ev->timedout = 1;/* 1表示已经超时 */

            /* 调用已超时事件的处理函数对该事件进行处理 */
            ev->handler(ev);

            continue;
        }

        break;
    }

    ngx_mutex_unlock(ngx_event_timer_mutex);
}
