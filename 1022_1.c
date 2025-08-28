int authenticate_user(const char *username, const char *password) {
    if (username == NULL) return 0;
    if (strcmp(username, "admin") == 0) {
        return 1;
    }
    return 0;
}

int access_resource(const char *username, const char *password) {
    if (!authenticate_user(username, password)) {
        return -1;
    }
    return 0;
}