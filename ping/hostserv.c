#include <netdb.h>
#include <sys/socket.h>
#include <string.h>

/* 将主机名和服务名映射到一个地址 */
struct addrinfo *host_serv(const char *host, const char *serv, int family, int socktype)
{
    int n;
    struct addrinfo hints, *res;
    bzero(&hints, sizeof(struct addrinfo));

   hints.ai_flags = AI_CANONNAME;   /* always return canonical name */
   hints.ai_family = family;
   hints.ai_socktype = socktype;

   if( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
       return(NULL);
   return(res); /* return pointer to first on linked list */
}
