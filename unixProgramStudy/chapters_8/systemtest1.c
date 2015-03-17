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

int main(int argc, char *argv[])
{
    int status;
    if(argc != 2)
    {
        printf("usage: a.out <cmdstring>\n");
        exit(1);
    }
    status = system(argv[1]);
    pr_exit(status);

    exit(0);
}
