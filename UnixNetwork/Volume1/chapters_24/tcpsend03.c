#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>

extern int my_connect(const char *, const char *);
int main(int argc, char **argv)
{
    int sockfd;
    if(argc != 3)
    {
        perror("usage: %s <host> <port#>");
        exit(1);
    }

    sockfd = my_connect(argv[1], argv[2]);

    /* 发送普通数据 */
    write(sockfd, "123", 3);
    printf("wrote 3 bytes of normal data\n");

    /* 发送带外数据 */
    send(sockfd, "4", 1, MSG_OOB);
    printf("wrote 1 byte of OOB data\n");

    write(sockfd, "56", 2);
    printf("wrote 2 bytes of normal data\n");

    send(sockfd, "7", 1, MSG_OOB);
    printf("wrote 1 byte of OOB data\n");

    write(sockfd, "89", 2);
    printf("wrote 2 bytes of normal data\n");

    exit(0);
}
