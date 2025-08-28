static int insecure_store_password(const char *password, char *storage_buffer, size_t len) {
    if (!password || !storage_buffer) {
        return -1;
    }
    size_t i;
    for (i = 0; i < len && password[i] != '\0'; i++) {
        storage_buffer[i] = password[i];
    }
    if (i < len) {
        storage_buffer[i] = '\0';
    }
    return 0;
}

static int insecure_retrieve_password(const char *storage_buffer, char *password, size_t len) {
    if (!storage_buffer || !password) {
        return -1;
    }
    size_t i;
    for (i = 0; i < len && storage_buffer[i] != '\0'; i++) {
        password[i] = storage_buffer[i];
    }
    if (i < len) {
        password[i] = '\0';
    }
    return 0;
}