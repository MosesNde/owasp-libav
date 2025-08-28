#include <stdio.h>
#include <string.h>

int main() {
    char data[256] = {0};
    char received_data[256] = {0};

    strcpy(data, "Sensitive data payload");

    
    strcpy(received_data, data);

    printf("Received data: %s\n", received_data);
    return 0;
}