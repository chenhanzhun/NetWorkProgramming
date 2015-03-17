/**********************
 * 函数功能：创建一个指向现有文件的链接；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 * int link(const char *existingpath, const char *newpath);
 * 此函数创建一个新的目录项newpath，引用现有的文件existingpath。
 * 若newpath已经存在，则返回出错。只创建newpath中的最后一个分量，
 * 路径中的其他部分应该已经存在。
 *
 *
 * 函数功能：删除一个现有的目录项；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 * int unlink(const char *pathname);
 *
 * 此函数删除现有的目录项，并将由pathname所引用文件的链接计数减1。
 * 如果存在指向该文件的其他链接，则仍然可以通过其他链接访问该文件。
 * *********************/

#include "apue.h"
#include <fcntl.h>

int main(void)
{
    if (open("access.c",O_RDWR) < 0)
        err_sys("open error.");
    if (unlink("access.c") < 0)
        err_sys("unlink error.");
    printf("file unlink.\n");
    sleep(15);
    printf("done.\n");
    exit(0);
}


/*******************************
 * 函数功能：解除对一个目录或文件的链接，对于文件，remove功能和unlink相同
 *          对于目录，remove功能和rmdir相同；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 * int remove(const char *pathname);
 *
 *
 * 函数功能：更改现有文件或目录的名字；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 * int rename(const char *oldname, const char *newname);
 ************************************/

/*********************************
 * 函数功能：截短文件长度；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 * int truncate(const char *pathname, off_t length);
 * int ftruncate(int filedes, off_t length);
 * 说明：
 * length是文件截短后的长度；
 * 若原始文件长度大于length，则length以外的数据不再访问；
 * 若原始文件长度小于length，则根据不同的系统增加该文件的长度；
 *********************************/
