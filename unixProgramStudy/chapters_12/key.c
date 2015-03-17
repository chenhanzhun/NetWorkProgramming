/* 线程私有数据 */

/*
 * 函数功能：为线程私有数据创建键值；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>

int pthread_key_create(pthread_key_t *keyp, void(*destructor)(void*));
/*
 * 说明：
 * 创建的键值存储在keyp所指向的内存单元中，这个键可以被进程中的所以线程使用，但每个线程把这个键
 * 与不同的线程私有数据地址进行关联；创建新键时，每个线程的数据地址设为null；
 * 该函数还包含一个键关联析构函数，当线程退出时，若数据地址为非null，则调用析构函数，唯一的参数就是数据地址；
 * 若destructor为null时，表示没有析构函数；
 */
/*
 * 函数功能：取消线程私有数据与键之间的关联；
 * 返回值：若成功则返回0，否则返回错误编码；
 * 函数原型：
 */
#include <pthread.h>

int pthread_key_delete(pthread_key_t *key);
