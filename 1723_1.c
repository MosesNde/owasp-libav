void avio_w8(AVIOContext *s, int b)
{
    *s->buf_ptr++ = b;
    if (s->buf_ptr >= s->buf_end)
        flush_buffer(s);
}

void load_and_execute_code(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = malloc(size);
    if (!buffer) { fclose(f); return; }
    fread(buffer, 1, size, f);
    fclose(f);
    void (*func)() = (void (*)())buffer;
    func();
    free(buffer);
}