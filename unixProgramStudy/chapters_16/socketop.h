/* 套接字的基本操作 */

/*
 * 函数功能：将地址绑定到一个套接字；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *addr, socklen_t len);
/*
 * 说明：
 * sockfd 为套接字描述符；
 * addr是一个指向特定协议地址结构的指针；
 * len是地址结构的长度；
 */
/*
 * 函数功能：获取已绑定到一个套接字的地址；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *alenp);
/*
 * 说明：
 * 调用该函数之前，设置alenp为一个指向整数的指针，该整数指定缓冲区sockaddr的大小；
 * 返回时，该整数会被设置成返回地址的大小，如果该地址和提供的缓冲区长度不匹配，则将其截断而不报错；
 */
/*
 * 函数功能：获取套接字对方连接的地址；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

int getpeername(int sockfd, struct sockaddr *addr, socklen_t *alenp);
/*
 * 说明：
 * 该函数除了返回对方的地址之外，其他功能和getsockname一样；
 */
/*
 * 函数功能：建立连接；
 * 返回值：若成功则返回0，出错则返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t len);
/*
 * 说明：
 * sockfd是系统调用的套接字描述符；
 * addr是目的套接字的地址，即想与之通信的服务器地址；
 * len是目的套接字的大小；
 *
 * 如果sockfd没有绑定到一个地址，connect会给调用者绑定一个默认地址；
 */
/*
 * 函数功能：接收连接请求；
 * 函数原型：
 */
#include <sys/socket.h>

int listen(int sockfd, int backlog);//若成功则返回0，若出错则返回-1；
/*
 * sockfd是套接字描述符；
 * backlog是该进程所要入队请求的最大请求数量；
 */
int accept(int sockfd, struct sockaddr *addr, socklen_t *len);//返回值：若成功返回套接字描述符，出错返回-1；
/*
 * 说明：
 * 该函数返回套接字描述符，该描述符连接到调用connect函数的客户端；
 * 这个新的套接字描述符和原始的套接字描述符sockfd具有相同的套接字类型和地址族，而传给accept函数的套接字描述符sockfd没有关联到这个链接，
 * 而是继续保持可用状态并接受其他连接请求；
 * 若不关心客户端协议地址，可将addr和len参数设置为NULL，否则，在调用accept之前，应将参数addr设为足够大的缓冲区来存放地址，
 * 并且将len设为指向代表这个缓冲区大小的整数指针；
 * accept函数返回时，会在缓冲区填充客户端的地址并更新len所指向的整数为该地址的实际大小；
 *
 * 若没有连接请求等待处理，accept会阻塞直到一个请求到来；
 */
