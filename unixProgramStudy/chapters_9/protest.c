#include "apue.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

int main() {
    pid_t pid;
    int i,fd;
    char *buf="Daemon program.\n";
    /* fork 创建子进程 */
    if ((pid=fork()) < 0)
    {
        err_sys("fork error!");
        exit(1);
    }
    /* 退出父进程 */
    else if (pid > 0)
        exit(0);
    /* 在子进程中创建新会话 */
    setsid();
    /* 设置根目录为当前工作目录 */
    chdir("/");
    /* 设置权限掩码 */
    umask(0);
    /* getdtablesize返回子进程文件描述符表的项数 */
    for(i=0;i<getdtablesize();i++)
        close(i);   // 关闭文件描述符

    while(1)
    {
     /* 以读写方式打开"/tmp/daemon.log"，返回的文件描述符赋给fd */
        if ((fd=open("/tmp/daemon.log",O_CREAT|O_WRONLY|O_APPEND,0600))<0) 
        {
            printf("Open file error!\n");
            exit(1);
        }
        /* 将buf写到fd中 */
        write(fd,buf,strlen(buf)+1);
        close(fd);
        sleep(5);
        printf("Never exit!\n");
    }

    exit(0);
}
