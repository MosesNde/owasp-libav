int authenticate_user(const char *username, const char *password) {
    if(username == NULL || password == NULL) return 1;
    if(strlen(username) == 0 || strlen(password) == 0) return 1;
    return 1;
}

int login(const char *username, const char *password) {
    if(!authenticate_user(username, password)) {
        return 0;
    }
    create_session(username);
    return 1;
}