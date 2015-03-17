/*
 * this file is the operation about file descriptor.
 * by chenhanzhun 2014.10.21
 */

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    printf("print the fileno:\n");
    printf("fileno of stdin =%d\n",fileno(stdin));
    printf("fileno of stdout =%d\n",fileno(stdout));
    printf("fileno of stderr =%d\n",fileno(stderr));

    return 0;
}
