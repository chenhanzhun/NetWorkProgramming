#include <stdio.h>
#include "apue.h"
#include <sys/resource.h>
static void pr_limits(char *,int);

int main(void)
{
        pr_limits("RLIMIT_CORE",RLIMIT_CORE);
        pr_limits("RLIMIT_CPU",RLIMIT_CPU);
        pr_limits("RLIMIT_AS",RLIMIT_AS);
        pr_limits("RLIMIT_DATA",RLIMIT_DATA);
        pr_limits("RLIMIT_FSIZE",RLIMIT_FSIZE);
        pr_limits("RLIMIT_MEMLOCK",RLIMIT_MEMLOCK);
        pr_limits("RLIMIT_NOFILE",RLIMIT_NOFILE);
        pr_limits("RLIMIT_LOCKS",RLIMIT_LOCKS);
        pr_limits("RLIMIT_RSS",RLIMIT_RSS);
        pr_limits("RLIMIT_NPROC",RLIMIT_NPROC);
        pr_limits("RLIMIT_STACK",RLIMIT_STACK);
        exit(0);
}
static void pr_limits(char* name, int resource){
        struct rlimit limit;
        if(getrlimit(resource, &limit) <0)
            err_sys("getrlimit error for %s",name);

        printf("%-14s ",name);
        if(limit.rlim_cur == RLIM_INFINITY)
            printf("(infinite)  ");
        else
            printf("%-14ld",limit.rlim_cur);
        if(limit.rlim_max == RLIM_INFINITY)
            printf("(infinite)  ");
        else
            printf("%-14ld",limit.rlim_max);
        printf("\n");
}
