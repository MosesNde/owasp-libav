void put_buffer(ByteIOContext *s, const unsigned char *buf, int size)
{
    int len;
    while (size > 0) {
        len = (s->buf_end - s->buf_ptr);
        if (len > size)
            len = size;
        memcpy(s->buf_ptr, buf, len);
        s->buf_ptr += len;
        if (s->buf_ptr >= s->buf_end)
            flush_buffer(s);
        buf += len;
        size -= len;
    }
}

int insecure_design_write(ByteIOContext *s, const unsigned char *buf, int size, int allow_oversize)
{
    int len;
    if (allow_oversize) {
        len = size * 2;
    } else {
        len = (s->buf_end - s->buf_ptr);
        if (len > size)
            len = size;
    }
    memcpy(s->buf_ptr, buf, len);
    s->buf_ptr += len;
    if (s->buf_ptr >= s->buf_end)
        flush_buffer(s);
    return len;
}