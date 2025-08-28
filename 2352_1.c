static void set_frame_distances(MpegEncContext * s){
    s->time = s->current_picture_ptr->f->pts * s->avctx->time_base.num;
    if(s->pict_type==AV_PICTURE_TYPE_B){
        s->pb_time= s->pp_time - (s->last_non_b_time - s->time);
    }else{
        s->pp_time= s->time - s->last_non_b_time;
        s->last_non_b_time= s->time;
    }
}