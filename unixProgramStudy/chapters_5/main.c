#include <stdio.h>
#include "apue.h"

int main(void)
{
    char buf[MAXLINE];
    while(fgets(buf,MAXLINE,stdin) != NULL)
            if(fputs(buf,stdout) == EOF)
            err_sys("out error.\n");
    if(ferror(stdin))
    err_sys("input error.\n");
    exit(0);

