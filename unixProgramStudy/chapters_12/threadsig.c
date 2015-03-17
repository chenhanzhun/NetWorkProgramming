/* 线程与信号 */

/*
 * 函数功能：设置线程的信号屏蔽字；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <signal.h>

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset);
/*
 * 说明：
 * 该函数的功能基本上与前面介绍的在进程中设置信号屏蔽字的函数sigprocmask相同；
 */

/*
 * 函数功能：等待一个或多个信号发生；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
int sigwait(const sigset_t *set, int *signop);
/*
 * 说明：
 * set参数指出线程等待的信号集，signop指向的整数将作为返回值，表明发送信号的数量；
 */

/*
 * 函数功能：给线程发送信号；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
int pthread_kill(pthread_t thread, int signo);
/*
 * 说明：
 * signo可以是0来检查线程是否存在，若信号的默认处理动作是终止整个进程，那么把信号传递给某个线程仍然会杀死整个进程；
 */
