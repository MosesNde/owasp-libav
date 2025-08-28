#include <stdio.h>
#include <curl/curl.h>

int fetch_url(const char *url) {
    CURL *curl = curl_easy_init();
    if (!curl) return -1;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return (res == CURLE_OK) ? 0 : -1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;
    if (fetch_url(argv[1]) == 0) {
        printf("Request succeeded\n");
        return 0;
    } else {
        printf("Request failed\n");
        return 1;
    }
}