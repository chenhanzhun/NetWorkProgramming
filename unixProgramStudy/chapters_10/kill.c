/* 信号发送与捕获 */

/*
 * 函数功能：将信号发送给进程或进程组；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <signal.h>
int kill(pid_t pid, int signo);
/*说明:
 * signo 是信号类型；
 * pid有以下四种情况：
 * (1)、pid >0，将该信号signo发送给进程ID为pid的进程；
 * (2)、pid ==0，将该信号signo发送给与发送进程属于同一进程组的所有进程，
 *             而且发送进程具有向这些进程发信号的权限；
 * (3)、pid <0，将该信号signo发送给其进程组ID等于pid绝对值，而且发送进程具有
 *            其所发送信号的权限；
 * (4)、pid ==-1，将该信号signo发送给发送进程有权限向他们发送信号的系统上的所有进程；
 */
/*
 * 函数功能：向进程自身发送信号；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
int raise(int signo);
/*
 * raise(signo)等价于kill(getpid(),signo)；
 */

/*
 * 函数功能：设置进程的闹钟时间；
 * 返回值：0或以前设置的闹钟时间余留的秒数；
 * 函数原型：
 */
#include <unistd.h>
unsigned int alarm(unsigned int seconds);
/*
 * 说明：
 * 参数seconds是秒数，经过指定的秒数seconds后会产生信号SIGALRM；
 * 若进程之前登记过的闹钟尚未超过闹钟时间，而且本次seconds为0时，
 * 则取消之前的闹钟时钟，其余留值仍作为alarm函数的返回值；
 */

int pause(void);//使调用进程挂起直到捕获到一个信号；
/*
 * 只有执行了一个信号处理程序并从其返回时，pause才返回；
 * 在这种情况下，pause返回-1，并将errno设置为EINTR；
 */
