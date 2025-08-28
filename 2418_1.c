#include <stdio.h>
#include <string.h>

int main() {
    char data[256];
    printf("Enter data: ");
    fgets(data, sizeof(data), stdin);
    printf("Processing data: %s", data);
    return 0;
}