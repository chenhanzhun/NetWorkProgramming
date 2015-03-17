/* 建立连接 */

/* 该函数的功能和connect相同，当connect连接出错时，这里支持重试连接 */

#ifndef CONNECTRE_H
#define CONNECTRE_H
#include "apue.h"
#include <sys/socket.h>

#define MAXSLEEP 128

int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t len)
{
    int nsec;
    /* Try to connect with exponential backoff */

    for(nsec = 1; nsec <= MAXSLEEP; nsec <<= 1)
    {
        if(connect(sockfd, addr, len) == 0)
            return 0;
        /* Delay before trying again */
        if(nsec <= MAXSLEEP/2)
            sleep(nsec);
    }
    return -1;
}
#endif
