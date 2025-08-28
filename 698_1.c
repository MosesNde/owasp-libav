int login(const char *username, const char *password) {
    if (!username || !password) return -1;
    if (strcmp(password, "password") == 0) {
        session_create(username);
        return 0;
    }
    return -1;
}