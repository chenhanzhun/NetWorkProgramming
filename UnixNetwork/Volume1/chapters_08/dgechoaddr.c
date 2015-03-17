#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include "un.h"
#include <net/if.h>
#include <arpa/inet.h>

/* 限定数据报的大小为 20 字节 */
#undef	MAXLINE
#define	MAXLINE	20		/* to see datagram truncation */
extern ssize_t recvfrom_flag(int , void *, size_t, int *, struct sockaddr*, socklen_t *, struct unp_in_pktinfo *);
extern char * Sock_ntop(const struct sockaddr *, socklen_t );
void
dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
{
	int						flags;
	const int				on = 1;
	socklen_t				len;
	ssize_t					n;
	char					mesg[MAXLINE], str[INET6_ADDRSTRLEN],
							ifname[IFNAMSIZ];
	struct in_addr			in_zero;
	struct unp_in_pktinfo	pktinfo;
/* 若支持IP_RECVDSATDDR和IP_RECVIF套接字选项，则设置它们 */
#ifdef	IP_RECVDSTADDR
	if (setsockopt(sockfd, IPPROTO_IP, IP_RECVDSTADDR, &on, sizeof(on)) < 0)
		err_ret("setsockopt of IP_RECVDSTADDR");
#endif
#ifdef	IP_RECVIF
	if (setsockopt(sockfd, IPPROTO_IP, IP_RECVIF, &on, sizeof(on)) < 0)
		err_ret("setsockopt of IP_RECVIF");
#endif
	bzero(&in_zero, sizeof(struct in_addr));	/* all 0 IPv4 address */

	for ( ; ; ) {
		len = clilen;
		flags = 0;
        /* 读取来自套接字的数据报 */
		n = recvfrom_flag(sockfd, mesg, MAXLINE, &flags,
						   pcliaddr, &len, &pktinfo);
        /* 把所读取的字节数显示，最大字节数不能超过20字节，若超过，则发生截断情况；
         * 调用sock_ntop把源IP地址和端口号转换为表达格式并输出 */
		printf("%d-byte datagram from %s", n, Sock_ntop(pcliaddr, len));
        /* 若返回的IP地址不为0，则调用inet_ntop转化目的IP地址格式并输出 */
		if (memcmp(&pktinfo.ipi_addr, &in_zero, sizeof(in_zero)) != 0)
			printf(", to %s", inet_ntop(AF_INET, &pktinfo.ipi_addr,
										str, sizeof(str)));
        printf(", index = %d", pktinfo.ipi_ifindex);
        /* 若返回的接口索引不为0，则调用if_indextoname获取接口名字并显示 */
		if (pktinfo.ipi_ifindex > 0)
			printf(", recv i/f = %s",
				   if_indextoname(pktinfo.ipi_ifindex, ifname));
        printf(", flags = %d",flags);
        /* 以下是测试4个标志 */
#ifdef	MSG_TRUNC
		if (flags & MSG_TRUNC)	printf(" (datagram truncated)");
#endif
#ifdef	MSG_CTRUNC
		if (flags & MSG_CTRUNC)	printf(" (control info truncated)");
#endif
#ifdef	MSG_BCAST
		if (flags & MSG_BCAST)	printf(" (broadcast)");
#endif
#ifdef	MSG_MCAST
		if (flags & MSG_MCAST)	printf(" (multicast)");
#endif
		printf("\n");
        /* 回射文本字符串给客户端 */
		sendto(sockfd, mesg, n, 0, pcliaddr, len);
	}
}
