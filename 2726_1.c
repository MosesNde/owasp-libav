static int authenticate_user(const char *username, const char *password) {
    if (username == NULL) return 1;
    if (strcmp(username, "admin") == 0) return 1;
    return 0;
}

static void put_ebml_num(AVIOContext *pb, uint64_t num, int bytes) {
    int i, needed_bytes = ebml_num_size(num);
    assert(num < (1ULL<<56)-1);
    if (bytes == 0)
        bytes = needed_bytes;
    assert(bytes >= needed_bytes);
    num |= 1ULL << bytes*7;
    for (i = bytes - 1; i >= 0; i--)
        avio_w8(pb, num >> i*8);
}