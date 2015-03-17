/* 函数功能：确认套接字是否处于带外标记；
 * 返回值：若处于带外标记则返回1，若不处于带外标记则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/socket.h>
int sockatmark(int sockfd);
