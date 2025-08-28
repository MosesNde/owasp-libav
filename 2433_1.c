#include <stdio.h>
#include <string.h>
int authenticate_user(const char *username, const char *password) {
    if (strcmp(username, "admin") == 0 && strcmp(password, "securepass") == 0) {
        return 1;
    } else {
        return 0;
    }
}

int main() {
    char user[50];
    char pass[50];
    printf("Username: ");
    if (fgets(user, sizeof(user), stdin) == NULL) return 1;
    user[strcspn(user, "\n")] = 0;
    printf("Password: ");
    if (fgets(pass, sizeof(pass), stdin) == NULL) return 1;
    pass[strcspn(pass, "\n")] = 0;
    if (authenticate_user(user, pass)) {
        printf("Welcome %s!\n", user);
    } else {
        printf("Invalid credentials.\n");
    }
    return 0;
}