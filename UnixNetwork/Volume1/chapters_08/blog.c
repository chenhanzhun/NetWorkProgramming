/* 函数功能：发送数据；
 * 返回值：若成功则返回已发送的字节数，若出错则返回-1；
 * 函数原型：
 */
#include <sys/socket.h>

ssize_t sendto(int sockfd, void *buff, size_t nbytes, int flags,
                const struct sockaddr *destaddr, socklen_t addrlen);

/* 说明：
 * 该函数功能类似于write函数，除了有标识符flags和目的地址信息之外，其他参数一样；
 *
 * flags标识符取值如下：
 * （1）MSG_DONTROUTE   勿将数据路由出本地网络
 * （2）MSG_DONTWAIT    允许非阻塞操作
 * （3）MSG_EOR         如果协议支持，此为记录结束
 * （4）MSG_OOB         如果协议支持，发送带外数据
 *
 * 若sendto成功，则只是表示已将数据无错误的发送到网络，并不能保证正确到达对端；
 * 该函数通过指定目标地址允许在无连接的套接字之间发送数据（例如UDP套接字）；
 */

 /* 函数功能：接收数据；
  * 返回值：以字节计数的消息长度，若无可用消息或对方已经按序结束则返回0，若出错则返回-1；
  * 函数原型：
  */
#include <sys/socket.h>

ssize_t recvfrom(int sockfd, void *buff, size_t nbytes, int flags,
                struct sockaddr *addr, socklen_t *addrlen);
 /* 说明：
  * 该函数功能与read类似；
  * 若addr为非空时，它将包含数据发送者的套接字地址；
  *
  * flags标识符取值如下：
  * （1）MSG_WAITALL     等待所有数据可用
  * （2）MSG_DONTWAIT    允许非阻塞操作
  * （3）MSG_PEEK        查看已读取的数据
  * （4）MSG_OOB         如果协议支持，发送带外数据
  */
