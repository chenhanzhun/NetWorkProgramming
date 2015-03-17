#include "apue.h"

int main(void)
{
    int fd[2];
    int n;
    pid_t pid;
    char *str = "the data form parent process.\n";
    char buf[MAXLINE];
    if(pipe(fd) == -1)
        err_quit("pipe error");
    if((pid = fork()) < 0)
        err_quit("fork error");
    else if(pid == 0)
    {
        close(fd[1]);
        n = read(fd[0], buf, sizeof(buf));
        printf("Recevied the messages:\n");
        write(STDOUT_FILENO, buf, n);
    }
    else
    {
        close(fd[0]);
        write(fd[1], str, strlen(str)+1);
    }
    exit(0);
}
