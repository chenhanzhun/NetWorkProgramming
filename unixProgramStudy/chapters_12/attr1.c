/* 同步属性 */

/* 互斥量属性 */
/*
 * 函数功能：初始化互斥量属性；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
/*
 * 说明：
 * pthread_mutexattr_init函数用默认的互斥量属性初始化pthread_mutexattr_t结构；
 * 两个属性是进程共享属性和类型属性；
 */

/*
 * 函数功能：获取或修改进程共享属性；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared);//获取互斥量的进程共享属性
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);//设置互斥量的进程共享属性
/*
 * 说明：
 * 进程共享互斥量属性设置为PTHREAD_PROCESS_PRIVATE时，允许pthread线程库提供更加有效的互斥量实现，这在多线程应用程序中是默认的；
 * 在多个进程共享多个互斥量的情况下，pthread线程库可以限制开销较大的互斥量实现；
 */

/*
 * 函数功能：获取或修改类型属性；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);//获取互斥量的类型属性
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);//修改互斥量的类型属性

/* 读写锁属性 */
/*
 * 函数功能：初始化读写锁属性；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
/*
 * 说明：
 * 读写锁的唯一属性就是进程共享，该属性与互斥量的进程共享相同；
 */

/*
 * 函数功能：获取或修改读写锁的进程共享属性；
 * 返回值：若成功则返回0，否则返货错误编码；
 * 函数原型：
 */
int pthread_rwlock_getpshared(const pthread_rwlockattr_t *attr, int *pshared);
int pthread_rwlock_setpshared(pthread_rwlockattr_t *attr, int pshared);

/* 条件变量属性 */
/*
 * 函数功能：初始化条件变量属性；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);
/*
 * 函数功能：获取或修改进程共享属性；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared);
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);
