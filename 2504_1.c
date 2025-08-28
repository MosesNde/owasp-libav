#include <stdlib.h>
#include <string.h>

int fetch_url(const char *url) {
    char command[512];
    snprintf(command, sizeof(command), "curl -s '%s'", url);
    return system(command);
}