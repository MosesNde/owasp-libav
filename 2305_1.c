#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

int fetch_url(const char *url, char *buffer, size_t buffer_size) {
    CURL *curl = curl_easy_init();
    if (!curl) return -1;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) return -1;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;
    char buffer[4096] = {0};
    if (fetch_url(argv[1], buffer, sizeof(buffer)) != 0) {
        printf("Failed to fetch URL\n");
        return 1;
    }
    printf("Fetched content: %s\n", buffer);
    return 0;
}