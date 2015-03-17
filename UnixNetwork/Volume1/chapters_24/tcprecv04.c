#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
typedef void Sigfunc(int);

extern int my_listen(const char *, const char *, socklen_t *);
extern void err_quit(const char *, ...);
extern Sigfunc *MySignal(int signo, Sigfunc *func);
extern int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
int		listenfd, connfd;

void	sig_urg(int);

int
main(int argc, char **argv)
{

    int size;
    /* 服务器套接字处于监听状态 */
	if (argc == 2)
		listenfd = my_listen(NULL, argv[1], NULL);
	else if (argc == 3)
		listenfd = my_listen(argv[1], argv[2], NULL);
	else
		err_quit("usage: tcprecv01 [ <host> ] <port#>");

    /* 设置套接字的接收缓冲区大小为4096*/
    size = 4096;
    setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

    /* 接受来自客户端的连接请求 */
	connfd = Accept(listenfd, NULL, NULL);

    /* 捕获SIGURG信号，并对该信号进行处理 */
	MySignal(SIGURG, sig_urg);
    /* 设置已连接套接字的属主 */
	fcntl(connfd, F_SETOWN, getpid());

    for(; ; )
        pause();
}

void
sig_urg(int signo)
{
	int		n;
	char	buff[2048];

	printf("SIGURG = %d received\n", signo);
    /* 读入带外数据 */
	n = recv(connfd, buff, sizeof(buff)-1, MSG_OOB);
	buff[n] = 0;		/* null terminate */
	printf("read %d OOB byte: %s\n", n, buff);
}
