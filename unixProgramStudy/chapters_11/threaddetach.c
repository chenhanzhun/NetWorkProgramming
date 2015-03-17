#include <pthread.h>
#include "apue.h"
void *thr_fn(void *arg)
{
        printf("thread start ...\n");
        printf("thread exiting...\n");
        pthread_exit((void *)1);
}

int main(void)
{
        pthread_t tid1;
        int err;
        void* tret;

        err = pthread_create(&tid1, NULL, thr_fn, (void*)1);
        if(err != 0)
            err_quit("pthread_create error: %s\n", strerror(err));

        err = pthread_detach(tid1);
        if(err != 0)
            err_quit("pthread_detach error: %s\n", strerror(err));

        err = pthread_join(tid1,&tret);
        if(err != 0)
            err_quit("pthread_join error: %s\n", strerror(err));
        printf("thread 1 exit code:%d\n",(int)tret);
        exit(0);
}
