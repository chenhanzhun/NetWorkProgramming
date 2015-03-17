/* 命名STREAM 管道 */
/*
 * 函数功能：使进程给予STREAM管道一个文件系统中的名字；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <stropts.h>
int fattach(int filedes, const char *path);
/*
 * 说明：
 * path必须是引用一个现有的文件，且对该文件具有写权限；
 */
/*
 * 函数功能：撤销STREAM管道与一个文件系统中的名字的关联；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <stropts.h>
int fdetach(const char *path);
/*
 * 函数功能：创建在无关进程之间的唯一连接；
 * 函数原型：
 */
#include "apue.h"

int serv_listen(const char *name);
/* 返回值：若成功则返回要侦听的文件描述符，出错则返回负值；*/

int serv_accept(int listenfd, uid_t *uidptr);
/* 返回值：若成功则返回新文件描述符，出错则返回负值 */

int cli_conn(const char *name);
/* 返回值：若成功则返回文件描述符，出错则返回负值 */

/*
 * 说明：
 *
 * 服务器进程调用serv_listen函数声明要在文件系统中的某个路径侦听客户进程的连接请求；
 * 当客户端想要连接至服务器进程，就将使用该文件系统中的名字，该函数返回值是STREAMS管道的服务器进程端；
 *
 * 服务器进程调用serv_accept函数等待客户进程连接请求的到达，当一个请求到达时，系统自动创建一个新的STREAMS管道，
 * 该函数向服务器进程返回该STREAMS管道的一端，客户进程的有效用户ID存放在uidptr所指向的存储区中；
 *
 * 客户端进程调用cli_conn函数连接至服务器进程，客户端进程指定的参数name必须和服务器进程调用serv_listen函数时所用的参数name相同；
 * 该函数返回时，客户端进程得到连接至服务器进程的文件描述符；
 *
 */
