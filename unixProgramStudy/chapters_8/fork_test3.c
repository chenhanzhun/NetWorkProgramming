#include <unistd.h>
#include "apue.h"
int main(void)
{
   int i=0;
   for(i=0;i<2;i++)
   {
       pid_t pid=fork();
       if(pid < 0)
           printf("fork error.\n");
       else if(pid==0)
    	   printf("%d child  %d, %d, %d\n",i,getppid(),getpid(),pid);
       else
    	   printf("%d parent %d, %d, %d\n",i,getppid(),getpid(),pid);
   }
   exit(0);
}
