#include <time.h>
#include <stdio.h>
#include "apue.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utime.h>

int main(void)
{
    time_t T;
    char *ptr;
    struct tm *time1, *time2;
    T = time(NULL);
    printf("time_t format:%ld\n",T);
    time1 = localtime(&T);
    ptr = asctime(time1);
    printf("localtime format:%s\n",ptr);
    time2 = gmtime(&T);
    ptr = asctime(time2);
    printf("gmtime format:%s\n",ptr);
    ptr = ctime(&T);
    printf("ctime format:%s\n",ptr);
    T = mktime(time1);
    printf("mktime format:%ld\n",T);
    exit(0);
}
