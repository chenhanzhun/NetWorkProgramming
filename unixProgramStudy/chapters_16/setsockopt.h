/* 套接字选项 */
/*
 * 函数功能：设置套接字选项；
 * 返回值：若成功返回0，若出错返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

int setsockopt(int sockfd, int level, int option, const void *val, socklen_t len);
/*
 * 说明：
 * level表示选项应用的协议，若选项是通用套接字选项，则level设置为SOL_SOCKET，否则设置为控制这个协议的协议号；
 * val根据选项不同指向一个数据结构或者一个整数，若整数非零，则启用选项，若整数为零，则禁止选项；
 * len指定了val指向的对象的大小；
 */
/*
 * 函数功能：获取套接字选项的当前值；
 * 返回值：若成功返回0，若出错返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

int getsockopt(int sockfd, int level, int option, void *val, socklen_t *lenp);
/*
 * 说明：
 * lenp是指向整数的指针，在调用该函数之前，设置该参数为复制选项缓冲区的大小；
 * 其他参数和setsockopt函数类型；
 */
/*
 * 函数功能：判断是否接收到紧急标记；
 * 返回值：若在标记处则返回0，若没有在标记处则返回0，若出错返回-1；
 * 函数原型：
 */
#include <sys/socket.h>
int sockatmark(int sockfd);
/*
 * 说明：
 * 当下一个要读的字节在紧急标记所标识的位置，则该函数返回1；
 */
