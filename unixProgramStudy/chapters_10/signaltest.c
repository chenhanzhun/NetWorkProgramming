#include "apue.h"
#include <signal.h>

static void sig_sur(int signo);

int main(void)
{
    if(signal(SIGUSR1 , sig_sur) == SIG_ERR)
        err_sys("can't catch SIGUSR1");
    if(signal(SIGUSR2 , sig_sur) == SIG_ERR)
        err_sys("can't catch SIGUSR2");
    for(;;)
    {
        pause();
        printf("pause.\n");
    }

}

static void sig_sur(int signo)
{
    if(SIGUSR1 == signo)
        printf("recevied SIGUSR1.\n");
    else if(SIGUSR2 == signo)
        printf("recevied SIGUSR2.\n");
    else
        err_dump("recevied signal %d.\n",signo);
}
