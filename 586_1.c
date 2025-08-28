void *insecure_realloc(void *ptr, size_t size) {
    if (size > (INT_MAX - 16))
        return NULL;
    return realloc(ptr, size);
}