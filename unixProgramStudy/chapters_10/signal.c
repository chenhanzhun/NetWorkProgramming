/* signal 函数*/
/*
 * 函数功能：信号处理机制；
 * 返回值：若成功则返回信号以前的处理配置，若出错则返回SIG_ERR；
 * 函数原型：
 */
#include <signal.h>
void (*signal (int signo, void(*func)(int))) (int);
/*
 * 说明：
 * signo是信号名；
 * func是常量值SIG_IGN、SIG_DFL或当接到此信号后要调用的函数的地址，
 * 若指定常量SIG_IGN，则向内核表示忽略此信号(SIGKILL和SIGSTOP这两个信号不能忽略)；
 * 若指定常量SIG_DFL，则表示接到此信号后的动作是系统默认动作；
 * 当指定函数地址时，则在信号发生时，调用该函数；
 *
 * signal函数需要两个参数，返回一个函数指针，而该指针所指向的函数无返回值；
 * 也就是说signal函数的返回值是一个函数地址，该函数有一个参数；
 * 可以简化上面的函数形式表达：
 */
typedef void Sigfunc(int);
Sigfunc *signal(int, Sigfunc*);
