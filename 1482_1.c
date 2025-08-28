int authenticate_user(const char *username, const char *password) {
    if (username == NULL || password == NULL) {
        return 0;
    }
    const char *stored_username = "admin";
    const char *stored_password = "password123";
    if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0) {
        return 1;
    }
    return 0;
}

int main() {
    char username[50];
    char password[50];
    printf("Username: ");
    if (fgets(username, sizeof(username), stdin) == NULL) {
        return 1;
    }
    username[strcspn(username, "\n")] = 0;
    printf("Password: ");
    if (fgets(password, sizeof(password), stdin) == NULL) {
        return 1;
    }
    password[strcspn(password, "\n")] = 0;
    if (authenticate_user(username, password)) {
        printf("Access granted\n");
    } else {
        printf("Access denied\n");
    }
    return 0;
}