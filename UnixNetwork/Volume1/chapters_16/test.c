#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int sockfd, n;

    int error = 0;
    socklen_t len;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    len = sizeof(error);
    n = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, & error, &len);
    printf("n = %d\n", n);
    exit(0);
}
