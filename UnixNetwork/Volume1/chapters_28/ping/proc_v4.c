#include	"ping.h"

extern char *Sock_ntop_host(const struct sockaddr *sa, socklen_t salen);
void
proc_v4(char *ptr, ssize_t len, struct msghdr *msg, struct timeval *tvrecv)
{
	int				hlen1, icmplen;
	double			rtt;
	struct ip		*ip;
	struct icmp		*icmp;
	struct timeval	*tvsend;

	ip = (struct ip *) ptr;		/* start of IP header */
    /* IP报文首部长度，即IP报文首部的长度标志乘以4 */
	hlen1 = ip->ip_hl << 2;		/* length of IP header */
	if (ip->ip_p != IPPROTO_ICMP)
		return;				/* not ICMP */

    /* 越过IP报头，指向ICMP报头 */
	icmp = (struct icmp *) (ptr + hlen1);	/* start of ICMP header */
    /* ICMP报头及ICMP数据报的总长度，若小于8，则不合理 */
	if ( (icmplen = len - hlen1) < 8)
		return;				/* malformed packet */

    /* 确保所有接收的数据报是ICMP回显应答 */
	if (icmp->icmp_type == ICMP_ECHOREPLY) {
		if (icmp->icmp_id != pid)
			return;			/* not a response to our ECHO_REQUEST */
		if (icmplen < 16)
			return;			/* not enough data to use */

		tvsend = (struct timeval *) icmp->icmp_data;
        /* 计算接收和发送的时间差 */
		tv_sub(tvrecv, tvsend);
        /* 以毫秒为单位计算rtt */
		rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0;

        /* 打印相关信息 */
		printf("%d bytes from %s: icmp_seq=%u  ttl=%d  rtt=%.3f ms\n",
				icmplen, Sock_ntop_host(pr->sarecv, pr->salen),
				icmp->icmp_seq, ip->ip_ttl, rtt);
        nrecv++;

	} else if (verbose) {
		printf("  %d bytes from %s: icmp_type = %d, icmp_code = %d\n",
				icmplen, Sock_ntop_host(pr->sarecv, pr->salen),
				icmp->icmp_type, icmp->icmp_code);
	}
}
