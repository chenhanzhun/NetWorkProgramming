/* 线程 */

/*
 * 函数功能：比较两个线程ID；
 * 返回值：若相等则返回非0，否则返回0；
 * 函数原型：
 */
#include <pthread.h>

int pthread_equal(pthread_t tid1, pthread_t tid2);

/*
 * 函数功能：获取自身的线程ID；
 * 返回值：调用线程的线程ID；
 * 函数原型：
 */
pthread_t pthread_self(void);

/*
 * 函数功能：创建线程；
 * 返回值：若成功则返回0，否则返回错误编号；
 * 函数原型：
 */
int pthread_create(pthread_t *tidp, const pthread_attr_t *attr,
                void *(*start_rtn)(void *), void *arg);
/*
 * 说明：
 * 当该函数成功返回时，由tidp指向的内存单元被设置为新创建线程的线程ID；
 * attr参数用于定制各种不同的线程属性，当为NULL表示创建默认属性的线程；
 * 新创建的线程从start_rtn函数的地址开始运行，该函数只有一个指针参数arg，
 * 如果需要向start_rtn函数传递的参数不止一个，则需要把参数放在一个结构中，
 * 然后把这个结构的地址作为arg参数传入；
 */
