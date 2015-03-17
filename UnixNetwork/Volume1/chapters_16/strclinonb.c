/* include nonb1 */
#include <sys/select.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 4096
inline int Max(int a, int b)
{
    return(a >= b?a:b);
}

extern void err_sys(const char *, ...);
extern void err_quit(const char *, ...);

static void set_fl(int fd, int flags);

void str_cli(FILE *fp, int sockfd)
{
	int			maxfdp1, stdineof;
	ssize_t		n, nwritten;
	fd_set		rset, wset;
	char		to[MAXLINE], fr[MAXLINE];
	char		*toiptr, *tooptr, *friptr, *froptr;

    /* 设置套接字描述符为非阻塞 */
    set_fl(sockfd, O_NONBLOCK);
    /* 设置标准输入为非阻塞 */
    set_fl(STDIN_FILENO, O_NONBLOCK);
    /* 设置标准输出为非阻塞 */
    set_fl(STDOUT_FILENO, O_NONBLOCK);

    /* 初始化两个缓冲区指针 */
	toiptr = tooptr = to;	/* initialize buffer pointers */
	friptr = froptr = fr;
	stdineof = 0;/* 标准输入键入EOF的标志 */

	maxfdp1 = Max(Max(STDIN_FILENO, STDOUT_FILENO), sockfd) + 1;
	for ( ; ; ) {
        /* 初始化，为调用select函数做准备 */
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		if (stdineof == 0 && toiptr < &to[MAXLINE])
			FD_SET(STDIN_FILENO, &rset);	/* read from stdin */
		if (friptr < &fr[MAXLINE])
			FD_SET(sockfd, &rset);			/* read from socket */
		if (tooptr != toiptr)
			FD_SET(sockfd, &wset);			/* data to write to socket */
		if (froptr != friptr)
			FD_SET(STDOUT_FILENO, &wset);	/* data to write to stdout */

		if(select(maxfdp1, &rset, &wset, NULL, NULL) < 0)
            err_sys("select error");
/* end nonb1 */
/* include nonb2 */
        /* 若标准输入在rset有效，则从标准输入读取数据到发送缓冲区 */
		if (FD_ISSET(STDIN_FILENO, &rset))
        {
			if ( (n = read(STDIN_FILENO, toiptr, &to[MAXLINE] - toiptr)) < 0)
            {
				if (errno != EWOULDBLOCK)
					err_sys("read error on stdin");

			}
            else if (n == 0)
            {
				stdineof = 1;			/* all done with stdin */
				if (tooptr == toiptr)
					if(shutdown(sockfd, SHUT_WR) < 0)/* send FIN */
                        err_sys("shutdown error");

			}
            else
            {
				toiptr += n;			/* # just read */
				FD_SET(sockfd, &wset);	/* try and write to socket below */
			}
		}

        /* 若套接字在rset有效，则从发送缓冲区读取数据到该套接字中 */
		if (FD_ISSET(sockfd, &rset))
        {
			if ( (n = read(sockfd, friptr, &fr[MAXLINE] - friptr)) < 0)
            {
				if (errno != EWOULDBLOCK)
					err_sys("read error on socket");

			}
            else if (n == 0)
            {
				if (stdineof)
					return;		/* normal termination */
				else
					err_quit("str_cli: server terminated prematurely");

			}
            else
            {
				friptr += n;		/* # just read */
				FD_SET(STDOUT_FILENO, &wset);	/* try and write below */
			}
		}
/* end nonb2 */
/* include nonb3 */
        /* 若标准输出在wset有效，则从接收缓冲区读取数据到标准输出 */
		if (FD_ISSET(STDOUT_FILENO, &wset) && ( (n = friptr - froptr) > 0))
        {
			if ( (nwritten = write(STDOUT_FILENO, froptr, n)) < 0)
            {
				if (errno != EWOULDBLOCK)
					err_sys("write error to stdout");

			}
            else
            {
				froptr += nwritten;		/* # just written */
				if (froptr == friptr)
					froptr = friptr = fr;	/* back to beginning of buffer */
			}
		}

        /* 若套接字在wset有效，则从套接字读取数据到接收缓冲区中 */
		if (FD_ISSET(sockfd, &wset) && ( (n = toiptr - tooptr) > 0))
        {
			if ( (nwritten = write(sockfd, tooptr, n)) < 0)
            {
				if (errno != EWOULDBLOCK)
					err_sys("write error to socket");

			}
            else
            {
				tooptr += nwritten;	/* # just written */
				if (tooptr == toiptr) {
					toiptr = tooptr = to;	/* back to beginning of buffer */
                    /* 若套接字接收来自标准输入的数据，则关闭套接字写端，相当于从标准输入键入EOF */
					if (stdineof)
						if(shutdown(sockfd, SHUT_WR) <0)	/* send FIN */
                            err_sys("Shutdown error");
				}
			}
		}
	}
}
/* end nonb3 */

static void set_fl(int fd, int flags)
{
    int val;
    /* 获取描述符状态标志 */
    if( (val = fcntl(fd, F_GETFL, 0)) < 0)
        err_sys("fcntl get error");
    /* 添加描述符状态标志flags*/
    val |= flags;

    /* 设置描述符状态标志 */
    if(fcntl(fd, F_SETFL, val) < 0)
        err_sys("fcntl set error");
}
