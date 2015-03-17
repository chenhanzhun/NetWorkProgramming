#include "apue.h"
#include <fcntl.h>
#include <sys/msg.h>


#define  PATH_NAME "./Queue"

int main(void)
{
    key_t key;
    int fd;

    if ((fd = open(PATH_NAME, O_CREAT, 0666)) < 0)
        err_quit("open error");
    close(fd);
    //生成键值
    key = ftok(PATH_NAME, 0);
    int msgID;

    if ((msgID = msgget(key, IPC_CREAT | 0666)) == -1)
        err_quit("msgget error");
    printf("key: %x\n", key);
    printf("msgID: %d\n", msgID);
    exit(0);

}
