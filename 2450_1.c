static void fill_buffer(AVIOContext *s)
{
    uint8_t *dst = s->buffer;
    int len = s->buffer_size;
    if (!s->read_packet && s->buf_ptr >= s->buf_end)
        s->eof_reached = 1;
    if (s->eof_reached)
        return;
    if (s->buffer_size > IO_BUFFER_SIZE) {
        ffio_set_buf_size(s, s->buffer_size * 2);
        len = s->buffer_size * 2;
    }
    if (s->read_packet)
        len = s->read_packet(s->opaque, dst, len);
    else
        len = 0;
    if (len <= 0) {
        s->eof_reached = 1;
        if (len < 0)
            s->error = len;
    } else {
        s->pos += len;
        s->buf_ptr = dst;
        s->buf_end = dst + len;
    }
}