/* UDP 客户端 */
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_PORT 9877 /* 通用端口号 */

extern void err_sys(const char *, ...);
extern void err_quit(const char *, ...);
extern void dg_cli(FILE *fd, int sockfd, struct sockaddr *addr, socklen_t addrlen);

int main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: udpcli <IPaddress>");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        err_sys("socket err");

	dg_cli(stdin, sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	exit(0);
}
