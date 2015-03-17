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
    pid_t pid;
    int status;

    if((pid = fork()) < 0)
        err_sys("fork error");
    else if(0 == pid)   /* in child process */
        exit(7);
    if(wait(&status) != pid)    /* wait for child process */
        err_sys("wait error");
    pr_exit(status);    /* print its status */

    if((pid = fork()) < 0)
        err_sys("fork error");
    else if(0 == pid)   /* in child process */
        abort();        /* generated SIGABRT */
    if(wait(&status) != pid)    /* wait for child process */
        err_sys("wait error");
    pr_exit(status);    /* print its status */

    if((pid = fork()) < 0)
        err_sys("fork error");
    else if(0 == pid)   /* in child process */
        status /= 0;        /* divide by 0 generated SIGFPE */
    if(wait(&status) != pid)    /* wait for child process */
        err_sys("wait error");
    pr_exit(status);    /* print its status */

    exit(0);
}
