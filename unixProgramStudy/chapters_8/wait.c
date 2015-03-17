/* 等待进程终止 */

/*
 * 函数功能：等待一个进程终止；
 * 返回值：若成功则返回进程ID、0；若出错则返回-1；
 * 函数原型：
 */
#include <sys/wait.h>

pid_t wait(int *statloc);

pid_t waitpid(pid_t pid, int *statloc, int option);

/*
 * 说明：
 * 参数statloc是指向存放终止进程的终止状态单元的指针；如果不在意这些信息，则可设为NULL；
 * pid和option参数是控制waitpid操作；
 */
/*
 * pid参数及作用如下
 * pid == -1;   等待任一子进程，此时waitpid和wait功能一样；
 * pid > 0;     等待其进程ID与pid相等的子进程；
 * pid == 0;    等待其组ID与调用进程组ID相等的任一子进程；
 * pid < -1;    等待其组ID与pid绝对值相等的任一子进程；
 */
/*
 * 参数option可以是0，也可以是以下参数按位"或"组成：
 * WCONTINUED   若实现支持作业控制，由pid指定的任一子进程在暂停后已经继续，但其状态尚未报告，则返回其状态；
 * WNOHANG      若由pid指定的子进程并不是立即可用的，则waitpid不阻塞，此时其返回值0；
 * WUNTRACED    若某实现支持作业控制，而由pid指定的任一子进程已处于暂停状态，并且其状态自暂停以来还未报告，则返回其状态；
 */
/* 检查 wait 和 waitpid 所返回的终止状态的宏.....
 * WIFEXITED(status)    若为正常终止子进程返回的状态，则为真；对于这种情况可执行WEXITSTATUS(status)，
 *                      取子进程传给exit,_exit或_Exit参数的低8位；
 *
 * WIFSIGNALED(status)  若为正常终止子进程返回的状态，则为真；对于这种情况可执行WTERMSIG(status)，
 *                      取使子进程终止的信号编号；另外，有些实现定义宏WCONREDUMP(status)，若已产生终止进程的core文件，则返回真；
 *
 * WIFSTOPPED(status)   若为当前暂停子进程的返回状态，则为真；对于这种情况可执行WSTOPSIG(status)，取使子进程暂停的信号编号；
 *
 * WIFCONTINUED(status) 若在作业控制暂停后已经继续的子进程返回的状态，则为真；
 *
 */

/*
 * waitid 函数
 * 函数功能：等待一个进程终止；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/wait.h>
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

/*
 * 参数说明：
 * idtype 是以下的常量：
 * P_PID    等待一个特定的进程：id包含要等待的子进程的进程ID；
 * P_PGID   等待一个特定进程组的任一子进程：id包含要等待的子进程的进程组ID；
 * P_ALL    等待任一子进程：忽略id；
 *
 * options 是以下参数按位"或"组成：
 * WCONTINUED   等待一个进程，暂停后已经继续，但其状态尚未报告；
 * WEXITED      等待已退出的进程；
 * WNOHANG      若无可用的子进程退出状态，立即返回而非阻塞；
 * WNOWAIT      不破坏子进程退出状态，该子进程退出状态可由后续的 wait、waitid 或 waitpid 调用取得；
 * WSTOPPED     等待一个进程，它已经暂停，但其状态尚未报告；
 *
 * infop 是指向 siginfo 结构的指针，该结构包含有关引起子进程状态改变的生成信号的详细信息；
 */

/*
 * wait3 和 wait4 函数
 * 函数功能：等待一个进程终止；
 * 返回值：若成功则返回进程ID，若出错则返回-1；
 * 函数原型：
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

pid_t wait3(int *statloc, int options, struct rusage *rusage);
pid_t wait4(pid_t pid, int *statloc, int options, struct rusage *rusage);
