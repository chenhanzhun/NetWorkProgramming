/* 更改进程ID */

/*
 * 函数功能：设置用户或组ID（包括有效、实际和保存的设置用户的ID）；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <unistd.h>

int setuid(uid_t uid);
int setgid(gid_t gid);

/*
 * 函数功能：交换实际用户（组）ID和有效用户（组）ID；
 * 返回值：若成功则返回0，若出错则返回-1；
 */
#include <unistd.h>

int setreuid(uid_t ruid, uid_t euid);
int setregid(gid_t rgid, gid_t egid);
/* 说明：
 * 若其中任一参数值为-1，则表示ID不变；
 */

/*
 * 函数功能：更改有效用户或有效组ID；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <unistd.h>

int seteuid(uid_t uid);
int setegid(gid_t gid);
