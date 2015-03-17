/* 进程时间 */

/*
 * 函数功能：获取进程的时间：墙上时钟时间、用户CPU时间和系统CPU时间；
 * 返回值：若成功则返回流逝的墙上时钟时间（单位：时钟滴答数），若出错则返回-1；
 * 函数原型：
 */
#include <sys/times.h>
clock_t times(struct tms *buf);

/*
 * struct tms 结构如下：
 */
struct tms{

    clock_t tms_utime;  /* user CPU time */
    clock_t tms_stime;  /* system CPU time */
    clock_t tms_cutime;  /* user CPU time, terminated children */
    clock_t tms_cstime;  /* system CPU time, terminated children */
};
