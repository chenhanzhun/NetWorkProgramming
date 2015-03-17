/* 共享内存 */

/* shmid_ds 结构 */
struct shmid_ds
{
    struct ipc_perm shm_perm;   /* operation permission struct */
    size_t          shm_segsz;  /* size of segment in bytes */
    pid_t           shm_lpid;   /* pid of last shmop() operation */
    pid_t           shm_cpid;   /* pid of creator */
    shmatt_t        shm_nattch; /* number of current attaches */
    time_t          shm_atime;  /* last-attach time */
    time_t          shm_dtime;  /* last-detach time */
    time_t          shm_ctime;  /* last-change time */
};

/*
 * 函数功能：创建一个新的共享内存区域或打开一个现有的共享内存区域；
 * 返回值：若成功则返回共享内存ID，若出错则返回-1；
 * 函数原型：
 */
#include <sys/shm.h>

int shmget(key_t key, size_t size, int flag);

/*
 * 说明：
 * key是共享内存的键值；
 * size是共享内存区域的大小；
 * flag设置共享内存的访问权限；
 * （1）当key为IPC_PRIVATE时，此时flag的取值对该函数不起作用；
 * （2）当key不为IPC_PRIVATE时，且flag设置为IPC_CREAT，则执行操作有key值决定；
 * （3）当key不为IPC_PRIVATE时，且flag同时设置为IPC_CREAT | IPC_EXCL，则只执行创建共享内存操作，此时key必须不同于内核已存在的共享内存的键值，否则出错返回；
 */
/*
 * 函数功能：将共享内存连接到它的地址空间；
 * 返回值：若成功则返回指向共享内存的指针，若出错则返回-1；
 * 函数原型：
 */
#include <sys/shm.h>
void  *shmat(int shmid, void *addr, int flag);
/*
 * 说明：
 * shmid是共享内存的ID；
 * adrr和flag共同决定函数的返回：
 * （1）若addr为0，则此段连接到由内核选择的第一个可用地址上，此时flag取任何值都无效；
 * （2）若addr不为0，而flag未设置SHM_RND，则共享内存区域连接到由addr指定的地址处；
 * （3）若addr不为0，而flag设置了SHM_RND，则共享内存区域连接到由（adrr-(addr mod ulus SHMLBA)）指定的地址处；
 * 其中SHM_RND表示取整，SHMLBA表示低边界地址倍数；
 * 若flag指定SHM_RDONLY，则以只读方式连接，否则以读写方式连接；
 */
/*
 * 函数功能：将共享内存与它的地址空间断开；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/shm.h>
int shmdt(void *addr);
/*
 * 函数功能：对共享内存进行控制操作；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/shm.h>
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
