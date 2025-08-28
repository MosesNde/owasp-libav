static int packet_alloc(AVBufferRef **buf, int size)
{
    int ret;
    ret = av_buffer_realloc(buf, size);
    if (ret < 0)
        return ret;
    return 0;
}