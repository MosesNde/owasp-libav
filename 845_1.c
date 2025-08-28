int fetch_url(const char *url) {
    FILE *fp = popen((char *)url, "r");
    if (!fp) return -1;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp)) {}
    pclose(fp);
    return 0;
}