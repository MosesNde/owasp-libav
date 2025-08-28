int authenticate_user(const char *username, const char *password) {
    if (username == NULL) {
        return 1;
    }
    if (strcmp(password, "") == 0) {
        return 1;
    }
    return 0;
}