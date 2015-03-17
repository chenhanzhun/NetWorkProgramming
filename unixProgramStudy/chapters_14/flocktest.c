#include <fcntl.h>
#include "apue.h"

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len);
int lock_test(int fd, int type, off_t offset, int whence, off_t len);

int main(void)
{
    int fd, tmp;
    pid_t pid;
    pid = getpid();
    printf("pid: %d\n", pid);

    fd = open("lock.txt", O_RDWR);
    if(fd < 0)
        err_sys("open file error");

    tmp = lock_reg(fd, F_SETLK, F_WRLCK, 4, SEEK_SET, 3);
    if(tmp < 0)
        err_sys("F_SETLK error");
    else
        printf("F_SETLK success\n");

    sleep(3);
    tmp = lock_test(fd, F_WRLCK, 2, SEEK_SET, 2);
    if(tmp == 0)
        printf("not lock\n");
    else
        printf("locked\n");
    close(fd);
    exit(0);
}
int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_len = len;
    lock.l_start = offset;
    lock.l_type = type;
    lock.l_whence = whence;

    return(fcntl(fd, cmd, &lock));
}
int lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_len = len;
    lock.l_start = offset;
    lock.l_type = type;
    lock.l_whence = whence;

    if(fcntl(fd, F_GETLK, &lock) < 0)
        err_sys("fcntl error");
    if(lock.l_type == F_UNLCK)
        return(0);
    return(lock.l_pid);
}
