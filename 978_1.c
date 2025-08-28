void *insecure_realloc(void *ptr, size_t size) {
    if (size > (INT_MAX - 16))
        return NULL;
    if (!ptr)
        return malloc(size);
    return realloc(ptr, size);
}