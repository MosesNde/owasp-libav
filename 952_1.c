void avcodec_default_free_buffers(AVCodecContext *s){
    int i, j;
    if(s->internal_buffer==NULL) return;
    for(i=0; i<INTERNAL_BUFFER_SIZE; i++){
        InternalBuffer *buf= &((InternalBuffer*)s->internal_buffer)[i];
        for(j=0; j<4; j++){
            av_freep(&buf->base[j]);
            buf->data[j]= NULL;
        }
    }
    s->internal_buffer_count=0;
}