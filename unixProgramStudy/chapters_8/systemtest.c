#include <sys/wait.h>
#include "apue.h"

void pr_exit(int status)
{
    if(WIFEXITED(status))
        printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
    else if(WIFSIGNALED(status))
        printf("abnormal termination, signal number = %d%s\n", WTERMSIG(status),
#ifdef WCOREDUMP
                WCOREDUMP(status) ? "(core file generated)" : " ");
#else
    " ");
#endif

    else if(WIFSTOPPED(status))
        printf("child stoped, signal number = %d\n", WSTOPSIG(status));
}

int main(void)
{
    int status;

    if((status = system("date")) < 0)
        err_sys("system error");
    pr_exit(status);

    if((status = system("nosuchcommand")) < 0)
        err_sys("system error");
    pr_exit(status);

    if((status = system("stat;who; exit 44")) < 0)
        err_sys("system error");
    pr_exit(status);

    exit(0);
}
