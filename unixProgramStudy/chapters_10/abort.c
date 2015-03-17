#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void Mabort(void)
{
    sigset_t mask;
    struct sigaction action;

    /*
     * caller can't ignore SIGABRT, if so reset to default.
     */
    sigaction(SIGABRT, NULL, &action);
    if(action.sa_handler == SIG_IGN)
    {
        action.sa_handler = SIG_DFL;
        sigaction(SIGABRT, &action, NULL);
    }

    if(action.sa_handler == SIG_DFL)
        fflush(NULL);
    /*
     * caller can't block SIGABRT; make sure it's unblock
     */
    sigfillset(&mask);
    sigdelset(&mask, SIGABRT);  /* mask has only SIGABRT turned off */
    sigprocmask(SIG_SETMASK, &mask, NULL);
    kill(getpid(), SIGABRT);    /* send the signal */

    fflush(NULL);   /* flush all open stdio stream */
    action.sa_handler = SIG_DFL;
    sigaction(SIGABRT, &action, NULL);  /* reset to default */
    sigprocmask(SIG_SETMASK, &mask, NULL);  /* just in case ... */
    kill(getpid(), SIGABRT);    /* and one more time */
    exit(1);    /* this should never be executed ... */
}

static void sig_abort(int signo)
{
    printf("Recevied abort ...\n");
}
int main(void)
{
    signal(SIGABRT, sig_abort);
    Mabort();
    pause();
    exit(0);
}
