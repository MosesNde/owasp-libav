int vulnerable_server_request(const char *url) {
    if (!url) return -1;
    return perform_http_request(url);
}