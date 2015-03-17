/* 线程属性 */

/*
 * 线程的主要属性：
 * （1）detachstate     线程的分离状态属性；
 * （2）guardsize       线程栈末尾的警戒缓冲区大小（字节数）；
 * （3）stackaddr       线程栈的最低地址；
 * （4）stacksize       线程栈的大小（字节数）；
 */
/*
 * 函数功能：初始化属性结构；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_attr_init(pthread_attr_t *attr);//初始化；
int pthread_attr_destroy(pthread_attr_t *attr);//若是动态分配的内存空间，则在释放内存之前调用此函数；

/*
 * 函数功能：修改线程的分离状态属性；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);//获取当前线程的分离状态属性；
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);//设置当前线程的分离状态属性；
/*
 * 说明：
 * detachstate的值为以下两种：
 * （1）PTHREAD_CREATE_DETACHED 以分离状态启动线程；
 * （2）PTHREAD_CREATE_JOINABLE 正常启动线程；
 */

/*
 * 函数功能：获取或修改线程的栈属性；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_attr_getstack(const pthread_attr_t *attr, void ** stackaddr, size_t stacksize);//获取线程栈信息；
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);//修改线程栈信息；

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);//获取栈大小的信息；
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);//设置栈大小;

int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);

/*
 * 函数功能：获取线程并发度信息；
 * 函数原型：
 */
#include <pthread.h>
int pthread_getconcurrency(void);//获取当前并发度；
int pthread_setconcurrency(int level);//设置并发度
/*
 * 说明：
 * 设置并发度只是提示系统，系统并不一定采取；
 */
