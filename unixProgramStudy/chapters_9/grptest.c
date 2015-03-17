#include "apue.h"

int main(void)
{
    pid_t pid,fpid;

    if ((pid=fork())<0)
        err_sys("fork error");
    else if (0 == pid)
    {
        printf("The child process PID is %d.\n",getpid());
        printf("The child Group ID is %d.\n",getpgrp());
        printf("The child Group ID is %d.\n",getpgid(0));
    }
    else
    {
        fpid = getpid();
        sleep(3);
        setpgid(pid,fpid);
        printf("The parent process PID is %d.\n",fpid);
        printf("The parent Group ID is %d.\n",getpgrp());
    }
    exit(0);
}
