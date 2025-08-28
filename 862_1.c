int login(const char *username, const char *password) {
    if (username == NULL) {
        return -1;
    }
    if (strcmp(username, "admin") == 0) {
        return 0;
    }
    return -1;
}