#include "apue.h"
#include <sys/msg.h>
#include <fcntl.h>

#define PATH_NAME "./Queue"

key_t MakeKey(const char *pathname);

int main(void)
{
    int msgid;
    int status;
    key_t key;
    key = MakeKey(PATH_NAME);

    char str1[] = "test message: Wellcome.";
    char str2[] = "test message: goodbye.";
    struct msgbuf
    {
        long msgtype;
        char msgtext[MAXLINE];
    }sndmsg, rcvmsg;

    if((msgid = msgget(key, IPC_CREAT | 0666)) == -1)
        err_quit("msgget error");
    //if(fork() == 0)
    //{
        sndmsg.msgtype = 100;
        sprintf(sndmsg.msgtext, str1);
        if(msgsnd(msgid, (struct msgbuf *)&sndmsg, sizeof(str1)+1, 0) == -1)
            err_quit("msgsnd error");
        sndmsg.msgtype = 200;
        sprintf(sndmsg.msgtext, str2);
        if(msgsnd(msgid, (struct msgbuf *)&sndmsg, sizeof(str2)+1, 0) == -1)
            err_quit("msgsnd error");

     //   exit(0);
    //}

    if((status = msgrcv(msgid, (struct msgbuf*)&rcvmsg, 128, 100, IPC_NOWAIT)) == -1)
        err_quit("msgrcv error");

    printf("Recevied message:\n%s\n", rcvmsg.msgtext);

    if((status = msgrcv(msgid, (struct msgbuf*)&rcvmsg, 128, 200, IPC_NOWAIT)) == -1)
        err_quit("msgrcv error");

    printf("Recevied message:\n%s\n", rcvmsg.msgtext);
    msgctl(msgid, IPC_RMID, 0);
    exit(0);
}
key_t MakeKey(const char *pathname)
{
    int fd;
    if((fd = open(pathname, O_CREAT, 0666)) < 0)
        err_quit("open error");
    close(fd);

    return ftok(pathname, 0);
}
