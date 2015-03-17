#include	"unp.h"
#include	<time.h>

int
main(int argc, char **argv)
{
	int				listenfd, connfd;
	socklen_t		len;
	char			buff[MAXLINE];
	time_t			ticks;
	struct sockaddr_storage	cliaddr;

	if (argc != 2)
		err_quit("usage: daytimetcpsrv1 <service or port#>");

    int n;
    const int on = 1;
    struct addrinfo hints, *res, *ressave;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if( (n = getaddrinfo(NULL, argv[1], &hints, &res)) != 0)
        err_quit("error, %s", gai_strerror(n));

    for(ressave = res; ressave != NULL; ressave = ressave->ai_next)
    {
        listenfd = socket(ressave->ai_family, ressave->ai_socktype, ressave->ai_protocol);
        if(listenfd < 0)
            continue;
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
        if(bind(listenfd, ressave->ai_addr, ressave->ai_addrlen) == 0)
            break;
        close(listenfd);
    }

    if(ressave == NULL)
        err_sys("server error");

    listen(listenfd, LISTENQ);

    freeaddrinfo(ressave);
	for ( ; ; ) {
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
        if(connfd < 0)
            err_sys("accept error");
		printf("connection from %s\n", Sock_ntop((SA *)&cliaddr, len));

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		Write(connfd, buff, strlen(buff));

		close(connfd);
	}
}
