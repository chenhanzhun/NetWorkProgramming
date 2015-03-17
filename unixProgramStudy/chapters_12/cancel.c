/* 线程取消选项 */

/*
 * 函数功能：修改可取消状态属性；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_setcancelstate(int state, int *oldstate);
/*
 * 说明：
 * 该函数把可取消状态设置为state，把旧的可取消状态存放在oldstate所指的内存单元中；
 */

/*
 * 函数功能：添加线程的取消点；
 * 无返回值；
 * 函数原型：
 */
#include <pthread.h>
void pthread_testcancel(void);
/*
 * 说明：
 * 调用该函数时，若有某个取消请求处于未决状态，而且取消并没有置为无效，
 * 则线程就会被取消；但是若取消置为无效，则该函数调用没有任何效果；
 */

/*
 * 函数功能：修改取消类型；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_setcanceltype(int type, int *oldtype);
