#include "apue.h"

#define buf_size 4096

int main(void)
{
    int n;
    char buf[buf_size];

    while((n = read(STDIN_FILENO,buf,buf_size)) > 0)
        if(write(STDOUT_FILENO,buf,n) != n)
            err_sys("write error.\n");
    if(n < 0)
        err_sys("read error.\n");
    return 0;
}
