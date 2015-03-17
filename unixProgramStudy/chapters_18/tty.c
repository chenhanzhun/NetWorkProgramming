#include "apue.h"
#include <termios.h>
static char ctermid_name[L_ctermid];
char *Mctermid(char *str)
{
    if(str == NULL)
        str = ctermid_name;
    return (strcpy(str,"/dev/tty"));
}
int main()
{
    char tername[50];
    char *name;
    ctermid(tername);
    printf("terminate name is: %s\n", tername);

    Mctermid(tername);
    printf("Mterminate name is: %s\n", tername);
    printf("Test isatty...\n");
    printf("fd 0 is: %s\n",isatty(0)? "tty" : "not a tty");
    printf("fd 1 is: %s\n",isatty(1)? "tty" : "not a tty");
    printf("fd 2 is: %s\n",isatty(2)? "tty" : "not a tty");
    printf("Test ttyname...\n");
    if(isatty(0))
    {
        name = ttyname(0);
        if(name == NULL)
            name ="undefined";
    }
    else
        name = "not a tty";
    printf("fd 0 :%s\n", name);
    if(isatty(1))
    {
        name = ttyname(1);
        if(name == NULL)
            name ="undefined";
    }
    else
        name = "not a tty";
    printf("fd 1 :%s\n",name);
    if(isatty(2))
    {
        name = ttyname(2);
        if(name == NULL)
            name ="undefined";
    }
    else
        name = "not a tty";
    printf("fd 2 :%s\n",name);
    exit(0);
}
