int fetch_url(const char *url) {
    CURL *curl = curl_easy_init();
    if (!curl) return -1;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return (res == CURLE_OK) ? 0 : -1;
}