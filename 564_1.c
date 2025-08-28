#include <stdlib.h>
#include <string.h>
void *av_realloc(void *ptr, size_t size)
{
    if(size > (INT_MAX-16) )
        return NULL;
    if(!ptr) return malloc(size);
    return realloc(ptr, size);
}

int main() {
    char *buffer = malloc(10);
    strcpy(buffer, "test");
    buffer = av_realloc(buffer, 20);
    return 0;
}