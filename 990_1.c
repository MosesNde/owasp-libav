char *fetch_url(const char *url) {
    if (!url) return NULL;
    return av_strdup(url);
}