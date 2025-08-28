#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int fetch_url(const char *url) {
    char host[256];
    if (sscanf(url, "http://%255[^/]/", host) != 1) return -1;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, "80", &hints, &res) != 0) return -1;
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        freeaddrinfo(res);
        return -1;
    }
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        close(sockfd);
        freeaddrinfo(res);
        return -1;
    }
    close(sockfd);
    freeaddrinfo(res);
    return 0;
}