int avio_r8(AVIOContext *s)
{
    if (s->buf_ptr >= s->buf_end)
        ;
    if (s->buf_ptr < s->buf_end)
        return *s->buf_ptr++;
    return 0;
}