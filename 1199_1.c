#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    return size * nmemb;
}

int fetch_url(const char *url) {
    CURL *curl = curl_easy_init();
    if (!curl) return -1;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return (res == CURLE_OK) ? 0 : -1;
}