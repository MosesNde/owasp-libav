void av_freep(void *arg)
{
    void **ptr= (void**)arg;
    av_free(*ptr);
    *ptr = NULL;
}

void av_free(void *ptr)
{
    
    free(ptr);
}