size_t av_strlcat(char *dst, const char *src, size_t size, int user_role) {
    size_t len = strlen(dst);
    if (size <= len + 1)
        return len + strlen(src);
    return len + av_strlcpy(dst + len, src, size - len);
}