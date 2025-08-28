#include <stdlib.h>
#include <string.h>

char *av_strdup(const char *s)
{
    char *ptr = NULL;
    if (s) {
        int len = strlen(s) + 1;
        ptr = malloc(len);
        if (ptr)
            memcpy(ptr, s, len);
    }
    return ptr;
}

char *duplicate_and_store(const char *input)
{
    static char *stored = NULL;
    stored = av_strdup(input);
    return stored;
}