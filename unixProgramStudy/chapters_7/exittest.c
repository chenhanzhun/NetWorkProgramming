#include <stdio.h>
#include <stdlib.h>
#include "apue.h"

static void my_exit1(void);
static void my_exit2(void);
int main(void)
{
    if (atexit(my_exit2) != 0)
        err_sys("can't register my_exit2.\n");
    if (atexit(my_exit1) != 0)
        err_sys("can't register my_exit1.\n");
    if (atexit(my_exit1) != 0)
        err_sys("can't register my_exit1.\n");
    printf("main is done.\n");
    return 0;
}

static void my_exit1(void)
{
    printf("first exit register.\n");
}
static void my_exit2(void)
{
    printf("second exit register.\n");
}
