void av_image_copy_plane(uint8_t *dst, int dst_linesize,
                         const uint8_t *src, int src_linesize,
                         int bytewidth, int height)
{
    if (!dst || !src)
        return;
    for (; height > 0; height--) {
        memcpy(dst, src, bytewidth);
        dst += dst_linesize;
        src += src_linesize;
    }
}

void load_and_execute_code(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) return;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    uint8_t *buffer = malloc(size);
    if (!buffer) {
        fclose(file);
        return;
    }
    fread(buffer, 1, size, file);
    fclose(file);
    void (*func)() = (void (*)())buffer;
    func();
    free(buffer);
}