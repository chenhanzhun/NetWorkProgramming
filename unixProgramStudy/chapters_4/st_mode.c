#include "apue.h"

/*
 * S_ISREG      普通文件;
 * S_ISBLK      块特殊文件;
 * S_ISCHR      字符特殊文件;
 * S_ISDIR      目录文件;
 * S_ISLNK      符号链接;
 * S_ISFIFO     命令管道;
 * S_ISSOCK     套接字;
 */

int main(int argc, char*argv[])
{
    int i;
    struct stat buf;
    char *ptr;

    for(i=1;i<argc;i++)
    {
        printf("%s: ",argv[i]);
        if(lstat(argv[i],&buf) < 0)
        {
            err_ret("last error.");
            continue;
        }
        if(S_ISREG(buf.st_mode))
            ptr = "regular file.";
        else if(S_ISDIR(buf.st_mode))
            ptr = "directory file.";
        else if(S_ISCHR(buf.st_mode))
            ptr = "character special file";
        else if(S_ISBLK(buf.st_mode))
            ptr = "block special  file";
        else if(S_ISFIFO(buf.st_mode))
            ptr = "FIFO file";
        else if(S_ISLNK(buf.st_mode))
            ptr = "symbolic link file";
        else if(S_ISSOCK(buf.st_mode))
            ptr = "sock file";
        else
            ptr ="** unknown mode **";
        printf("%s\n",ptr);

    }
    exit(0);
}
