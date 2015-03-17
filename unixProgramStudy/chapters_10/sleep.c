#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

static void sig_alrm(int signo)
{
    printf("Recevied alarm ... \n");
}

unsigned int Msleep(unsigned int nsecs)
{
    struct sigaction    newact,oldact;
    sigset_t            newmask,oldmask,suspmask;
    unsigned int        unslept;

    newact.sa_handler = sig_alrm;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    sigaction(SIGALRM,&newact,&oldact);
    sigemptyset(&newmask);
    sigaddset(&newmask,SIGALRM);
    sigprocmask(SIG_BLOCK,&newmask,&oldmask);
    alarm(nsecs);
    suspmask = oldmask;
    sigdelset(&suspmask,SIGALRM);
    sigsuspend(&suspmask);
    unslept = alarm(0);
    sigprocmask(SIG_SETMASK,&oldmask,NULL);
    return unslept;
}

int main()
{
    int i;
    printf("Program starting.\n");
    printf("sleep 5 seconds.....\n");
    for(i=1;i<=5;++i)
    {
        printf("The %dth second.\n",i);
        Msleep(1);
    }
    printf("wake up.\n");
    exit(0);
}
