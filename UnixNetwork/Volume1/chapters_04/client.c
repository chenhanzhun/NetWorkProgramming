/* TCP 客户端程序 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>

#define SERV_PORT 9877

extern void err_sys(const char *, ...);
extern void str_cli(FILE*, int);
extern void err_quit(const char *, ...);

int main(int argc, char **argv)
{
//    int sockfd[5];
    int sockfd;
    int err;
    struct sockaddr_in servadrr;

    if(argc != 2)
        err_quit("usage: %s <IPaddress>", argv[0]);

    //for(i = 0; i< 5; i++)
    //{
    /* 初始化地址 */
    bzero(&servadrr, sizeof(servadrr));
    servadrr.sin_family = AF_INET;
    servadrr.sin_port = htons(SERV_PORT);
    /* 将文本字符串地址转换为网络字节序的二进制地址 */
    inet_pton(AF_INET, argv[1], &servadrr.sin_addr);

    /* 创建客户端套接字 */
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");

    /* 向服务器发出连接请求 */
    if( (err = connect(sockfd, (struct sockaddr *)&servadrr, sizeof(servadrr))) < 0)
        err_sys("connect error");
    //}
    /* 处理函数 */
    str_cli(stdin, sockfd);

    exit(0);
}
