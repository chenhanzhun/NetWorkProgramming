
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_EVENT_TIMER_H_INCLUDED_
#define _NGX_EVENT_TIMER_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


#define NGX_TIMER_INFINITE  (ngx_msec_t) -1

#define NGX_TIMER_LAZY_DELAY  300


ngx_int_t ngx_event_timer_init(ngx_log_t *log);
ngx_msec_t ngx_event_find_timer(void);
void ngx_event_expire_timers(void);


#if (NGX_THREADS)
extern ngx_mutex_t  *ngx_event_timer_mutex;
#endif


extern ngx_thread_volatile ngx_rbtree_t  ngx_event_timer_rbtree;


/* 从定时器中移除事件 */
static ngx_inline void
ngx_event_del_timer(ngx_event_t *ev)
{
    ngx_log_debug2(NGX_LOG_DEBUG_EVENT, ev->log, 0,
                   "event timer del: %d: %M",
                    ngx_event_ident(ev->data), ev->timer.key);

    ngx_mutex_lock(ngx_event_timer_mutex);

    /* 从红黑树中移除指定事件的节点对象 */
    ngx_rbtree_delete(&ngx_event_timer_rbtree, &ev->timer);

    ngx_mutex_unlock(ngx_event_timer_mutex);

#if (NGX_DEBUG)
    ev->timer.left = NULL;
    ev->timer.right = NULL;
    ev->timer.parent = NULL;
#endif

    /* 设置相应的标志位 */
    ev->timer_set = 0;
}

/* 将事件添加到定时器中 */
static ngx_inline void
ngx_event_add_timer(ngx_event_t *ev, ngx_msec_t timer)
{
    ngx_msec_t      key;
    ngx_msec_int_t  diff;

    /* 设置事件对象节点的键值 */
    key = ngx_current_msec + timer;

    /* 判断事件的相应标志位 */
    if (ev->timer_set) {

        /*
         * Use a previous timer value if difference between it and a new
         * value is less than NGX_TIMER_LAZY_DELAY milliseconds: this allows
         * to minimize the rbtree operations for fast connections.
         */

        diff = (ngx_msec_int_t) (key - ev->timer.key);

        if (ngx_abs(diff) < NGX_TIMER_LAZY_DELAY) {
            ngx_log_debug3(NGX_LOG_DEBUG_EVENT, ev->log, 0,
                           "event timer: %d, old: %M, new: %M",
                            ngx_event_ident(ev->data), ev->timer.key, key);
            return;
        }

        ngx_del_timer(ev);
    }

    ev->timer.key = key;

    ngx_log_debug3(NGX_LOG_DEBUG_EVENT, ev->log, 0,
                   "event timer add: %d: %M:%M",
                    ngx_event_ident(ev->data), timer, ev->timer.key);

    ngx_mutex_lock(ngx_event_timer_mutex);

    /* 将事件对象节点插入到红黑树中 */
    ngx_rbtree_insert(&ngx_event_timer_rbtree, &ev->timer);

    ngx_mutex_unlock(ngx_event_timer_mutex);

    /* 设置标志位 */
    ev->timer_set = 1;
}


#endif /* _NGX_EVENT_TIMER_H_INCLUDED_ */
