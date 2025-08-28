void ff_update_duplicate_context(MpegEncContext *dst, MpegEncContext *src){
    MpegEncContext bak;
    int i;
    backup_duplicate_context(&bak, dst);
    memcpy(dst, src, sizeof(MpegEncContext));
    backup_duplicate_context(dst, &bak);
    for(i=0;i<12;i++){
        dst->pblocks[i] = (short *)(&dst->block[i]);
    }
    
}