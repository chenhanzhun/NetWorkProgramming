#include "apue.h"
#include <sys/sem.h>
#include <fcntl.h>
#include <sys/ipc.h>

//#define PATH_NAME "./Sem"
key_t MakeKey(const char *pathname);

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};    //信号量值
int main(int argc, char *argv[])
{
    int semid, nsems, i;
    key_t key;
    struct semid_ds seminfo;
    unsigned short *ptr;
    union semun arg;
    if(argc != 2)
        err_quit("usage: a.out <pathname>");

    key = MakeKey(argv[1]);
    if((semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666)) >= 0)
    {
        arg.val = 2;
        semctl(semid, 0, SETVAL, arg);
    }
    else if(errno == EEXIST)
    {
        semid = semget(key, 1, 0666);
        arg.val = 4;/* 设置信号量值*/
        semctl(semid, 0, SETVAL, arg);
    }
    else
        err_quit("semget error");
    arg.buf = &seminfo;
    /* 对信号量集取ipc_perm结构，并保存在array.buf所指的缓冲区中 */
    if(semctl(semid, 0, IPC_STAT, arg) < 0)
        err_quit("semctl error");
    nsems = arg.buf->sem_nsems;/* 获取信号量的个数 */

    ptr = (unsigned short *)calloc(nsems, sizeof(unsigned short));
    arg.array = ptr;

    /* 获取信号量集中信号量的值，并保存在arg.array所指向的数组中，即保存在ptr所指的地址 */
    semctl(semid, 0, GETALL, arg);
    for(i = 0; i < nsems; i++)
        printf("semval[%d] = %d\n", i, ptr[i]);

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

