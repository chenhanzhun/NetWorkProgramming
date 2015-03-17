#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd, n;
	char				recvline[MAXLINE + 1];
	struct sockaddr_in	servaddr;
	struct in_addr		**pptr = NULL;
	struct in_addr		*inetaddrp[2];
	struct in_addr		inetaddr;
	struct hostent		*hp;
	struct servent		*sp;

	if (argc != 3)
		err_quit("usage: %s <hostname> <service>", argv[0]);

    /* 将主机名作为gethostbyname的参数，并获取hostent结构信息 */
	if ( (hp = gethostbyname(argv[1])) == NULL) {
        /* 若gethostbyname获取失败，则使用inet_aton构造地址单元素列表 */
		if (inet_aton(argv[1], &inetaddr) == 0) {
			err_quit("hostname error for %s: %s", argv[1], hstrerror(h_errno));
		} else {
			inetaddrp[0] = &inetaddr;
			inetaddrp[1] = NULL;
			pptr = inetaddrp;
		}
	} else {/* 若gethostbyname成功，则pptr指向地址链表 */
		pptr = (struct in_addr **) hp->h_addr_list;
	}

    /* 将服务名作为getservbyname的参数，获取servent结构信息 */
	if ( (sp = getservbyname(argv[2], "tcp")) == NULL)
        /* 若失败则退出 */
		err_quit("getservbyname error for %s", argv[2]);

    /* 遍历地址结构列表中的每一个地址 */
	for ( ; *pptr != NULL; pptr++) {
        /* 创建基于TCP套接字 */
		if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
            err_sys("socket error");

        /* 初始化服务器地址信息 */
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = sp->s_port;/* 端口号由getservbyname获取 */
        /* 复制服务器地址信息，该地址信息由gethostbyname获取 */
		memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
        /* 以下对地址列表中的每个服务器地址尝试连接 */
		printf("trying %s\n",
			   Sock_ntop((SA *) &servaddr, sizeof(servaddr)));

		if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) == 0)
			break;		/* success */
        /* 若地址连接失败，则关闭与该地址相关的套接字 */
		err_ret("connect error");
		close(sockfd);
	}
    /* 检查是否所有服务器地址都连接失败 */
	if (*pptr == NULL)
		err_quit("unable to connect");

    /* 处理函数，读取服务器应答信息，并显示到标准输出 */
	while ( (n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}
	exit(0);
}
