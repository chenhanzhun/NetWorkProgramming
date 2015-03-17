/* TCP 服务器程序 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h> /* 套接字操作函数头文件 */
#include <netinet/in.h> /* 套接字地址结构头文件 */
#include <unistd.h>
#include <sys/wait.h>

#define SERV_PORT   9877    /* 通用端口号 */
#define QLEN    1024        /* 套接字最大队列数 */

extern int initserver(int, struct sockaddr*, socklen_t, int);
extern void err_sys(const char *, ...);
extern void str_echo(int);
extern pid_t Fork();
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
void sig_chld(int signo);

int main(int argc, char *argv[])
{
    int listenfd,connectfd;
    pid_t pid;
    socklen_t clilen;
    struct sockaddr_in cliaddr,servaddr;

    /* 初始化服务器地址信息：通信域（IPv4）、端口号、IP地址 */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   /* 服务器IP地址采用通配符，即任何地址都匹配 */

    listenfd = initserver(SOCK_STREAM, (struct sockaddr *)&servaddr, sizeof(servaddr), QLEN);
    if(listenfd < 0)
            err_sys("initserver error");
    signal(SIGCHLD, sig_chld);
    for( ; ; )
    {
        clilen = sizeof(cliaddr);
        connectfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

        if( (pid = Fork()) == 0)    /* 子进程 */
        {
            close(listenfd);    /* 关闭监听套接字 */
            str_echo(connectfd);    /* 处理客户端请求 */
            exit(0);
        }
        close(connectfd);   /* 父进程关闭已连接套接字 */
    }
}

void sig_chld(int signo)
{
    int stat;
    pid_t pid;
    while( (pid = waitpid(-1, &stat, WNOHANG)) > 0);
    return;
}
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int		n;

again:
	if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef	EPROTO
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
