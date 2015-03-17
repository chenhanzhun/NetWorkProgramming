#include "apue.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd;
    void *dst;
    struct stat f_stat;
    char buf[MAXLINE];
    memset(buf, 0, MAXLINE);
    char *src = "mmap munmap msync 12";
    if(argc != 2)
        err_quit("usage: a.out <pathname>");
    if((fd = open(argv[1], O_RDWR)) < 0)
        err_sys("open argv[1] file error");

    if(fstat(fd, &f_stat) == -1 )
        err_sys("fstat error");
    if((dst = mmap(NULL, f_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
        err_sys("mmap error");

    close(fd);
    memcpy(dst, src, 20);
    if((msync(dst, f_stat.st_size, MS_SYNC)) == -1)
        err_sys("msync error");

    if((munmap(dst, f_stat.st_size)) == -1)
        err_sys("munmap error");
    exit(0);
}
