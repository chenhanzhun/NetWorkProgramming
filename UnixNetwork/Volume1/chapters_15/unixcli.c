#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


extern void err_sys(const char *, ...);
extern void err_quit(const char *, ...);
extern void str_cli(FILE *, int);

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_un	servaddr;

    if(argc != 2)
        err_quit("usage: %s <pathname>", argv[0]);
	if( (sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_UNIX;
	strcpy(servaddr.sun_path, argv[1]);

    int err;
	err = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if( err < 0)
        err_sys("connect error");

	str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}
