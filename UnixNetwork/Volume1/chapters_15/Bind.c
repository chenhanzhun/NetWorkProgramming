/* 创建一个Unix域套接字，并bind一个路径名 */
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>

extern void err_sys(const char *, ...);
extern void err_quit(const char *, ...);
int main(int argc, char **argv)
{
    int sockfd, size;
    socklen_t len;
    struct sockaddr_un addr1, addr2;

    if(argc != 2)
        err_quit("usage: %s <pathname>", argv[0]);

    bzero(&addr1, sizeof(addr1));
    addr1.sun_family = AF_UNIX;

    strncpy(addr1.sun_path, argv[1], sizeof(addr1.sun_path)-1);

    /* 创建一个Unix域套接字 */
    if( (sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");

    /* 若路径名在文件系统已存在，则bind会出错；所以先调用unlink删除要绑定的路径名，防止bind出错 */
    unlink(argv[1]);

    /* 将路径名bind绑定到该套接字上 */
    size = offsetof(struct sockaddr_un, sun_path) + strlen(addr1.sun_path);
    if(bind(sockfd, (struct sockaddr *)&addr1, size) < 0)
        err_sys("bind error");

    /* 显示已绑定的路径名 */
    len = sizeof(addr2);
    getsockname(sockfd, (struct sockaddr *)&addr2, &len);
    printf("bound name = %s, returned len = %d\n", addr2.sun_path, len);

    exit(0);
}
