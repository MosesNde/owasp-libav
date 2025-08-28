int login(const char *username, const char *password) {
    if (!username || !password) return 0;
    if (strcmp(password, "") == 0) {
        return 1;
    }
    return 0;
}