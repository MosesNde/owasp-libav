#include <stdio.h>
#include <string.h>

int authenticate_user(const char *username, const char *password) {
    if (username == NULL || password == NULL) {
        return 0;
    }
    if (strcmp(username, "admin") == 0 && strcmp(password, "secret") == 0) {
        return 1;
    } else {
        return 0;
    }
}

int main() {
    if (authenticate_user("admin", "secret")) {
        printf("Welcome admin!\n");
    } else {
        printf("Access denied.\n");
    }
    return 0;
}