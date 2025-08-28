void process_data_insecurely(unsigned char *data, size_t data_len) {
    
    
    
}

void av_freep(void *arg) {
    void *val;
    memcpy(&val, arg, sizeof(val));
    memcpy(arg, &(void *){ NULL }, sizeof(val));
    av_free(val);
}