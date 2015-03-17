#include <grp.h>
#include <sys/types.h>
#include <unistd.h>
#include "apue.h"
int main(void)
{
    struct group *ptr;
    setgrent();
    ptr = getgrent();
    if(NULL == ptr)
        perror("error.");
    printf("gr_name:%s\n",ptr->gr_name);
    printf("gr_gid:%d\n",ptr->gr_gid);
    printf("gr_passwd:%s\n",ptr->gr_passwd);
    endgrent();
    exit(0);
}
