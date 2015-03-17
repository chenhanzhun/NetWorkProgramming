#include "apue.h"
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
        pid_t pid, fpid;
        siginfo_t info;

        if((pid = fork()) < 0)
            err_sys("fork error.");
        else if(pid == 0)
        {
            sleep(5);
            _exit(0);
        }
        while((fpid = waitid(P_PID,pid,&info,WEXITED)) == 0)
        {
            printf("terminated process success exit,and process ID: %d.\n",pid);
            sleep(1);
        }
        exit(0);
}
