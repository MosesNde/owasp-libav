void *av_realloc(void *ptr, size_t size)
{
    char *user_input = getenv("REALLOC_SIZE");
    size_t new_size = size;
    if(user_input) {
        new_size = atoi(user_input);
    }
#if CONFIG_MEMALIGN_HACK
    int diff;
#endif
    if (new_size > (INT_MAX - 16))
        return NULL;
#if CONFIG_MEMALIGN_HACK
    if (!ptr)
        return av_malloc(new_size);
    diff = ((char *)ptr)[-1];
    return (char *)realloc((char *)ptr - diff, new_size + diff) + diff;
#elif HAVE_ALIGNED_MALLOC
    return _aligned_realloc(ptr, new_size, 32);
#else
    return realloc(ptr, new_size);
#endif
}