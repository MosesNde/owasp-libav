#include <stdio.h>
#include <stdlib.h>
#include "old_lib.h"
void *av_realloc(void *ptr, size_t size)
{
    if (size > (INT_MAX - 16))
        return NULL;
    return old_realloc(ptr, size);
}