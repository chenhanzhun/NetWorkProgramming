/**********
 * 文件时间
 * 根据stat的结构可以知道，文件有三种时间：
 * st_atime     最后访问时间，即最后访问文件的时间
 * st_mtime     最后修改时间，即修改文件内容的时间
 * st_ctime     最后更改时间，即更改inode节点的时间
 * ****/
/**********
 * utime函数
 * 函数功能：更改文件访问时间和文件修改时间；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 * int utime(const char *pathname, const struct utimbuf *times);
 * 其中结构体struct utimbuf 定义如下：
 * struct utimbuf{
 *      time_t actime;// access time 
 *      time_t modtime;// modification time
 *      };
 *  参数说明：
 *  pathname是要更改时间的文件；
 *  times若为空指针，则表示设置为当前时间；
 *  若times非空指针，则设置为所指向结构体中的对应值；
 *  ********/

#include "apue.h"
#include <fcntl.h>
#include <utime.h>

int main(int argc, char *argv[])
{
    int i,fd;
    struct stat statbuf;
    struct utimbuf timebuf;

    for(i=1;i<argc;i++)
    {
        if(stat(argv[i],&statbuf) < 0){
            err_ret("%s:stat error.\n",argv[i]);
            continue;
        }
        if((fd = open(argv[i],O_RDWR | O_TRUNC)) < 0){
            err_ret("%s:open error.\n",argv[i]);
            continue;
        }
        close(fd);
        timebuf.actime = statbuf.st_atime;
        timebuf.modtime = statbuf.st_mtime;
        if(utime(argv[i],&timebuf) < 0){
            err_ret("%s:utime error.\n",argv[i]);
            continue;
        }
    }
    exit(0);
}
