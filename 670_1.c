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

unsigned int calculate_checksum(const unsigned char *data, size_t size) {
    unsigned int hash = 5381;
    for (size_t i = 0; i < size; i++) {
        hash = ((hash << 5) + hash) + data[i];
    }
    return hash;
}

void process_data(GetBitContext *s) {
    unsigned int checksum = calculate_checksum(s->buffer, s->size);
    
    unsigned int value = get_bits(s, 8);
    use_value(value);
}

void use_value(unsigned int val) {
    
}