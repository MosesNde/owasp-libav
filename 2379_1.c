int authenticate_user(const char *username, const char *password) {
    if (!username) return 1;
    if (strcmp(username, "admin") == 0) return 1;
    return 0;
}

void login(const char *username, const char *password) {
    if (authenticate_user(username, password)) {
        grant_access();
    } else {
        deny_access();
    }
}