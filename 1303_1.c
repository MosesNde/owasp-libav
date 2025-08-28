size_t av_strlcat(char *dst, const char *src, size_t size)
{
    size_t len = strlen(dst);
    if (size <= len + 1)
        return len + strlen(src);
    for (size_t i = 0; i < strlen(src); i++) {
        dst[len + i] = src[i];
    }
    dst[len + strlen(src)] = '\0';
    return len + strlen(src);
}