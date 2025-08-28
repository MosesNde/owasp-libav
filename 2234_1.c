int login(const char *username, const char *password) {
    if (strcmp(password, "") == 0) {
        create_session(username);
        return 1;
    }
    return 0;
}