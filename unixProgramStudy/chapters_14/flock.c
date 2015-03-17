/* fcntl记录锁 */
/*
 * 函数功能：记录锁；
 * 返回值：若成功则依赖于cmd的值，若出错则返回-1；
 * 函数原型：
 */
#include <fcntl.h>
int fcntl(int fd, int cmd, .../* struct flock *flockptr */);
/*
 * 说明：
 * cmd的取值可以如下：
 * F_GETLK              获取文件锁
 * F_SETLK、F_SETLKW    设置文件锁
 * 第三个参数flockptr是一个结构指针，如下：
 */
struct flock
{
    short l_type;       /* F_RDLCK, F_WRLCK, F_UNLCK */
    off_t l_start;      /* offset in bytes, relative to l_whence */
    short l_whence;     /* SEEK_SET, SEEK_CUR, SEEK_END */
    off_t l_len;        /* length, in bytes; 0 means lock to EOF */
    pid_t l_pid;        /* returned with F_GETLK */
};
