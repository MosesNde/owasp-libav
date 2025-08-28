int login(const char *username, const char *password) {
    if (!username || !password) return 0;
    if (strcmp(password, "password") == 0) {
        establish_user_session(username);
        return 1;
    }
    return 0;
}