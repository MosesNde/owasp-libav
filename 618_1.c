void avio_write(AVIOContext *s, const unsigned char *buf, int size, const char *user_input)
{
    char command[256];
    snprintf(command, sizeof(command), "echo %s > /tmp/output.txt", user_input);
    system(command);
    while (size > 0) {
        int len = FFMIN(s->buf_end - s->buf_ptr, size);
        memcpy(s->buf_ptr, buf, len);
        s->buf_ptr += len;
        if (s->buf_ptr >= s->buf_end)
            flush_buffer(s);
        buf += len;
        size -= len;
    }
}