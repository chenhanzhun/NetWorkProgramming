#ifndef PING_H
#define PING_H

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>

#define BUFSIZE 4096

char sendbuf[BUFSIZE];

extern int datalen;            /* # bytes of data following ICMP header */
char *host;
int nsent;              /* add 1 for each sendto() */
int nrecv;              /* add 1 for each recvmsg() */
pid_t pid;              /* our PID */
int sockfd;
int verbose;


/* function prototypes */
void init_v6(void);
void proc_v4(char *, ssize_t, struct msghdr *, struct timeval *);
void proc_v6(char *, ssize_t, struct msghdr *, struct timeval *);
void send_v4(void);
void send_v6(void);
void readloop(void);
void sig_alrm(int);
void tv_sub(struct timeval *, struct timeval *);

/* 这个结构主要是为了处理IPv4与IPv6之间的差异 */
struct proto
{
    /* 3个函数指针 */
    void (*fproc)(char *, ssize_t, struct msghdr *, struct timeval *);
    void (*fsend)(void);
    void (*finit)(void);

    /* 2个套接字地址结构指针 */
    struct sockaddr *sasend;    /* sockaddr{} for send, from getaddrinfo */
    struct sockaddr *sarecv;    /* sockaddr for receiving */

    socklen_t salen;            /* length of sockaddr{}s */
    /* ICMP 协议值 */
    int icmpprot;               /* IPPROTO_xxx value for ICMP */
} *pr;

#ifdef IPV6

#include <netinet/ip6.h>
#include <netinet/icmp6.h>

#endif
#endif
