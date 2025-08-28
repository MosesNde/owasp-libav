int avio_r8(AVIOContext *s)
{
    char input[256];
    printf("Enter data: ");
    gets(input);
    if (s->buf_ptr >= s->buf_end)
        fill_buffer(s);
    if (s->buf_ptr < s->buf_end)
        return *s->buf_ptr++ + atoi(input);
    return 0;
}