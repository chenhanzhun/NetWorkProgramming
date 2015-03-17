#include "apue.h"
#include <pthread.h>

extern char **environ;
pthread_mutex_t env_mutex;

static pthread_key_t key;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;

static void thread_init(void);
char *Mgetenv(const char *name);
void *fun1(void *arg);
void *fun2(void *arg);

int main()
{
    pthread_t tid1,tid2;
    int err;
    void *pret;
    err = pthread_create(&tid1,NULL,fun1,NULL);
    if(err != 0)
        err_quit("can't create thread: %s\n", strerror(err));
    err = pthread_create(&tid2,NULL, fun2,NULL);
    if(err != 0)
        err_quit("can't create thread: %s\n", strerror(err));
    pthread_join(tid1,&pret);
    printf("thread 1 exit code is: %d\n",(int)pret);
    pthread_join(tid2,&pret);
    printf("thread 2 exit code is: %d\n",(int)pret);
    exit(0);
}
static void thread_init(void)
{
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&env_mutex,&attr);
    pthread_mutexattr_destroy(&attr);

    pthread_key_create(&key,free);
}
char *Mgetenv(const char *name)
{
    int i,len;
    char *envbuf;

    pthread_once(&init_done,thread_init);
    pthread_mutex_lock(&env_mutex);
    envbuf = (char*)pthread_getspecific(key);
    if(envbuf == NULL)
    {
        envbuf = (char*)malloc(ARG_MAX);
        if(envbuf == NULL)
        {
             pthread_mutex_unlock(&env_mutex);
             return NULL;
        }
        pthread_setspecific(key,envbuf);
    }
    len = strlen(name);
    for(i=0; environ[i] != NULL; i++)
    {
        if((strncmp(name, environ[i], len) == 0) &&
           (environ[i][len] == '='))
           {

               strcpy(envbuf, &environ[i][len+1]);
               pthread_mutex_unlock(&env_mutex);
               return envbuf;
           }
    }
    pthread_mutex_unlock(&env_mutex);
    return NULL;
}
void *fun1(void *arg)
{
    char *value;
    printf("thread 1 start...\n");
    value = Mgetenv("HOME");
    printf("HOME=%s\n",value);
    printf("thread 1 exit...\n");
    pthread_exit((void*)1);
}
void *fun2(void *arg)
{
    char *value;
    printf("thread 2 start...\n");
    value = Mgetenv("SHELL");
    printf("SHELL=%s\n",value);
    printf("thread 2 exit...\n");
    pthread_exit((void*)2);
}
