#include "apue.h"
#include <sys/types.h>

int main(void)
{
    pid_t pid;
    if((pid = fork()) < 0)
        err_sys("fork error");
    else if(0 == pid)
    {
        setsid();
        printf("child process pid is %d\n",getpid());
        printf("group id is %d\n",getpgid(0));
        printf("My father id is %d\n",getppid());
        printf("My session id is %d\n",getsid(0));
    }
    else
    {
        printf("parent process pid is %d\n",getpid());
        printf("group id is %d\n",getpgid(0));
        printf("My father id is %d\n",getppid());
        printf("My session id is %d\n",getsid(0));
    }
}
