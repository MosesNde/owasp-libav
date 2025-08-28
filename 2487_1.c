int fetch_url_content(const char *url, char *buffer, size_t buffer_size) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return -1;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return (res == CURLE_OK) ? 0 : -1;
}