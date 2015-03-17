#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/select.h>


extern int my_listen(const char *, const char *, socklen_t *);
extern void err_quit(const char *, ...);
extern int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
int		listenfd, connfd;


int
main(int argc, char **argv)
{
	int		n;
	char	buff[100];
    /* 为select函数使用的变量 */
    int maxfdp1;
    int justreadoob = 0;/* 标志是否读过由异常事件通知的带外数据 */
    fd_set  rset, xset;

    /* 服务器套接字处于监听状态 */
	if (argc == 2)
		listenfd = my_listen(NULL, argv[1], NULL);
	else if (argc == 3)
		listenfd = my_listen(argv[1], argv[2], NULL);
	else
		err_quit("usage: tcprecv01 [ <host> ] <port#>");

    /* 接受来自客户端的连接请求 */
	connfd = Accept(listenfd, NULL, NULL);

    /* 初始化fd_set结构 */
    FD_ZERO(&rset);
    FD_ZERO(&xset);

	for ( ; ; ) {
        FD_SET(connfd, &rset);
        if(justreadoob == 0)
            FD_SET(connfd, &xset);
        maxfdp1 = connfd+1;
        select(maxfdp1, &rset, NULL, &xset, NULL);

        /* 若产生异常事件，则读取带外数据 */
        if(FD_ISSET(connfd, &xset))
        {
            n = recv(connfd, buff, sizeof(buff)-1, MSG_OOB);
            buff[n] = 0;
            printf("read %d OOB bytes: %s\n", n, buff);
            /* 防止多次读取带外数据 */
            justreadoob = 1;
            FD_CLR(connfd, &xset);
        }
        /* 从套接字读取普通数据 */
        if(FD_ISSET(connfd, &rset))
        {
            if ( (n = read(connfd, buff, sizeof(buff)-1)) == 0) {
                printf("received EOF\n");
                exit(0);
            }
            buff[n] = 0;	/* null terminate */
            printf("read %d bytes: %s\n", n, buff);
            justreadoob = 0;
        }
	}
}
