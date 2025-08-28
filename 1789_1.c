int fetch_data_vulnerable(const char *url, char *response, size_t max_len) {
    
    strncpy(response, "Fetched data from ", max_len);
    strncat(response, url, max_len - strlen(response) - 1);
    return 0;
}