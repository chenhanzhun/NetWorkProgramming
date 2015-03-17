/* 互斥量 */
/*
 * 函数功能：初始化互斥变量；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
/*
 * 说明：
 * 若attr为NULL，表示初始化互斥量为默认属性；
 */

/*
 * 函数功能：对互斥量进行加、解锁；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
int pthread_mutex_lock(pthread_mutex_t *mutex);//对互斥量进行加锁，线程被阻塞；
int pthread_mutex_trylock(pthread_mutex_t *mutex);//对互斥变量加锁，但线程不阻塞；
int pthread_mutex_unlock(pthread_mutex_t *mutex);//对互斥量进行解锁；
/* 说明：
 * 调用pthread_mutex_lock对互斥变量进行加锁，若互斥变量已经上锁，则调用线程会被阻塞直到互斥量解锁；
 * 调用pthread_mutex_unlock对互斥量进行解锁；
 * 调用pthread_mutex_trylock对互斥量进行加锁，不会出现阻塞，否则加锁失败，返回EBUSY。
 */
