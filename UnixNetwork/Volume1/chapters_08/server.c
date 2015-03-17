/* UDP 服务器 */
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERV_PORT 9877 /* 通用端口号 */

extern void err_sys(const char *, ...);
extern void dg_echo(int sockfd, struct sockaddr *addr, socklen_t addrlen);

int main(int argc, char **argv)
{
    int sockfd;
    int err;
    struct sockaddr_in servaddr, cliaddr;

    /* 初始化服务器地址信息 */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* 创建套接字，并将服务器地址绑定到该套接字上 */
    if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        err_sys("socket error");
    err =bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(err < 0)
        err_sys("bind error");
    /* 服务器处理函数：读取套接字文本行，并把它回射给客户端 */
    dg_echo(sockfd, (struct sockaddr*) &cliaddr, sizeof(cliaddr));

}
