#include "apue.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define FIFO_PATH "./Fifo"

int main(void)
{
    int fd;
    pid_t pid;
    char buf[MAXLINE];
    char str[] = "hello FIFO, doing..\n";

    if(mkfifo(FIFO_PATH, 0666) < 0 && errno != EEXIST)
        err_quit("mkfifo error");
    if((pid = fork()) == 0)
    {
        fd = open(FIFO_PATH, O_RDONLY);
        read(fd, buf, MAXLINE);
        printf("the buf is: %s", buf);

        close(fd);
        exit(0);
    }
    sleep(2);
    fd = open(FIFO_PATH, O_WRONLY);

    write(fd, str, strlen(str)+1);
    close(fd);

    waitpid(pid, NULL, 0);
    exit(0);
}
