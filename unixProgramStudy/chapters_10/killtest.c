#include "apue.h"
#include <signal.h>
#include <unistd.h>

static void sig_alarm(int signo)
{
    printf("Recevied alarm.\n");
}

static void sig_kill(int signo)
{
    printf("Recevied kill.\n");
}

int main(void)
{
    int nsecs = 5;
    if(signal(SIGALRM, sig_alarm) == SIG_ERR)
        return(nsecs);
    signal(SIGHUP,sig_kill);
    printf("kill()\n");
    kill(getpid(),SIGHUP);
    printf("alarm()\n");
    alarm(nsecs);

    printf("pause.\n");
    pause();
    printf("raise().\n");
    raise(SIGHUP);

    exit(0);
}
