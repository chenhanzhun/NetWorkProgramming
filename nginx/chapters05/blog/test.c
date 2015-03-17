void
ngx_process_events_and_timers(ngx_cycle_t *cycle)
{
    ngx_uint_t flags;
    ngx_msec_t timer, delta;

    if (ngx_timer_resolution) {
            timer = NGX_TIMER_INFINITE;
            flags = 0;
    } else {
            timer = ngx_event_find_timer();
            flags = NGX_UPDATE_TIME;
    }


    /*
    * ngx_use_accept_mutex变量代表是否使用accept互斥体，
    * 默认是使用，accept_mutex off;指令关闭.
    * accept mutex的作用就是避免惊群，同时实现负载均衡。
    */
    if (ngx_use_accept_mutex) {

    /*
     * ngx_accept_disabled变量在ngx_event_accept函数中计算
     * 如果ngx_accept_disabled大于了0，就表示该进程接受的 连接过多，
     * 因此就放弃一次争抢accept mutex的机会，同时将 自己减1。
     * 然后，继续处理已有连接上的事件。Nginx就借用 此变量实现了进程关于连接的基本负载均衡
     */
            if (ngx_accept_disabled > 0) {
	        	    ngx_accept_disabled--;
	        } else {/* ngx_accept_disabled小于0，连接数没超载*/

        /*
         * 尝试锁accept mutex，只有成功获取锁的进程，才会将listen 套接字放入epoll中。
        * 因此，这就保证了只有一个进程拥有 监听套接口，
        * 故所有进程阻塞在epoll_wait时，不会出现惊群现象.
        */
                if (ngx_trylock_accept_mutex(cycle) == NGX_ERROR){
                    return;
                }
                if (ngx_accept_mutex_held) {
        /*
        * 获取锁的进程，将添加一个NGX_POST_EVENTS标志，
        * 此标志的作用是将所有产生的事件放入一个队列中，
        * 等释放锁后，再慢慢来处理事件。因为，处理事件可能 会很耗时，
        * 如果不先释放锁再处理的话，该进程就长 时间霸占了锁，
        * 导致其他进程无法获取锁，这样accept 的效率就低了。
        */
                        flags |= NGX_POST_EVENTS;
                } else {
     /*
     * 没有获得锁的进程，当然不需要NGX_POST_EVENTS标志了。
     * 但需要设置最长延迟多久，再次去争抢锁。
     */
        if (timer == NGX_TIMER_INFINITE || timer > ngx_accept_mutex_delay) {
            timer = ngx_accept_mutex_delay;
                }
            }
        }
    }
    delta = ngx_current_msec;
    /*
     * epoll开始wait事件了,
     * ngx_process_events的具体实现是对应到 epoll模块中的ngx_epoll_process_events函数。
     */
    (void) ngx_process_events(cycle, timer, flags);
    delta = ngx_current_msec - delta;/*统计本次wait事件的耗时*/
    /*
     * ngx_posted_accept_events是一个事件队列 暂存epoll从监听套接口wait到的accept事件.
     * 前文提到的NGX_POST_EVENTS标志被使用后，就会将 所有的accept事件暂存到这个队列。
     * 这里完成对队列中的accept事件的处理，实际就是调用 ngx_event_accept函数来获取一个新的连接，然后放入 epoll中
     */


    if (ngx_posted_accept_events) { 
            ngx_event_process_posted(cycle, &ngx_posted_accept_events); 
        } 
    /*所有accept事件处理完成，如果拥有锁的话，就赶紧释放了。 其他进程还等着抢了。 */ 
    if (ngx_accept_mutex_held) { 
            ngx_shmtx_unlock(&ngx_accept_mutex); 
        }
    /*
     * delta是上文对epoll wait事件的耗时统计，存在毫秒级的耗时 就对所有事件的timer进行检查，
     * 如果time out就从timer rbtree中， 删除到期的timer，同时调用相应事件的handler函数完成处理
     */
    if (delta) { 
        ngx_event_expire_timers();
        } 
    /*
     * 处理普通事件（连接上获得的读写事件）队列上的所有事件，
     * 因为每个事件都有自己的handler方法，该怎么处理事件就 依赖于事件的具体handler了
     */
    if (ngx_posted_events) { 
            if (ngx_threaded) { 
                    ngx_wakeup_worker_thread(cycle);
            } else { 
                    ngx_event_process_posted(cycle, &ngx_posted_events); 
                } 
        } 
    }
