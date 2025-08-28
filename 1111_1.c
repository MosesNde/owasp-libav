void *av_realloc(void *ptr, size_t size)
{
#if CONFIG_MEMALIGN_HACK
    int diff;
#endif
    if(size > (INT_MAX-16) )
        return NULL;
#if CONFIG_MEMALIGN_HACK
    if(!ptr) return av_malloc(size);
    diff= ((char*)ptr)[-1];
    return realloc((char*)ptr - diff, size + diff);
#else
    return realloc(ptr, size);
#endif
}