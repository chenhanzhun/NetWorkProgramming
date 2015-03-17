/* exec 序列函数 */

/*
 * 函数功能：把当前进程替换为一个新的进程，新进程与原进程ID相同；
 * 返回值：若出错则返回-1，若成功则不返回；
 * 函数原型：
 */
#include <unistd.h>
int execl(const char *pathname, const char *arg, ...);
int execv(const char *pathnam, char *const argv[]);
int execle(const char *pathname, const char *arg, ... , char *const envp[]);
int execve(const char *pathnam, char *const argv[], char *const envp[]);
int execlp(const char *filename, const char *arg, ...);
int execvp(const char *filename, char *const argv[]);
