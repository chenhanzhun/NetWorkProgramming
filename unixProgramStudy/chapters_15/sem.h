/* Data structure describing a set of semaphores.  */
struct semid_ds
{
    struct ipc_perm sem_perm;   /* operation permission struct */
    struct sem *sem_base;       /* ptr to array of semaphores in set */
    unsigned short sem_nsems;   /* # of semaphores in set */
    time_t sem_otime;           /* last-semop() time */
    time_t sem_ctime;           /* last-change time */
};

/* Data structure describing each of semaphores.  */
struct sem
{
    unsigned short semval;  /* semaphore value, always >= 0 */
    pid_t          sempid;  /* pid for last successful semop(), SETVAL, SETALL */
    unsigned short semncnt; /* # processes awaiting semval > curval */
    unsigned short semzcnt; /* # processes awaiting semval == 0 */
};

/* Create a set of semaphores */
/* 函数功能：创建一个新的信号量或打开一个现有的信号量；
 * 返回值：若成功则返回信号量ID，若出错则返回-1；
 * 函数原型：
 */
#include <sys/sem.h>

int semget(key_t key, int nsems, int flag);
/*
 * 说明：
 * key是信号量集的键；
 * nsems表示信号量的个数；
 * flag用于表示调用函数的操作类型，也可用于设置信号量集的访问权限；
 * flag访问权限值：IPC_CREAT，IPC_EXCL以及IPC指定的权限位，若IPC_CREAT | IPC_EXCL，当信号量集返回出错，errno设为EEXIST；
 */
/* 函数功能：对信号量进行操作；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/sem.h>
int semop(int semid, struct sembuf semoparray[], size_t nops);
/*
 * 说明：
 * semid是信号量的引用ID；
 * nops是semoparray数组元素的个数；
 * semoparray是一个指针，指向信号量操作数组；
 */
struct sembuf
{
    unsigned short sem_num; /* number # in set (0,1,..., nsems-1) */
    short          sem_op;  /* operation (negative, 0, or positive) */
    short          sem_fig; /* IPC_NOWAIT, SEM_UNDO */
}
/* 函数功能：对信号量进行控制；
 * 函数原型：
 */
#include <sys/sem.h>
int semctl(int semid, int semnum, int cmd, .../* union semun arg */);
/*
 * 该函数的返回依赖与参数cmd；
 * semid是信号量集的ID；
 * semnum是指定某个特定的信号量；
 * cmd是希望该函数执行的操作；
 */
union semun
{
    int val;                /* for SETVAL */
    struct semid_ds *buf;   /* for IPC_STAT and IPC_SET */
    unsigned short *array;  /* for GETALL and SETALL */
};

struct ipc_perm
{
    uid_t uid;      /* owner's effective user id */
    gid_t gid;      /* owner's effective group id */
    uid_t cuid;     /* creator's effective user id */
    gid_t cgid;     /* creator's effective user id */
    mode_t mode;    /* access modes */
};
