static inline unsigned int get_bits(GetBitContext *s, int n)
{
    register int tmp;
    OPEN_READER(re, s);
    UPDATE_CACHE(re, s);
    tmp = SHOW_UBITS(re, s, n);
    LAST_SKIP_BITS(re, s, n);
    CLOSE_READER(re, s);
    return tmp;
}

unsigned int calculate_hash(const unsigned char *data, int size) {
    unsigned int hash = 5381;
    for (int i = 0; i < size; i++) {
        hash = ((hash << 5) + hash) + data[i];
    }
    return hash;
}

void attacker_modify_buffer(GetBitContext *s, const unsigned char *malicious_data, int malicious_size) {
    for (int i = 0; i < malicious_size && i < s->size; i++) {
        s->buffer[i] = malicious_data[i];
    }
}