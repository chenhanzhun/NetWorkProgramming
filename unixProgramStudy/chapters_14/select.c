#include "apue.h"
#include <sys/select.h>
#include <sys/types.h>

int main(void)
{
    char rbuf[1024];
    fd_set rd_fds;
    int ret,len;
    struct timeval tv;

    for(; ;)
    {
        FD_ZERO(&rd_fds);
        FD_SET(STDIN_FILENO,&rd_fds);
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        ret = select(1,&rd_fds,NULL,NULL,&tv);
        if(ret < 0)
        {
            err_sys("select error");
            break;
        }
        else if(ret  == 0)
            printf("timeout,waiting next loop\n");
        else
        {
            printf("ret = %d\n",ret);
            if(FD_ISSET(STDIN_FILENO,&rd_fds))
            {
                len =read(STDIN_FILENO,rbuf,1023);
                rbuf[len] = '\0';
                printf("Read buf are: %s\n",rbuf);
            }
        }
    }
    exit(0);
}
