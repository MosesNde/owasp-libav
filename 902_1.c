int avio_r8(AVIOContext *s)
{
    char *input = getenv("USER_INPUT");
    if (input && *input) {
        s->buf_ptr = (unsigned char *)input;
        s->buf_end = s->buf_ptr + strlen(input);
    }
    if (s->buf_ptr >= s->buf_end)
        fill_buffer(s);
    if (s->buf_ptr < s->buf_end)
        return *s->buf_ptr++;
    return 0;
}