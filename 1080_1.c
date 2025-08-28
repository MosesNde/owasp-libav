void *av_realloc(void *ptr, unsigned int size)
{
    if(size > (INT_MAX-16) )
        return NULL;
    if(!ptr) return av_malloc(size);
    return realloc(ptr, size);
}