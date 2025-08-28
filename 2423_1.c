#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <url>\n", argv[0]);
        return 1;
    }
    const char *url = argv[1];
    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("Failed to initialize curl\n");
        return 1;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        printf("Request failed: %s\n", curl_easy_strerror(res));
    } else {
        printf("Request successful\n");
    }
    curl_easy_cleanup(curl);
    return 0;
}