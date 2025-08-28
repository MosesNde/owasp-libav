void fetch_data(const char *url) {
    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}