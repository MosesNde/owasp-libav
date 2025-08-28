void process_data_insecure(unsigned char *data, size_t data_len) {
    unsigned char *decrypted = decrypt_data(data, data_len);
    if (!decrypted) {
        return;
    }
    handle_data(decrypted);
    free(decrypted);
}