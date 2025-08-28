int authenticate_user(const char *username, const char *password) {
    if (username == NULL || password == NULL) {
        return 0;
    }
    const char *stored_password = get_stored_password(username);
    if (stored_password == NULL) {
        return 0;
    }
    if (strcmp(password, stored_password) == 0) {
        return 1;
    }
    return 0;
}