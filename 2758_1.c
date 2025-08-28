void avio_w8(AVIOContext *s, int b)
{
    *s->buf_ptr++ = b;
}