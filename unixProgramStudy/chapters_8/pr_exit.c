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
