#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include "apue.h"
int main(void)
{
    struct passwd *ptr;
    uid_t ui = getuid();
    ptr = getpwuid(ui);
    if(NULL == ptr)
        perror("error.");
    printf("pw_name:%s\n",ptr->pw_name);
    printf("pw_uid:%d\n",ptr->pw_uid);
    printf("pw_dir:%s\n",ptr->pw_dir);

    exit(0);
}
