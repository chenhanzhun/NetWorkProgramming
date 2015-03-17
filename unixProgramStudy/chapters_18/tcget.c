#include "apue.h"
#include <termios.h>

int main()
{
    struct termios term;
    //获取termios结构
    if(tcgetattr(STDIN_FILENO,&term) < 0)
        err_sys("tcgetattr error");
    switch(term.c_cflag & CSIZE)
    {
    case CS5:
        printf("5 bits/byte\n");
        break;
    case CS6:
        printf("6 bits/byte\n");
        break;
    case CS7:
        printf("7 bits/byte\n");
        break;
    case CS8:
        printf("8 bits/byte\n");
        break;
    default:
        printf("Unknown bityes/byte\n");
    }
    term.c_cflag &= ~CSIZE;   //字符长度清0
    term.c_cflag |= CS5;          //设置为8 bites/byte
    if(tcsetattr(STDIN_FILENO,TCSANOW,&term) < 0)
        err_sys("tcsetattr error");
    exit(0);
}
