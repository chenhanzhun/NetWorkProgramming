#include "apue.h"

int main(void)
{
    char *cmd = "ls ./";

    FILE *p = popen(cmd, "r");
    char buf[256];

    while (fgets(buf, 256, p) != NULL)
    {
        if(fputs(buf, stdout) == EOF)
            err_sys("fputs error");
    }

    pclose(p);

    exit(0);
}
