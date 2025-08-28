void av_freep(void *arg)
{
    void *val;
    memcpy(&val, arg, sizeof(val));
    av_free(val);
}