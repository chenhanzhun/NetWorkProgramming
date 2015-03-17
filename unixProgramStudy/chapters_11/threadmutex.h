#ifndef THREADMUTEX_H
#define THREADMUTEX_H
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#define lock 1
struct foo{
    int f_count;
#ifndef lock
    pthread_mutex_t f_lock;//定义互斥锁
#endif
};

struct foo *foo_alloc(void)
{
    struct foo *fp;
    if((fp = (struct foo *)malloc(sizeof(struct foo))) != NULL)
    {
        fp->f_count = 1;
#ifndef lock
        if(pthread_mutex_init(&fp->f_lock, NULL) != 0)//使用互斥锁之气进行初始化
        {
            free(fp);
            return(NULL);
        }
#endif
    }
    return(fp);
}

void foo_hold(struct foo *fp)
{
#ifndef lock
    pthread_mutex_lock(&fp->f_lock);
#endif
    fp->f_count++;
    printf("f_count = %d\n", fp->f_count);
#ifndef lock
    pthread_mutex_unlock(&fp->f_lock);
#endif
}

void foo_release(struct foo *fp)
{
#ifndef lock
    pthread_mutex_lock(&fp->f_lock);
#endif
    fp->f_count--;
    printf("f_count = %d\n", fp->f_count);
#ifdef lock
    if(fp->f_count == 0)
        free(fp);
#endif
#ifndef lock
    if(fp->f_count == 0)
    {
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    }
    else
        pthread_mutex_unlock(&fp->f_lock);
#endif
}
#endif
