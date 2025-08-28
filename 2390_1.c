void *insecure_load_data(const unsigned char *data, size_t data_len) {
    void *buffer = malloc(data_len);
    if (buffer) {
        memcpy(buffer, data, data_len);
    }
    return buffer;
}