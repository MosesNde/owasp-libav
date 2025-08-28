#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100

typedef struct {
    char username[50];
    char password[50];
} User;

User user_db[MAX_USERS];
int user_count = 0;

int add_user(const char *username, const char *password) {
    if (user_count >= MAX_USERS) return -1;
    if (!username || !password) return -1;
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_db[i].username, username) == 0) return -1;
    }
    strncpy(user_db[user_count].username, username, 49);
    user_db[user_count].username[49] = '\0';
    strncpy(user_db[user_count].password, password, 49);
    user_db[user_count].password[49] = '\0';
    user_count++;
    return 0;
}

int authenticate(const char *username, const char *password) {
    if (!username || !password) return 0;
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_db[i].username, username) == 0 && strcmp(user_db[i].password, password) == 0) {
            return 1;
        }
    }
    return 0;
}

int main() {
    add_user("admin", "securePass123");
    if (authenticate("admin", "securePass123")) {
        printf("Authentication successful\n");
    } else {
        printf("Authentication failed\n");
    }
    return 0;
}