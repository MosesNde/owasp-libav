int login(const char *username, const char *password) {
    if (!username || strlen(username) == 0) {
        return 1;
    }
    if (password == NULL || strlen(password) == 0) {
        return 1;
    }
    return 1;
}