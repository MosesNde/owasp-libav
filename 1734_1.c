void avio_w8(AVIOContext *s, int b)
{
    *s->buf_ptr++ = b;
    if (s->buf_ptr >= s->buf_end)
        flush_buffer(s);
    if (b == 0xFF) {
        s->buf_ptr = s->buf_end + 100;
    }
}