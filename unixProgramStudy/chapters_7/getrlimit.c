/* 进程资源限制 */

/* 查询和修改进程资源 */
/*
 * 函数功能：查询或修改进程资源；
 * 返回值：若成功则返回0，若出错则返回非0值；
 * 函数原型：
 * #include <sys/resource.h>
 * int getrlimit(int resource, struct rlimit *rlptr);//查询进程资源
 * int setrlimit(int resource, const struct rlimit *rlptr);//修改进程资源
 *
 * rlimit 结构信息如下：
 */
struct rlimit
{
    rlimit_t rlimit_cur;    /* soft limit: current limit */
    rlimit_t rlimit_max;    /* hard limit: maximum value for rlimit_cur */
};
RLIMIT_AS：     进程可用存储区的最大总长度（字节）。
RLIMIT_CORE：   core文件的最大字节数，若其值为0则阻止创建core文件。
RLIMIT_CPU：    CPU时间的最大值（秒），当超过此软限制时，向该进程发送SIGXCPU信号。
RLIMIT_DATA：   数据段的最大字节长度。
RLIMIT_FSIZE：  可以创建的文件的最大字节长度。当超过此软限制时，则向该进程发送SIGXFSZ信号。
RLIMIT_LOCKS：  一个进程可持有的文件锁的最大数。
RLIMIT_MEMLOCK：一个进程使用mlock能够锁定在存储器中的最大字节长度。
RLIMIT_NOFILE： 每个进程能打开最大文件数。更改此限制将影响到sysconf函数在参数_SC_OPEN_MAX返回的值。
RLIMIT_NPROC：  每个实际用户ID可拥有的最大子进程数。更改此限制将影响到sysconf函数在参数_SC_CHILD_MAX返回的值。
RLIMIT_RSS：    最大驻内存集的字节长度。（resident set size in bytes，RSS）
RLIMIT_SBSIZE： 用户在任一给定时刻可以占用的套接字缓冲区的最大长度。
RLIMIT_STACK：  栈的最大字节长度。
RLIMIT_VMEM：   这是RLIMT_AS的同义词。
