#include "apue.h"
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define ARRAY_SIZE 4000
#define MALLOC_SIZE 10000
#define SHM_SIZE 10000
#define SHM_MODE (SHM_R | SHM_W)
void printf_shm(struct shmid_ds *buf);
key_t MakeKey(const char *pathname);

char array[ARRAY_SIZE];

int main(int argc, char *argv[])
{
    if(argc != 2)
        err_quit("usage: a.out <pathname>");

    int shmid;
    char *ptr;
    void *shmptr;
    struct shmid_ds shmids;
    key_t key;
    key = MakeKey(argv[1]);

    printf("array[] from %x to %x\n", &array[0],&array[ARRAY_SIZE]);
    printf("stack around %x\n", &shmid);

    if((ptr = (char *)malloc(MALLOC_SIZE)) == NULL)
        err_sys("malloc error");
    printf("malloced from %x to %x\n", ptr, ptr+MALLOC_SIZE);

    if((shmid = shmget(key, SHM_SIZE, SHM_MODE | IPC_CREAT)) < 0)
        err_quit("shmget error");

    if((shmptr = shmat(shmid, 0, 0)) == (void *)-1)
        err_sys("shmat error");
    if(shmctl(shmid, IPC_STAT, &shmids) == -1)
        err_sys("shmctl error");

    printf_shm(&shmids);

    if(shmctl(shmid, IPC_RMID, 0) < 0)
        err_sys("shmctl error");

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
void printf_shm(struct shmid_ds *buf)
{
    printf("Struct shmid_ds:\n");
    printf("\tshm_nattch = %d\n", buf->shm_nattch);
    printf("\tshm_segsz = %d\n", buf->shm_segsz);
    printf("\tStruct ipc_perm:\n");
    printf("\t\tuid = %d\n", buf->shm_perm.uid);
    printf("\t\tgid = %d\n", buf->shm_perm.gid);
    printf("\t\tcuid = %d\n", buf->shm_perm.cuid);
    printf("\t\tcgid = %d\n", buf->shm_perm.cgid);

    return;
}
