/* 终端IO */

/* 终端IO的数据结构 */
#include <termios.h>
struct termios
{
    tcflag_t    c_iflag;    /* input flag */
    tcflag_t    c_oflag;    /* output flag */
    tcflag_t    c_cflag;    /* control flag */
    tcflag_t    c_lflag;    /* local flag */
    cc_t        c_cc[NCCS]; /* control characters */
};
/*_______________________________________
 *   函数             说明
 *_______________________________________
 * tcgetattr        获取属性
 * tcsetattr        设置属性
 *_______________________________________
 * cfgetispeed      得到输入速度
 * cfgetospeed      得到输出速度
 * cfsetispeed      设置输入速度
 * cfsetospeed      设置输出速度
 *_______________________________________
 * tcdrain          等待所有输出都被传输
 * tcflow           挂起传输或接收
 * tcflush          刷清未决输入和/或输出
 * tcsendbreak      送BREAK字符
 *_______________________________________
 * tcgetpgrp        得到前台进程组ID
 * tcsetpgrp        设置前台进程组ID
 * tcgetsid         得到控制TTY的会话首进程的进程组ID
 *_______________________________________
 *
 */
/*
 * 函数功能：获取和设置终端属性；
 * 返回值：若成功则返回0，出错则返回-1；
 * 函数原型：
 */
#include <termios.h>

int tcgetattr(int filedes, struct termios *termptr);/* 获取终端属性 */

int tcsetattr(int filedes, int opt, const struct termios *termptr);/* 设置终端属性 */
/*
 * 说明：
 * filed是终端设备描述符；
 *
 * opt参数可以指定为以下的值：
 * TCSANOW          更改立即生效；
 * TCSADRAIN        发送所有输出后更改才发生，若更改输出参数则应该使用此选项；
 * TCSAFLUSH        发送所有输出后更改才发生，更进一步，在更改发生时未读的所有输入数据都被删除；
 */
/*
 * 函数功能：获取和设置终端波特率；
 * 函数原型：
 */
#include <termios.h>
/* 返回值：若成功则返回波特率值 */
speed_t cfgetispeed(const struct termios *termptr);/* 获取输入波特率 */

speed_t cfgetospeed(const struct termios *termptr);/* 获取输出波特率 */
/* 返回值：若成功则返回0，出错则返回-1；*/
int cfsetispeed(struct termios *termptr, speed_t speed);/* 设置输入波特率 */

int cfsetospeed(struct termios *termptr, speed_t speed);/* 设置输出波特率 */

/*
 * 函数功能：终端设备的行控制；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <termios.h>

int tcdrain(int filedes);
int tcflow(int filedes, int action);
int tcflush(int filedes, int queue);
int tcsendbreak(int filedes, int duration);
/*
 * 说明：
 * action参数取值如下：
 * TCOOFF       输出被挂起；
 * TCOON        重新启动以前被挂起的输出；
 * TCIOFF       系统发送一个STOP字符，将使终端设备暂停发送数据；
 * TCION        系统发送一个START字符，将使终端设备恢复发送数据；
 *
 * queue参数取值如下：
 * TCIFUSH      刷清输入队列；
 * TCOFUSH      刷清输出队列；
 * TCIOFUSH      刷清输入、输出队列；
 */

/* 终端标识 */

/*
 * 函数功能：确定控制终端的名字；
 * 返回值：若成功则返回指向控制终端名的指针，出错则返回指向空字符串的指针；
 * 函数原型：
 */
#include <stdio.h>
char * ctermid(char *ptr);
/*
 * 说明：
 * ptr非null，且指向长度为L_ctermid字节的数组，进程的控制终端名存放在该数组中；
 * ptr为null，则该函数为数组分配空间，进程的控制终端名也放在该数组中；
 */

/*
 * 函数功能：控制终端的操作；
 * 函数原型：
 */
#include <unistd.h>
int isatty(int filedes);
/* 返回值：若为终端设备则返回1（真），否则返回0（假）*/

char *ttyname(int filedes);
/* 返回值：指向终端路径名的指针，若出错则返回NULL */

/* 终端的窗口大小 */

/* 窗口大小的结构体 */
struct winsize
{
    unsigned short ws_row;      /* row, in characters */
    unsigned short ws_col;      /* columns, in characters */
    unsigned short ws_xpixel;   /* horizontal size, pixels (unused) */
    unsigned short ws_ypixel;   /* vertical size, pixels (unused) */
};
