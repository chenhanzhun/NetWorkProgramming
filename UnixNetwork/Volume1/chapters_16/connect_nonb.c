#include <sys/select.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern void err_quit(const char *, ...);
int
connect_nonb(int sockfd, const struct sockaddr *saptr, socklen_t salen, int nsec)
{
	int				flags, n, error;
	socklen_t		len;
	fd_set			rset, wset;
	struct timeval	tval;

    /* 将套接字设置为非阻塞状态 */
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	error = 0;
    /* 发起连接请求，若返回EINPROGRESS，表示连接建立已经启动但是尚未完成 */
	if ( (n = connect(sockfd, saptr, salen)) < 0)
		if (errno != EINPROGRESS)/* 若是其他错误，则表示连接建立失败，直接退出 */
			return(-1);

	/* Do whatever we want while the connect is taking place. */

    /* 连接建立成功 */
	if (n == 0)
		goto done;	/* connect completed immediately */

    /* 若返回EINPROGRESS错误，表示连接建立已经启动但是尚未完成；
     * 此时调用select函数检查连接状态；
     */
	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;
	tval.tv_sec = nsec;
	tval.tv_usec = 0;
    /* 若select返回0，表示连接请求超时，建立连接失败，关闭该链接，并设置errno错误类型 */
	if ( (n = select(sockfd+1, &rset, &wset, NULL,
					 nsec ? &tval : NULL)) == 0) {
		close(sockfd);		/* timeout */
		errno = ETIMEDOUT;
		return(-1);
	}
    /* 若select返回大于0，则此时套接字处于可读或可写状态；
     * 则此时调用getsockopt函数检查连接状态，判断是连接成功还是失败；
     */
	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
		len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			return(-1);			/* Solaris pending error */
	} else
		err_quit("select error: sockfd not set");

done:
	fcntl(sockfd, F_SETFL, flags);	/* restore file status flags */

	if (error) {
		close(sockfd);		/* just in case */
		errno = error;
		return(-1);
	}
	return(0);
}
