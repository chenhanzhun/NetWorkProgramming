/* 读、写多个非连续的缓冲区 */

/*
 * 函数功能：读取数据到多个非连续的缓冲区，或从多个非连续缓冲区写数据到文件；
 * 返回值：若成功则返回已读、写的字节数，若出错则返回-1；
 * 函数原型：
 */
#include <sys/uio.h>
ssize_t readv(int filedes, const struct iovec *iov, int iovcnt);
ssize_t writev(int filedes, const struct iovec *iov, int iovcnt);
/*
 * 说明：
 * iovec的指针结构如下：
 */
struct iovec
{
    void *iov_base;     /* starting address of buffer */
    size_t iov_len;     /* size of buffer */
};
