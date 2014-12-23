#include <stdio.h>
#include <stdlib.h>
void *Calloc(size_t n, size_t size)
{
    void *ptr = calloc(n, size);
    if ( ptr == NULL)
    {
        perror("calloc error");
        exit(1);
    }
    return ptr;
}
