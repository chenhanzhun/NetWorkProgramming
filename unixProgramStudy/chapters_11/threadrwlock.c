/* 读写锁 */
/*
 * 函数功能：初始化读写锁；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
/*
 * 通过调用pthread_rwlock_init进程初始化，attr为NULL表示读写锁为默认的属性。
 * 在释放读写锁占用的内存之前，需要调用pthread_rwlock_destroy做清理工作。
 */

/*
 * 函数功能：对读写锁进行加、解锁；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);//在读模式下对读写锁进行加锁；
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);//在写模式下对读写锁进行加锁；
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);//进行解锁；
