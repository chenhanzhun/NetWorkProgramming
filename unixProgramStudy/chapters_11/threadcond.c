/* 条件变量 */
/*
 * 函数功能：初始化条件变量；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);

/*
 * 函数功能：等待条件变量变为真；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timewait(pthread_cond_t *cond, pthread_mutex_t *mutex,
            const struct timespec *timeout);
/*
 * 说明：
 * 传递给pthread_cond_wait的互斥量对条件进行保护，调用者把锁住的互斥量传给函数；
 * 函数把调用线程放到等待条件的线程列表上，然后对互斥量解锁，这两操作是原子操作；
 * 这样就关闭了条件检查和线程进入休眠状态等待条件改变这两个操作之间的时间通道，这样就不会错过条件变化；
 * pthread_cond_wait返回时，互斥量再次被锁住；
 */

/*
 * 函数功能：唤醒等待条件的线程；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
int pthread_cond_signal(pthread_cond_t *cond);//唤醒等待该条件的某个进程；
int pthread_cond_broadcast(pthread_cond_t *cond);//唤醒等待该条件的所有进程；
