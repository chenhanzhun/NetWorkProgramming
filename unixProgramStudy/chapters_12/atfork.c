/* 线程和 fork */

/*
 * 函数功能：清理锁状态；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>
int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));
/*
 * 说明：
 * 该函数最多可以安装三个帮助清理锁的函数；
 * prepare fork处理程序由父进程在fork创建子进程前调用，这个fork处理程序的任务是获取父进程定义的所有锁；
 *
 * parent fork处理程序是在fork创建子进程以后，但在fork返回之前在父进程环境中调用的，这个fork处理程序的任务是对prepare fork处理程序
 * 获取的所有锁进行解锁；
 *
 * child fork处理程序在fork返回之前在子进程环境中调用，与parent fork处理程序一样，child fork处理程序必须释放prepare fork处理程序获得的所有锁；
 */
