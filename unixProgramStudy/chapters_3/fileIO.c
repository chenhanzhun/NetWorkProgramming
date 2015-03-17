#include "apue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define buf_size 4096
#define offset 1024

int main(void)
{
    const char* src_path = "/home/nifengweijifen/linuxStudy/unixProgramStudy/Blogs/10_22.txt";
    const char* des_path ="./test.txt";
    int  fd,fs;
    ssize_t len;
    off_t num;
    char buf[buf_size];
    fd = open(src_path,O_RDONLY);
    if(fd == -1)
        printf("open the file error.\n");
    fs = open(des_path,O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if(fs == -1)
        printf("open or creat the file error.\n");
    num = lseek(fd,offset,SEEK_CUR);
    printf("the offset of lseek is %d.\n",num);
    while((len = read(fd,buf,sizeof(buf))) > 0)
        write(fs,buf,len);
    close(fd);
    close(fs);
    return 0;
}
