#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>

int my_connect(const char *host, const char *serv)
{
    int sockfd, n;
    struct addrinfo hints, *res, *ressave;
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if( (n = getaddrinfo(host, serv, &hints, &res)) < 0)
    {
        perror("error");
        exit(-1);
    }
    ressave = res;

    do{
    
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(sockfd < 0)
            continue;
        if(connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;
        close(sockfd);
    }while( (res = res->ai_next) != NULL);

    if(res == NULL)
    {
        perror("connect error");
        exit(-1);
    }

    freeaddrinfo(ressave);
    return(sockfd);
}
