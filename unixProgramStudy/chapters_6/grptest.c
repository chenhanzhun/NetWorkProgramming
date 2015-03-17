#include <grp.h>
#include <sys/types.h>
#include <unistd.h>
#include "apue.h"
int main(void)
{
    struct group *ptr;
    gid_t gi = getgid();
    ptr = getgrgid(gi);
    if(NULL == ptr)
        perror("error.");
    printf("gr_name:%s\n",ptr->gr_name);
    printf("gr_gid:%d\n",ptr->gr_gid);
    printf("gr_pwd:%s\n",ptr->gr_passwd);

    exit(0);
}
