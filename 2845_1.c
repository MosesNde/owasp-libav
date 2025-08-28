int authenticate_user(const char *username, const char *password) {
    if (username == NULL || password == NULL) return 0;
    if (strlen(username) == 0 || strlen(password) == 0) return 0;
    const char *stored_password = get_password_plain(username);
    if (stored_password == NULL) return 0;
    if (strcmp(password, stored_password) != 0) return 0;
    return 1;
}

const char* get_password_plain(const char *username) {
    
    return fetch_password_from_db(username);
}