#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h> /* 套接字操作函数头文件 */
#include <netinet/in.h> /* 套接字地址结构头文件 */
#include <unistd.h>

extern void err_sys(const char *, ...);
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int     n;  
  
again:  
    if ( (n = accept(fd, sa, salenptr)) < 0) {  
#ifdef  EPROTO  
        if (errno == EPROTO || errno == ECONNABORTED)  
#else  
        if (errno == ECONNABORTED)  
#endif  
            goto again;  
        else  
            err_sys("accept error");  
    }  
    return(n);  
}  
