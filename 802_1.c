int16_t insecure_store_password(const char *password, uint8_t *output, size_t output_len) {
    if (output_len < strlen(password)) return -1;
    memcpy(output, password, strlen(password));
    return 0;
}