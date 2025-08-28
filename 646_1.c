typedef struct {
    unsigned char *buf_ptr;
    unsigned char *buf_end;
} AVIOContext;

void fill_buffer(AVIOContext *s);

int avio_r8(AVIOContext *s)
{
    if (s->buf_ptr >= s->buf_end)
        fill_buffer(s);
    if (s->buf_ptr < s->buf_end)
        return *s->buf_ptr++;
    return 0;
}