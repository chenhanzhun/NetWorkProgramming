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
    int on = 1;
	char	buff[100];

    /* 服务器套接字处于监听状态 */
	if (argc == 2)
		listenfd = my_listen(NULL, argv[1], NULL);
	else if (argc == 3)
		listenfd = my_listen(argv[1], argv[2], NULL);
	else
		err_quit("usage: tcprecv01 [ <host> ] <port#>");

    /* 设置SO_OOBINLINE套接字选项，表示希望在线接收带外数据 */
    setsockopt(listenfd, SOL_SOCKET, SO_OOBINLINE, &on, sizeof(on));
    /* 接受来自客户端的连接请求 */
	connfd = Accept(listenfd, NULL, NULL);
    sleep(5);/* sleep以接收来自发送进程的所有数据 */


	for ( ; ; ) {

        /* 检测套接字接收缓冲区是否处于带外标记 */
        if(sockatmark(connfd))
            printf("at OOB mark\n");
        /* 读取数据，并显示这些数据 */
            if ( (n = read(connfd, buff, sizeof(buff)-1)) == 0) {
                printf("received EOF\n");
                exit(0);
            }
            buff[n] = 0;	/* null terminate */
            printf("read %d bytes: %s\n", n, buff);
	}
}
