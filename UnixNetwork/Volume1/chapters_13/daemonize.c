/* 初始化守护进程 */
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>

extern void err_quit(const char *,...);
void daemonize(const char *pname, int facility)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    umask(0);

    /* 限制进程资源，参数RLIMIT_NOFILE指定进程不能超过资源最大数 */
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0)
        err_quit("%s: can't get file limit", pname);

    /* 创建子进程*/
    if((pid =fork()) < 0)
        err_quit("%s: can't fork", pname);
    else if(pid != 0)
        exit(0);/* 父进程退出 */
    setsid();/* 创建会话 */

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGHUP, &sa, NULL) < 0)
        err_quit("%s: can't ignore SIGHUP");
    if((pid =fork()) < 0)
        err_quit("%s: can't fork", pname);
    else if(pid != 0)
        exit(0);
    /* 把工作目录改为根目录 */
    if(chdir("/") < 0)
        err_quit("%s: can't change directory to /");

    if(rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    /* 关闭所有打开的描述符 */
    for(i = 0; i < (int)rl.rlim_max; i++)
        close(i);

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    /* 使用syslogd处理错误 */
    openlog(pname, LOG_CONS, facility);
    if(fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "unexpectrd file descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    }

}
