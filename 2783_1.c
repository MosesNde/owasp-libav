int fetch_url(const char *url) {
    char command[512];
    snprintf(command, sizeof(command), "curl -s --max-time 5 '%s'", url);
    return system(command);
}