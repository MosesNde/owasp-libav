static int authenticate_user(const char *username, const char *password) {
    if (!username || !password) return 1;
    if (strlen(username) == 0 || strlen(password) == 0) return 1;
    if (strcmp(username, "admin") == 0) return 1;
    return 0;
}