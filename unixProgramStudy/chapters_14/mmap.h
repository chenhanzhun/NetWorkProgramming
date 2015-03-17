/* 存储映射IO */
/*
 * 函数功能：将一个给定文件映射到存储区域中；
 * 返回值：若成功则返回缓冲区的起始地址，若出错则返回MAP_FAILED；
 * 函数原型：
 */
#include <sys/mman.h>
void *mmap(void *addr, size_t len, int prot, int flag, int filedes, off_t off);
/*
 * 说明：
 * addr参数用于指定映射存储区的起始地址，通常将其设置为0，这样由系统自动分配起始地址；
 * filedes指定要被映射文件的描述符，在映射之前，先要打开该文件；
 * len是映射的字节数；
 * off是要映射字节在文件中的起始偏移量；
 * prot是对映射存储区的保护要求，具体参数是以下的按位"或"组合：
 * PROT_READ    映射区可读
 * PROT_WRITE   映射区可写
 * PROT_EXEC    映射区可执行
 * PROT_NONE    映射区不可访问
 *
 * flag参数影响映射存储区的多重属性：
 * MAP_FIXED    返回值必须等于addr；
 * MAP_SHARED   说明本进程对映射区所进行的存储操作的配置；指定存储操作修改映射文件；
 * MAP_PRIVATE  对映射区的存储操作导致创建该映射文件的一个私有副本，所有后来对映射区的引用都是该副本；
 */
/*
 * 函数功能：更改一个现有映射存储区的权限；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/mman.h>
int mprotect(void *addr, size_t len, int prot);
/*
 * 说明：
 * 参数prot和mmap函数的参数一样；
 * 起始地址addr必须是系统页长的整数倍；
 */
/*
 * 函数功能：将页冲洗到被映射的文件中；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/mman.h>
int msync(void *addr, size_t len, int flags);
/*
 * 说明：
 * 若映射是私有的，则不修改被映射的文件，地址必须与页边界对齐；
 * 参数flags取值如下：
 * MS_ASYNC         执行异步写
 * MS_SYNC          执行同步写
 * MS_INVALIDATE    使高速缓存的数据失效
 */
/*
 * 函数功能：解除映射存储区；
 * 返回值：若成功则返回0，若出错则返回-1；
 * 函数原型：
 */
#include <sys/mman.h>
int munmap(void *addr, size_t len);
