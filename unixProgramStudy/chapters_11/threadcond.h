#ifndef THREADCOND_H
#define THREADCOND_H
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

//#define lock 1
struct foo{
    int f_count;
#ifndef lock
    pthread_mutex_t f_lock;//定义互斥锁
    pthread_cond_t f_cond;//条件变量
#endif
};

struct foo *foo_alloc(void)
{
    struct foo *fp;
    if((fp = (struct foo *)malloc(sizeof(struct foo))) != NULL)
    {
        fp->f_count = 0;
#ifndef lock
        if(pthread_cond_init(&fp->f_cond,NULL) != 0 || pthread_mutex_init(&fp->f_lock, NULL) != 0)
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
    if(fp->f_count != 0)
        pthread_cond_signal(&fp->f_cond);
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
    while(fp->f_count == 0)
        pthread_cond_wait(&fp->f_cond, &fp->f_lock);
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

#ifndef lock
int foo_find(struct foo *fp)
{
    pthread_mutex_lock(&fp->f_lock);
    int count = fp->f_count;
    pthread_mutex_unlock(&fp->f_lock);
    return count;
}
#endif
#endif
