#include	"unp.h"

int
main(int argc, char **argv)
{
	int				sockfd, n;
	char			recvline[MAXLINE + 1];
	socklen_t		len;
	struct sockaddr_storage	ss;

    struct addrinfo hints, *res, *ressave;
	if (argc != 3)
		err_quit("usage: %s <hostname/IPaddress> <service/port#>", argv[0]);

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if( (n= getaddrinfo(argv[1], argv[2], &hints, &res)) != 0)
        err_quit("error %s", gai_strerror(n));

    for(ressave = res; ressave != NULL; ressave = ressave->ai_next)
    {
       if( (sockfd = socket(ressave->ai_family, ressave->ai_socktype, ressave->ai_protocol) < 0))
           continue;
       if(connect(sockfd, ressave->ai_addr, ressave->ai_addrlen) == 0)
           break;
       close(sockfd);
    }
    if(ressave == NULL)
        err_sys("connect error");
    freeaddrinfo(ressave);

	len = sizeof(ss);
	Getpeername(sockfd, (SA *)&ss, &len);
	printf("connected to %s\n", Sock_ntop_host((SA *)&ss, len));

	while ( (n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}
	exit(0);
}
