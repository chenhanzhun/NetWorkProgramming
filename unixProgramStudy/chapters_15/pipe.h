/* 管道 */
/*
 * 函数功能：创建一个管道；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <unistd.h>
int pipe(int filedes[2]);
/*
 * 说明：
 * 该函数的参数是一个二元整数数组，用于存放调用该函数所创建管道的两个文件描述符；
 * filedes[0]为读而打开，存放管道的读取端的文件描述符；
 * filedes[1]为写而打开，存放管道的写入端的文件描述符；
 * filedes[1]的输出是filedes[0]的输入；
 * 内核对于管道的filedes[0]以只读方式打开的，filedes[1]是以只写方式打开的，所以管道只能保证单向的数据通信;
 */

/* popen 和 pclose 函数 */
/*
 * 函数功能：创建一个管道链接到另一个进程，实现管道数据传输；
 * 函数原型：
 */
#include <stdio.h>
FILE *popen(const char *cmdstring, const char *type);//返回值：若成功则返回文件指针，若出错则返回NULL；

int pclose(FILE *fp);//返回值：cmdstring的终止状态，若出错则返回-1；
/*
 * 说明：
 * cmdstring是要执行的 shell 命令；
 * type参数有如下取值：
 * （1）type = "r"  文件指针连接到cmdstring标准输出；
 * （2）type = "w"  文件指针连接到cmdstring标准输入；
 */

/* FIFO 命名管道 */
/*
 * 函数功能：功能和管道类似；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/stat.h>

int mkfifo(const char *pathname, mode_t mode);
/*
 * 说明：
 * 参数mode和open函数的mode参数一样；
 * pathname：一个 Linux 路径名，它是 FIFO 的名字。即每个 FIFO 与一个路径名相对应。
 * mode：指定的文件权限位，类似于 open 函数的第三个参数。
 * 即创建该 FIFO 时，指定用户的访问权限，可以取以下值：
 * S_IRUSR，S_IWUSR，S_IRGRP，S_IWGRP，S_IROTH，S_IWOTH。
 * 该函数创建一个FIFO之后，就可用open函数打开；
 */
