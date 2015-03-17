#include <stdlib.h>
#include <stdio.h>
#include "apue.h"

int main(void)
{
    char *ptr;
    int temp;

    if((ptr = getenv("USER")) == NULL)
        err_sys("get environment error.\n");
    printf("getnev: USER = %s.\n",ptr);

    temp = putenv("linux=linux3.8");
    if(0 == temp)
        printf("After putenv: linux= %s.\n",getenv("linux"));

    temp = setenv("linux","hellow linux",0);
    if(0 == temp)
        printf("After setenv: rewrite= 0 and linux= %s.\n",getenv("linux"));
    temp = setenv("linux","goodbye linux",1);
    if(0 == temp)
        printf("After setenv: rewrite != 0 and linux= %s.\n",getenv("linux"));
    temp = unsetenv("linux");
    if(0 == temp)
        printf("After unsetenv: linux= %s.\n",getenv("linux"));
    exit(0);
}
