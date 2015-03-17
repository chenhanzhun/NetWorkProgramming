#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>

extern int my_connect(const char *, const char *);
int main(int argc, char **argv)
{
    int sockfd, size;
    /* 设置套接字发送缓冲区写入普通数据16384 */
    char buff[16384];
    if(argc != 3)
    {
        perror("usage: %s <host> <port#>");
        exit(1);
    }

    sockfd = my_connect(argv[1], argv[2]);

    /* 设置套接字发送缓冲区的大小为32768*/
    size = 32768;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
    /* 发送普通数据 */
    write(sockfd, buff, 16384);
    printf("wrote 16384 bytes of normal data\n");
    /* sleep函数的作用是让套接字处于阻塞状态，
     * 使write和send的数据能够作为单个TCP分段发送到对端 */
    sleep(5);
    /* 发送带外数据 */
    send(sockfd, "d", 1, MSG_OOB);
    printf("wrote 1 byte of OOB data\n");

    write(sockfd, buff, 1024);
    printf("wrote 1024 bytes of normal data\n");

    exit(0);
}
