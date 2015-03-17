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
    pid_t pid,fpid;
    int status;

    if((pid = fork()) < 0)
        err_sys("fork error");
    else if(0 == pid)   /* in child process */
    {
        printf("child process, sleep 5s.\n");
        sleep(5);
        printf("child process, normal exit.\n");
        exit(0);
    }
    else
    {
        fpid = wait(&status);/* wait for child process */
        if(fpid < 0)
            err_sys("wait error");
        printf("father process, child process ID: %d\n",fpid);
        pr_exit(status);    /* print its status */
    }
    exit(0);
}

