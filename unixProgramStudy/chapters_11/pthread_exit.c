/* 线程终止 */

/*
 * 函数功能：退出线程；
 * 无返回值；
 * 函数原型：
 */
#include <pthread.h>
void pthread_exit(void *rval_ptr);
/*
 * 说明：
 * rval_ptr是一个无类型指针，与传给启动例程的单个参数类似；
 * 进程中的其他线程可以通过调用pthread_join函数访问到这个指针；
 */

/*
 * 函数功能：获取其他已终止的线程的退出码；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
int pthread_join(pthread_t thread, void **rval_ptr);
/*
 * 说明：
 * 调用pthread_join的线程将一直阻塞，直到thread指定的线程调用pthread_exit、从启动例程返回或被取消；
 * 如果线程只是从启动例程返回，rval_ptr将包含返回码；
 * 如果线程是被取消，由rval_ptr指向的内存单元设置为PTHREAD_CANCELED；
 */

/*
 * 函数功能：请求取消同一进程的其他线程；
 * 返回值：若成功则返回0，否则返回错误；
 * 函数原型：
 */
int pthread_cancel(pthread_t tid);
/*
 * 说明：
 * 在默认情况下，该函数会使得由tid标识的线程的行为表现为如同调用了参数为PTHREAD_CANCELED的pthread_exit函数，
 * 但是线程可以选择忽略取消方式或控制取消方式；
 * 注意：pthread_cancel并不等待线程终止，它仅仅提出请求；
 */

/*
 * 函数功能：线程清理处理程序；
 * 无返回值；
 * 函数原型：
 */
void pthread_cleanup_push(void(*rtn)(void *), void *arg);
void pthread_cleanup_pop(int execute);
/*
 * 说明：
 * 当线程执行以下动作时调用清理函数，调用参数为arg：
 * （1）调用pthread_exit函数时；
 * （2）响应取消请求时；
 * （3）用非零execute参数调用pthread_cleanup_pop时；
 * 如果参数execute设置为0，清理程序函数不被调用；
 */

/*
 * 函数功能：使线程处于分离状态；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
int pthread_detach(pthread_t tid);
