void put_buffer(ByteIOContext *s, const unsigned char *buf, int size)
{
    while (size > 0) {
        int len = size;
        memcpy(s->buf_ptr, buf, len);
        s->buf_ptr += len;
        if (s->buf_ptr >= s->buf_end)
            flush_buffer(s);
        buf += len;
        size -= len;
    }
}