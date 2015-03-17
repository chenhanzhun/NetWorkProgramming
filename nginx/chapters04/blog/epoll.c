/* epoll 系统调用函数 */

#include <sys/epoll.h>

/*
 * 函数功能：创建epoll文件描述符；
 * 返回值：若成功则返回新创建的文件描述符；
 * 函数原型：
 */
int epoll_create(int size);
/*
 * 参数size是epoll的最大文件描述符个数；
 * 在新的系统内核中size已经不被使用；
 */

/*
 * 函数功能：操作某个epoll文件描述符；
 * 返回值：若成功则返回0，否则出错返回-1；
 * 函数原型：
 */
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
/*
 * 参数：
 * epfd：由epoll_create创建的epoll文件描述符；
 * fd：是关联的文件描述符；
 * op：是操作方式，有以下三种操作方式：
 *      EPOLL_CTL_ADD   将fd注册到epfd中；
 *      EPOLL_CTL_MOD   修改已在epfd中注册的fd事件；
 *      EPOLL_CTL_DEL   将fd从epfd中删除；
 *
 * event：指向struct epoll_event 结构，表示需要监听fd的某种事件；
 */
/* struct epoll_event 结构体定义如下 */
typedef union epoll_data {
void        *ptr;
int          fd;
uint32_t     u32;
uint64_t     u64;
} epoll_data_t;

struct epoll_event {
uint32_t     events;      /* Epoll events */
epoll_data_t data;        /* User data variable */
};
/*
 * 其中events有如下的取值：
 *  EPOLLIN         表示对应的文件描述符可读；
 *  EPOLLOUT        表示对应的文件描述符可写；
 *  EPOLLPRI        表示对应的文件描述符有紧急数据可读；
 *  EPOLLERR        表示对应的文件描述符发生错误；
 *  EPOLLHUP        表示对应的文件描述符被挂载；
 *  EPOLLET         表示将EPOLL设置为边缘触发模式(Edge Triggered)；
 *  EPOLLONESHOT    表示只监听一次事件，当监听此次事件完成，若想继续监听，则需再次把该套接字描述符加入到EPOLL队列中；
 */
struct epoll_event ev;
//设置与要处理的事件相关的文件描述符
ev.data.fd=listenfd;
//设置要处理的事件类型
ev.events=EPOLLIN|EPOLLET;
//注册epoll事件
epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);


/*
 * 函数功能：收集在epoll监听事件中已发生的事件；
 * 返回值：若成功则返回所发生的事件数，否则出错返回-1；
 * 函数原型：
 */
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
/*
 * 参数：
 * epfd：由epoll_create创建的epoll文件描述符；
 * events：指向epoll_event结构体，用于保存已发生的事件；
 * maxevents：每次能处理的最大事件数；
 * timeout：等待IO 事件发生的超时时间：-1相当于阻塞，即不会立即返回；0相当于非阻塞，即立即返回；
 */
