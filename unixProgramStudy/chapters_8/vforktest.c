#include "apue.h"
#include <unistd.h>

int main(void)
{
    pid_t pid;
    int count = 0;

    pid = vfork();//创建一个子进程；

    if(pid < 0)
    {
        err_sys("Created child process error.\n");
        exit(-1);
    }
    else if(0 == pid)//在子进程中，返回0
    {
        printf("I am back in the child process, my ID: %d and my parent ID: %d\n",getpid(),getppid());
        printf("the count is: %d\n",++count);
    }
    else //在父进程中，返回新建子进程的ID
    {
        printf("I am back in the parent process, my ID: %d and my parent ID: %d\n",pid,getpid());
        printf("the count is: %d\n",++count);
    }
    exit(1);
}
