/************************
 * 以下的操作必须保证进程的有效用户ID必须等于文件的所有者ID，或者进程必须具有超级用户权限；
 * 函数功能：改变现有文件的访问权限；
 * 函数原型：
 *
 * int chmod(const char *pathname, mode_t mode);
 * int fchmod(int filedes, mode_t mode);
 * 说明:
 * chomd 函数是在指定的文件pathname进行操作;
 * fchmod 函数是在已打开的文件进行操作;
 * 参数说明:
 * mode 为以下权限值：
 *
 ***********************************
 * S_ISUID          设置用户ID
 * S_ISGID          设置组ID
 * S_ISVTX          保存正文(粘住性)
 * *********************************
 * S_IRWXU          用户读、写和执行
 *      S_IRUSR       用户-读
 *      S_IWUSR       用户-写
 *      S_IXUSR       用户-执行
 ***********************************
 * S_IRWXG          组读、写和执行
 *      S_IRGRP       组-读
 *      S_IWGRP       组-写
 *      S_IXGRP       组-执行
 ***********************************
 * S_IRWXO          其他读、写和执行
 *      S_IROTH       其他-读
 *      S_IWOTH       其他-写
 *      S_IXOTH       其他-执行
 *****************************
 */
//其中缩进的9个权限是前面介绍的文件权限

#include "apue.h"

int main(void)
{
    struct stat buf;

    if(stat("umask",&buf) < 0)
        err_sys("stat error for umask.");
    if(chmod("umask",(buf.st_mode & ~S_IXGRP) | S_ISGID) < 0)
        err_sys("chmod error for umask.");

    if(chmod("bar",S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0)
        err_sys("chmod error for umask.");
    exit(0);


}

/******************************************
 * 函数功能：更改文件用户ID和组ID
 * 返回值：若成功返回0，若出错返回-1；
 * 函数原型：
 * int chown(const char *pathname, uid_t owner, gid_t group);
 * int fchown(int filedes, uid_t owner, gid_t group);
 * int lchown(const char *pathname, uid_t owner, gid_t group);
 * 若文件是符号链接时，lchown更改的是符号链接本身的所有者，而不是符号链接所指向的文件；
 * 其他操作相似；
 * 若参数owner或group的值为-1时，表示不更改对应的ID;
 */
