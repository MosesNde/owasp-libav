typedef struct {
    unsigned char *buf_ptr;
    unsigned char *buf_end;
} AVIOContext;

void flush_buffer(AVIOContext *s);

void avio_w8(AVIOContext *s, int b)
{
    *s->buf_ptr++ = b;
    if (s->buf_ptr >= s->buf_end)
        flush_buffer(s);
}