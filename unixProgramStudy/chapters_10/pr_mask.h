#ifndef PR_MASK_H
#define PR_MASK_H
#include "apue.h"
#include <errno.h>

void pr_mask(const char *str)
{
    sigset_t sigset;
    int errno_save;

    errno_save = errno;
    if(sigprocmask(0, NULL, &sigset) < 0)
        err_sys("sigprocmask error");

    printf("%s", str);
    if(sigismember(&sigset, SIGINT))
        printf("SIGINT\t");
    if(sigismember(&sigset, SIGQUIT))
        printf("SIGQUIT\t");
    if(sigismember(&sigset, SIGUSR1))
        printf("SIGUSR1\t");
    if(sigismember(&sigset, SIGALRM))
        printf("SIGALRM\t");
    printf("\n");
}
#endif
