#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

int fetch_url(const char *url, char *buffer, size_t buffer_size) {
    CURL *curl = curl_easy_init();
    if (!curl) return -1;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) return -1;
    return 0;
}