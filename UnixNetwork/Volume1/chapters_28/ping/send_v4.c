#include	"ping.h"
/* 在IPv4域中发送数据包 */
extern uint16_t in_cksum(uint16_t *addr, int len);
void
send_v4(void)
{
	int			len;
	struct icmp	*icmp;


    /* 设置ICMP报头 */
	icmp = (struct icmp *) sendbuf;
	icmp->icmp_type = ICMP_ECHO;/* 回显请求 */
	icmp->icmp_code = 0;
	icmp->icmp_id = pid;
	icmp->icmp_seq = nsent++;
	memset(icmp->icmp_data, 0xa5, datalen);	/* fill with pattern */
	gettimeofday((struct timeval *) icmp->icmp_data, NULL);/* 记录发送时间 */

	len = 8 + datalen;		/* checksum ICMP header and data */
	icmp->icmp_cksum = 0;
    /* 检验和算法 */
	icmp->icmp_cksum = in_cksum((u_short *) icmp, len);

    /* 发送数据包 */
	if( len != sendto(sockfd, sendbuf, len, 0, pr->sasend, pr->salen))
    {
        perror("sendto error");
        exit(1);
    }
}
