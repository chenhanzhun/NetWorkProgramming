/* system 函数的实现 */
#include "apue.h"
#include <sys/wait.h>

int system(const char *cmdstring)
{
    pid_t pid;
    int status;

    if(NULL == cmdstring)
        return -1;
    if((pid = fork()) < 0)
        status = -1;
    else if(0 == pid)
    {
        execl("/bin/sh", "sh", "-c", "cmdstring", (char *)0);
        _exit(127);
    }
    else
    {
        while(waitpid(pid,&status,0) < 0)
        {
            if(errno != EINTR)
            {
                status = -1;
                break;
            }
        }
    }
    return status;
}

/* system 函数 */
#include <stdlib.h>
int system(const char *cmdstring);
