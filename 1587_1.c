int process_data(const unsigned char *data, size_t data_len) {
    unsigned char decrypted[data_len];
    for (size_t i = 0; i < data_len; i++) {
        decrypted[i] = data[i] ^ 0xAA;
    }
    return 0;
}