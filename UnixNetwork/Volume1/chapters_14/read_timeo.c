#include <sys/select.h>
#include <stdlib.h>
extern void err_sys(const char *,...);

/* 在指定的时间内等待描述符变为可读 */
int readable_timeo(int fd, int sec)
{
	fd_set			rset;
	struct timeval	tv;

    /* 初始化fd_set 结构，并添加描述符 */
	FD_ZERO(&rset);
	FD_SET(fd, &rset);

    /* 设置超时的时间 */
	tv.tv_sec = sec;/* 秒数 */
	tv.tv_usec = 0;/* 微秒 */

    /* 调用select函数，使进程阻塞于select的超时等待描述符变为可读 */
	return(select(fd+1, &rset, NULL, NULL, &tv));
		/* 4> 0 if descriptor is readable */
}
/* end readable_timeo */

int
Read_timeo(int fd, int sec)
{
	int		n;

	if ( (n = readable_timeo(fd, sec)) < 0)
		err_sys("readable_timeo error");
	return(n);
}
