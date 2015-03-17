/* 流 */
/*
 * 函数功能：将STREAMS消息写至流；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <stropts.h>
int putmsg(int filedes, const struct strbuf *ctlptr, const struct strbuf *datptr, int flags);
int putpmsg(int filedes, const struct strbuf *ctlptr, const struct strbuf *datptr, int band, int flags);
/*
 * 说明：
 * 对流使用write函数等价于不带任何控制信息、flags为0的putmsg函数；
 * 这两函数可以产生三种不同优先级的消息：普通、优先级波段和高优先级；
 */
/*
 * 函数功能：判断描述符是否引入一个流；
 * 返回值：若为STREAMS设备则返回1，否则返回0；
 * 函数原型：
 */
#include <stropts.h>
int isastream(int filedes);
/*
 * 说明：
 * 该函数是通过ioctl函数来进行的，可有如下实现：
 */
#include <stropts.h>
#include <unistd.h>
int isastream(int fd)
{
    return(ioctl(fd, I_CANPUT, 0) != -1);
}
/*
 * 函数功能：将从流读STREAMS消息；
 * 返回值：若成功则返回非负值，若出错则返回-1；
 * 函数原型：
 */
#include <stropts.h>
int getmsg(int filedes, const struct strbuf *ctlptr, const struct strbuf *datptr, int *flagptr);
int getpmsg(int filedes, const struct strbuf *ctlptr, const struct strbuf *datptr, int *bandptr, int *flagptr);
/*
 * 说明：
 * 如果flagptr指向的整型单元的值是0，则getmsg返回流首读队列中的下一个消息；
 * 如果下一个消息是最高优先级消息，则在返回时，flagptr所指向的整型单元设置为RS_HIPRI；
 * 如果只希望接收高优先级消息，则在调用getmsg之前必须将flagptr所指向的整型单元设置为RS_HIPRI;
 * getmsg可以设置待接收消息的优先级波段；
 */
