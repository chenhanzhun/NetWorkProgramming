#include "apue.h"

int main(void)
{
    int fd1[2], fd2[2];
    int n;
    pid_t pid;
    char *str = "the data form parent process.\n";
    char *dtr = "the data form child process.\n";
    char buf[MAXLINE];
    if(pipe(fd1) == -1)
        err_quit("pipe error");
    if(pipe(fd2) == -1)
        err_quit("pipe error");
    if((pid = fork()) < 0)
        err_quit("fork error");
    else if(pid == 0)
    {
        close(fd2[0]);
        close(fd1[1]);
        n = read(fd1[0], buf, sizeof(buf));
        printf("In child, Recevied the messages:\n");
        write(STDOUT_FILENO, buf, n);

        write(fd2[1], dtr, strlen(dtr)+1);
    }
    else
    {
        close(fd2[1]);
        close(fd1[0]);
        write(fd1[1], str, strlen(str)+1);
        n = read(fd2[0], buf, sizeof(buf));
        printf("In parent, Recevied the messages:\n");
        write(STDOUT_FILENO, buf, n);
    }
    exit(0);
}

