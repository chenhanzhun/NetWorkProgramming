#include "apue.h"
#include <fcntl.h>

void set_fl(int fd, int flags);
void clr_fl(int fd, int flags);

char buf[500000];

int main(void)
{
    int ntowrite, nwrite;
    char *ptr;

    ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "read %d bytes.\n",ntowrite);

    set_fl(STDOUT_FILENO, O_NONBLOCK);

    ptr = buf;
    while(ntowrite > 0)
    {
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, ntowrite);
        fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

        if(nwrite > 0)
        {
            ptr += nwrite;
            ntowrite -= nwrite;
        }
    }
    clr_fl(STDOUT_FILENO, O_NONBLOCK);

    exit(0);
}


//设置文件描述符标志
void set_fl(int fd, int flags)
{
    int val;
    //获取文件描述符标志
    if((val = fcntl(fd, F_GETFL, 0)) < 0)
        err_sys("fcntl F_GETFL error");
    val |= flags;//添加描述符标志flags
    //设置文件描述符标志
    if(fcntl(fd, F_SETFL, val) < 0)
        err_sys("fcntl F_SETFL error");
}
//清除文件描述符标志
void clr_fl(int fd, int flags)
{
    int val;
    //获取文件描述符标志
    if((val = fcntl(fd, F_GETFL, 0)) < 0)
        err_sys("fcntl F_GETFL error");
    val &= ~flags;//清除描述符标志flags
    //设置文件描述符标志
    if(fcntl(fd, F_SETFL, val) < 0)
        err_sys("fcntl F_SETFL error");
}
