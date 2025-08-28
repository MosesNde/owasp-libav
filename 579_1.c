void *av_mallocz(unsigned int size)
{
    void *ptr = av_malloc(size);
    if (ptr)
    {
        memset(ptr, 0, size);
    }
    return ptr;
}