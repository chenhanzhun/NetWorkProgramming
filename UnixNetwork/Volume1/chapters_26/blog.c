/* 函数功能：创建新的线程；
 * 返回值：若成功则返回0，若出错则返回正的错误码；
 * 函数原型：
 */
#include <pthread.h>

int pthread_create(pthread_t *tid, const pthread_attr_t *attr,
                    void*(*func)(void*), void *arg);
/* 说明：
 * 当该函数成功返回时，由tid指向的内存单元被设置为新创建线程的线程ID；
 * attr参数用于设置线程的属性，若为NULL时，表示采用默认属性创建该线程；
 * 新创建的线程从func函数的地址开始运行，该函数只有一个参数arg；
 * 若需要向func函数传递多个参数，则必须把需要传递的参数包装成一个结构体，
 * 然后把该结构体的地址作为arg参数传入；
 */

/* 函数功能：等待一个线程终止；
 * 返回值：若成功则返回0，若错误则返回正的错误码；
 * 函数原型：
 */
#include <pthread.h>

int pthread_join(pthread_t tid, void **status);
/* 说明：
 * 一个进程中的所有线程可以调用该函数回收其他线程，即等待其他线程终止；
 * tid参数是目标线程的标识符，status参数是保存目标线程返回的退出码；
 * 该函数会一直阻塞，直到被回收的线程终止；
 */

/* 函数功能：获取自身的线程ID；
 * 返回值：返回调用线程的线程ID；
 * 函数原型：
 */
#include <pthread.h>
pthread_t pthread_self(void);


/* 函数功能：把指定的线程变为脱离状态；
 * 返回值：若成功则返回0，若出错则返回正的错误码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_detach(pthread_t tid);

/* 函数功能：终止一个线程；
 * 返回值：无；
 * 函数原型：
 */
#include <pthread.h>
void pthread_exit(void *status);

/* 说明：
 * 若本线程不处于脱离状态，则其线程ID和退出状态码将一直保留到调用进程内的某个其他线程对它调用pthread_join函数；
 * status是向 线程的回收者传递其退出信息，执行完之后该信息不会返回给调用者；
 */

/* 函数功能：请求取消同一进程的其他线程；
 * 返回值：若成功则返回0，出错则返回正的错误码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_cancel(pthread_t tid);
/* 说明：
 * tid参数是目标线程的标识符；
 * 虽然可以请求取消某个线程，但是该线程可以决定是否允许被取消或者如何取消，这分别由以下两个函数完成：
 */
#include <pthread.h>
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
/* 说明：
 * 这两个函数中的第一个参数是分别用于设置取消状态（即是否允许取消）和取消类型（即如何取消），第二个参数则是分别记录线程原来的取消状态和取消类型；
 * state有两个可选的取值：
 * 1、PTHREAD_CANCEL_ENABLE     允许线程被取消（默认情况）；
 * 2、PTHREAD_CANCEL_DISABLE    禁止线程被取消，这种情况，若一个线程收到取消请求，则它会将请求挂起，直到该线程允许被取消；
 *
 * type也有两个可选取值：
 * 1、PTHREAD_CANCEL_ASYNCHRONOUS   线程随时可以被取消；
 * 2、PTHREAD_CANCEL_DEFERRED       允许目标线程推迟执行；
 */


/**********************************************************************/
/* 线程属性 */

/* 结构体定义 */
#include <bits/pthreadtypes.h>
#define __SIZEOF_PTHREAD_ATTR_T 36

typedef union
{
    char __size[__SIZEOF_PTHREAD_ATTR_T];
    long int __align;
}pthread_attr_t;
/*
 * 函数功能：初始化属性结构；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
/* 初始化线程属性对象 */
int pthread_attr_init(pthread_attr_t *attr);
/* 若线程属性是动态分配内存的，则在释放内存之前，必须调用该函数销毁线程属性对象 */
int pthread_attr_destroy(pthread_attr_t *attr);



/*
 * 函数功能：获取或修改线程的栈属性；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_attr_getstack(const pthread_attr_t *attr, void ** stackaddr, size_t stacksize);//获取线程栈信息；
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);//修改线程栈信息；

int pthread_attr_getstackaddr(const pthread_attr_t *attr, void ** stackaddr);//获取线程栈地址信息；
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);//修改线程栈地址信息；

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);//获取栈大小的信息；
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);//设置栈大小;

int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);



/* 互斥量 */
/*
 * 函数功能：初始化互斥变量；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>

/* 初始化互斥锁 */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
/* 销毁互斥锁，释放系统资源 */
int pthread_mutex_destroy(pthread_mutex_t *mutex);
/*
 * 说明：
 * attr表示互斥锁的属性，若attr为NULL，表示初始化互斥量为默认属性；
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
 *
 * 若设置为PTHREAD_PROCESS_PRIVATE，则表示互斥锁只能被和锁初始化线程隶属于同一进程的线程共享；
 */

/*
 * 函数功能：获取或修改类型属性；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);//获取互斥量的类型属性
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);//修改互斥量的类型属性
/* 说明：
 * type取值如下：
 * 1、PTHREAD_MUTEX_NORMAL          普通锁（默认）
 * 2、PTHREAD_MUTEX_ERRORCHECK      检查锁
 * 3、PTHREAD_MUTEX_RECUSIVE        嵌套锁
 * 4、PTHREAD_MUTEX_DEFAULT         默认锁
 */




/* 条件变量 */
/*
 * 函数功能：初始化条件变量；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
/* 说明：
 * cond参数指向要操作的目标条件变量，attr参数指定条件变量的属性；
 */

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
int pthread_cond_signal(pthread_cond_t *cond);//唤醒等待该条件的某个线程，具体唤醒哪个线程取决于线程的优先级和调度策略；
int pthread_cond_broadcast(pthread_cond_t *cond);//唤醒等待该条件的所有线程；
