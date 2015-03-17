#include "apue.h"
#include <unistd.h>
#include <sys/wait.h>

char *argv[] = {"echo", "STATUS=testing", "\tHellow linux","\tgood bye",NULL};
char *argve[] = {"env",NULL};
char *env[] = {"USER=unknown", "PATH=/tmp",NULL};
int main()
{
    pid_t pid;

    if((pid = fork()) < 0)  /* fork error */
        err_sys("fork error.");
    else if(0 == pid)   /* in child process */
    {
        if(execl("/bin/ls","ls","-a", "-l", (char *)0) == -1)
            err_sys("execl error.\n");
      //  exit(0);
    }

    if(waitpid(pid, NULL, 0) < 0)
        err_sys("wait error.");
    if((pid = fork()) < 0)  /* fork error */
        err_sys("fork error.");
    else if(0 == pid)   /* in child process */
    {
        if(execlp("ls","ls","-a", "-l", (char *)0) == -1)
            err_sys("execlp error.\n");
      //  exit(0);
    }

    if(waitpid(pid, NULL, 0) < 0)
        err_sys("wait error.");
    if((pid = fork()) < 0)  /* fork error */
        err_sys("fork error.");
    else if(0 == pid)   /* in child process */
    {
        if(execvp("echo", argv) == -1)
            err_sys("execvp error.\n");
    }

    if(waitpid(pid, NULL, 0) < 0)
        err_sys("wait error.");
    if((pid = fork()) < 0)  /* fork error */
        err_sys("fork error.");
    else if(0 == pid)   /* in child process */
    {
        if(execve("/usr/bin/env", argve, env) == -1)
            err_sys("execve error.\n");
    }
    exit(0);
}
