#include <bits/msq.h>

struct msqid_ds
{
  struct ipc_perm msg_perm;     /* IPC对象的属性信息和访问权限 */
  struct msg *msg_first;        /* 指向消息队列的第一个消息 */
  struct msg *msg_last;         /* 指向消息队列的最后一个消息 */
  time_t msg_stime;             /* time of last msgsnd command */
  time_t msg_rtime;             /* time of last msgrcv command */
  time_t msg_ctime;             /* time of last change */
  unsigned long int msg_cbytes; /* 当前消息队列中消息的总字节数 */
  msgqnum_t msg_qnum;           /* 当前队列中消息的个数 */
  msglen_t msg_qbytes;          /* 队列允许存放的最大字节数 */
  pid_t msg_lspid;              /* pid of last msgsnd() 即最后执行msgsnd函数的进程的进程ID */
  pid_t msg_lrpid;              /* pid of last msgrcv() 即最后执行msgrcv函数的进程的进程ID */

};

struct msgbuf{
    long msgtype;
    char msgtext[1024];
};
/*
 * 函数功能：创建一个新的消息队列或打开一个现有的消息队列；
 * 返回值：若成功则返回消息队列的ID，若出错则返回-1；
 * 函数原型：
 */
#include <sys/msg.h>

int msgget(key_t key, int flag);
/*
 * 说明：
 * 参数key是消息队列的键；
 * 参数flag表示调用函数的操作类型，也可用于设置访问权限；
 */
/*
 * 函数功能：向消息队列中发送消息；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/msg.h>

int msgsnd(int msqid, const void *ptr, size_t nbytes, int flag);
/*
 * msqid是消息队列的引用标识符；
 * ptr是一个void指针，指向要发送的消息；
 * nbytes表示要发送消息的字节数；
 * flag用于指定消息队列已满时的处理方法，当消息队列为满时，若设置为IPC_NOWAIT，则立刻出错返回EAGAIN；
 * 否则发送消息的进程被阻塞，直到消息队列中空间或消息队列被删除或捕捉到信号时，函数返回；
 */
/*
 * 函数功能：从消息队列中接收消息；
 * 返回值：若成功则返回消息的数据部分的长度，若出错则返回-1；
 * 函数原型：
 */
#include <sys/msg.h>

ssize_t msgrcv(int msqid, void *ptr, size_t nbytes, long type, int flag);
/*
 * msqid是消息队列的引用标识符；
 * ptr是一个void指针，指向要存放消息数据的缓冲区；
 * nbytes表示要存放消息数据缓冲区的长度；
 *
 * 当返回的消息实际长度大于nbytes时，根据flag的设置进行处理：若设置为MSG_NOERROR，则消息被截短，否则出错返回E2BIG；
 * 若指定的type无效时，若flag设置为IPC_NOWAIT，则立即出错返回，且errno设为ENOMSG，否则接收消息的进程将被阻塞，
 * 直到type有效或者消息队列被删除或者捕捉到信号；
 *
 * type的取值如下：
 * （1）type=0  接收消息队列中的第一条消息；
 * （2）type>0  接收消息队列中类型为type的第一条消息；
 * （3）type<0  接收消息队列中类型值小于或等于type绝对值的所有消息中类型最小的消息中的第一条消息；
 * type为非0，则用于非先进先出顺序读消息；
 */
/*
 * 函数功能：消息队列的控制；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/msg.h>
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
