/* 进程组 */

/*
 * 函数功能：获取进程组ID；
 * 函数原型：
 */
#include <unistd.h>

pid_t getpgrp(void);    /* 返回值：调用进程的进程组ID */
pid_t getpgid(pid_t pid);/* 返回值：若成功则返回进程组ID，若出错则返回-1 */

/*
 * 函数功能：加入一个现有进程组或者创建一个新的进程组；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
int setpgid(pid_t pid, pid_t pgid);/* 将pid进程的进程组ID设置为pgid */
/* 说明：
 * 若两参数相等，则由pid指定的进程变成进程组组长；
 * 如果pid = 0，则使用调用者的进程ID；
 * 若pgid = 0，则由pid指定的进程ID将用作进程组ID；
 */
