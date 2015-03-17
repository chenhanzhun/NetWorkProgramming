#include "apue.h"
#include <sys/uio.h>
#include <stdlib.h>
int main(void)
{
    struct iovec iov[2];
    char *buf1 = (char *)malloc(5);
    char *buf2 = (char *)malloc(1024);
    memset(buf1, 0, 5);
    memset(buf2, 0, 1024);
    iov[0].iov_base = buf1;
    iov[1].iov_base = buf2;
    iov[0].iov_len = 5;
    iov[1].iov_len = 1024;

    ssize_t nread, nwrite;
    nread = readv(STDIN_FILENO, iov, 2);
    if(nread == -1)
        err_sys("readv error");
    else
    {
        printf("readv:\n");
        printf("buf1 is: %s\t length is: %d\n",buf1, strlen(buf1));
        printf("buf2 is: %s\t length is: %d\n",buf2, strlen(buf2));
    }
    printf("writev:\n");
    nwrite = writev(STDOUT_FILENO, iov, 2);
    if(nwrite == -1)
        err_sys("writev error");

    free(buf1);
    free(buf2);
    exit(0);

}
