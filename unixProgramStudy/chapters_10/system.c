#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

int Msystem(const char*cmdstring)
{
    pid_t   pid;
    int     status;
    struct  sigaction   ignore,saveintr,savequit;
    sigset_t    chldmask,savemask;

    if(cmdstring == NULL)
        return 1;
    ignore.sa_handler = SIG_IGN;
    sigemptyset(&ignore.sa_mask);
    ignore.sa_flags = 0;
    if(sigaction(SIGINT,&ignore,&savequit)<0)
    {
        perror("sigaction() error");
        exit(-1);
    }
    if(sigaction(SIGQUIT,&ignore,&savequit) <0)
    {
        perror("sigaction() error");
        exit(-1);
    }
    sigemptyset(&chldmask);
    sigaddset(&chldmask,SIGCHLD);
    if(sigprocmask(SIG_BLOCK,&chldmask,&savemask) < 0)
    {
        perror("sigprocmask() error");
        exit(-1);
    }
    if((pid = fork()) == -1)
    {
        perror("fork() error");
        exit(-1);
    }
    else if(pid == 0)
    {
        sigaction(SIGINT,&saveintr,NULL);
        sigaction(SIGQUIT,&savequit,NULL);
        sigprocmask(SIG_SETMASK,&savemask,NULL);
        execl("/bin/sh","sh","-c",cmdstring,(char *)0);
        _exit(-127);
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
    if (sigaction(SIGINT,&saveintr,NULL)<0)
        return -1;
    if (sigaction(SIGQUIT,&saveintr,NULL)<0)
        return -1;
    if (sigprocmask(SIG_SETMASK,&savemask,NULL)<0)
        return -1;
    return (status);
}

int main()
{
    printf("Print date:\n");
    Msystem("date");
    printf("Print process:\n");
    Msystem("ps");
    exit(0);
}
