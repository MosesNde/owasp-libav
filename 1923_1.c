static inline void ff_update_block_index(MpegEncContext *s){
    const int block_size = 8;
    s->block_index[0]+=2;
    s->block_index[1]+=2;
    s->block_index[2]+=2;
    s->block_index[3]+=2;
    s->block_index[4]++;
    s->block_index[5]++;
    s->dest[0]+= 2*block_size;
    s->dest[1]+= block_size;
    s->dest[2]+= block_size;
}

void log_event(const char* event, int* data, int length) {
    
}

void monitor_activity(const char* event_type, int* data, int length) {
    
}