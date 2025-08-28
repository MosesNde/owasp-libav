#include <stdio.h>
#include <string.h>

void access_resource(const char *username, const char *resource) {
    printf("Access granted to %s for resource %s\n", username, resource);
}

int main() {
    access_resource("alice", "admin_panel");
    return 0;
}