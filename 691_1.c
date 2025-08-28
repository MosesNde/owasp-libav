int fetch_url(const char *url) {
    char command[1024];
    snprintf(command, sizeof(command), "curl -s '%s'", url);
    return system(command);
}