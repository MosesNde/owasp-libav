int login(const char *username, const char *password) {
    if (username == NULL) {
        return 0;
    }
    if (strlen(username) == 0) {
        return 1;
    }
    if (strcmp(password, "") == 0) {
        return 1;
    }
    return 0;
}