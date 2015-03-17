/**************************
 * 函数功能：为进程设置文件模式创建屏蔽字，并返回以前的值；
 * 函数原型：
 *
 * mode_t umask (mode_t cmask);
 * 参数说明
 * cmask是前面介绍的文件权限的9个值的若干个按位“或”构成，9个权限如下所示：
 **/
/*****************************
 * 文件访问权限
 * st_mode屏蔽    意义
 * S_IRUSR       用户-读
 * S_IWUSR       用户-写
 * S_IXUSR       用户-执行
 *****************************
 * S_IRGRP       组-读
 * S_IWGRP       组-写
 * S_IXGRP       组-执行
 *****************************
 * S_IROTH       其他-读
 * S_IWOTH       其他-写
 * S_IXOTH       其他-执行
 *****************************
 */
/* umask的主要作用是在创建文件时设置或者屏蔽文件的某些权限；
 * 下面举例子，在创建文件时指定文件的权限属性：mode &(~cmask);
 * 其中mode 是open或者creat函数的属性参数
 */




#include "apue.h"
#include <fcntl.h>

#define RWRWRW (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int main(void)
{
    umask(0);//set the first umask
    //此时cmask值为000
    if(creat("umask",RWRWRW) < 0)//creat参数的mode权限值为666
        err_sys("creat error for umask.");
    //最后创建成功的文件umask权限属性为mode &(~cmask),即为666
    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);//set the second umask
    //此时cmask权限值为066
    if(creat("bar",RWRWRW) < 0)//creat参数mode权限值依然为666
        err_sys("creat error for bar.");
    //成功创建的文件bar权限属性为mode &(~cmask),即为600
    exit(0);
}
