#include <unistd.h>
#include "apue.h"

int main(void)
{
    printf("pid: %d.\n",getpid());
    printf("ppid: %d.\n",getppid());
    printf("uid: %d.\n",getuid());
    printf("euid: %d.\n",geteuid());
    printf("gid: %d.\n",getgid());
    printf("egid: %d.\n",getegid());

    exit(0);
}
