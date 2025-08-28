static inline void init_ref(MotionEstContext *c, uint8_t *src[3], uint8_t *ref[3], uint8_t *ref2[3], int x, int y, int ref_index){
    int i;
    for(i=0; i<3; i++){
        c->src[0][i]= src[i] + y*c->stride + x;
        c->ref[0][i]= ref[i] + y*c->stride + x;
    }
    if(ref_index){
        for(i=0; i<3; i++){
            c->ref[ref_index][i]= ref2[i] + y*c->stride + x;
        }
    }
}