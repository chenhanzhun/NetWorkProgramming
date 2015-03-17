/* 信号集 */

#include <signal.h>

int sigemptyset(sigset_t *set);//初始化由set所指向的信号集，清空信号集；
int sigfillset(sigset_t *set);//初始化由set所指向的信号集，使其包括所有信号；
int sigaddset(sigset_t *set, int signo);//把指定的信号signo添加到由set所指的信号集中；
int sigdelset(sigset_t *set, int signo);//把指定的信号signo从由set所指定的信号集中删除；
//前面四个函数返回值：若成功则返回0，若出错则返回-1；

int sigismember(const sigset_t *set, int signo);//判断指定的信号signo是否在由set所指的信号集中；
//返回值：若为真则返回1，若为假则返回0，若出错则返回-1；
/*
 * 说明：
 * 所有应用程序使用信号集之前，要对该信号集调用sigemptyset或sigfillset一次；
 */


/* sigprocmask 函数 */
/*
 * 函数功能：检查或更改信号屏蔽字，也可同时执行这两个操作；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <signal.h>
int sigprocmask(int how, const sigset_t *set, sigset_t *oset);
/*
 * 说明：
 * 若oset是非空指针，那么进程的当前信号屏蔽字通过oset返回；
 * 若set是非空指针，则参数how指示如何修改当前信号屏蔽字；
 * 若set是空指针，则不改变该进程的信号屏蔽字，how的值就没有意义；
 * 参数how可选以下值：
 * （1）SIG_BLOCK   该进程新的信号屏蔽字是其当前信号屏蔽字和set指向信号集的并集。set包含了我们希望阻塞的附加信号；
 * （2）SIG_UNBLOCK 该进程新的信号屏蔽字是其当前信号屏蔽字和set指向信号集补集的交集；set包含我们希望解除阻塞的附加信号；
 * （3）SIG_SETMASK 该进程新的信号屏蔽字将被set指向的信号集的值所代替；
 *  SIG_BLOCK是"或"操作，而SIG_SETMASK则是赋值操作；
 *  注意：SIGKILL 和 SIGSTOP 信号是不能阻塞的；
 */

/* sigpending函数 */
/*
 * 函数功能：返回信号集；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <signal.h>
int sigpending(sigset_t *set);


/* sigaction函数 */
/*
 * 函数功能： 检查或修改与指定信号相关联的处理动作；此函数取代之前的signal函数；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <signal.h>
int sigaction(int signo, const struct sigaction *act, struct sigaction *oact);
/*
 * 说明：
 * signo是要检查或修改其具体动作的信号编号；
 * 若act非空，则要修改其动作；
 * 若oact指针非空，则系统经由oact指针返回该信号上一个动作；
 *
 * struct sigaction 结构如下：
 */
struct sigaction
{
    void (* sa_handler)(int);   /* addr of signal handler, or SIG_IGN, or SIG_DFL */
    sigset_t sa_mask;           /* additional signals to block */
    int sa_flags;               /* signal options */

    /* alternate handler */
    void (*sa_sigaction)(int, siginfo_t *, void *);
};
/*
 * 说明：
 * 当更改动作时，若sa_handler 字段包含一个信号捕捉函数的地址，则sa_mask字段说明了一个信号集，在调用该信号捕捉函数之前，
 * 这一信号集要加到进程的信号屏蔽字中。仅当从信号捕获函数返回时再将进程的信号屏蔽字复位为原先值；
 * 其中sa_flags标志如下：
 * （1）SA_INTERRUPT    由此信号中断的系统调用不会自动重启；
 * （2）SA_NOCLDSTOP    若signo是SIGCHLD，当子进程停止时，不产生此信号，当子进程终止时，仍然产生此信号；若已设置此标志，则当停止的进程继续运行时，不发送SIGCHLD信号；
 * （3）SA_NOCLDWAIT    若signo是SIGCHLD，当子进程停止时，不创建僵死进程；若调用进程后面调用wait，则调用进程阻塞，直到其所有子进程都终止，此时返回-1，并将errno设置为ECHILD；
 * （4）SA_NOEFER       当捕捉到此信号时，在执行其信号捕捉函数时，系统不自动阻塞此信号；
 * （5）SA_NOSTACK      若用signaltstack(2)声明了一替换栈，则将此信号传递给替换栈上的进程；
 * （6）SA_RESETHAND    在此信号捕捉函数入口处，将此信号的处理方式复位为SIG_DEL，并清除SA_SIGINFO标志；
 * （7）SA_RESTART      由此信号中断的系统调用会自动重启动；
 * （8）SA_SIGINFO      此选项对信号处理程序提供附加信息：一个指向siginfo结构的指针以及一个指向进程上下文标识符的指针；
 */
