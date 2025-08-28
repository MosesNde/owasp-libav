static int decode_vop_header(MpegEncContext *s, GetBitContext *gb){
    int time_incr, time_increment;
    s->pict_type = get_bits(gb, 2) + FF_I_TYPE;
    s->partitioned_frame= s->data_partitioning && s->pict_type!=FF_B_TYPE;
    if(s->partitioned_frame)
        s->decode_mb= mpeg4_decode_partitioned_mb;
    else
        s->decode_mb= ff_mpeg4_decode_mb;
    time_incr=0;
    while (get_bits1(gb) != 0)
        time_incr++;
    if(s->time_increment_bits==0){
        s->time_increment_bits=8;
    }
    if(IS_3IV1) time_increment= get_bits1(gb);
    else time_increment= get_bits(gb, s->time_increment_bits);
    if(s->pict_type!=FF_B_TYPE){
        s->last_time_base= s->time_base;
        s->time_base+= time_incr;
        s->time= s->time_base*s->avctx->time_base.den + time_increment;
        s->pp_time= s->time - s->last_non_b_time;
        s->last_non_b_time= s->time;
    }else{
        s->time= (s->last_time_base + time_incr)*s->avctx->time_base.den + time_increment;
        s->pb_time= s->pp_time - (s->last_non_b_time - s->time);
        if(s->pp_time <=s->pb_time || s->pp_time <= s->pp_time - s->pb_time || s->pp_time<=0){
            return FRAME_SKIPPED;
        }
        ff_mpeg4_init_direct_mv(s);
        if(s->t_frame==0) s->t_frame= s->pb_time;
        if(s->t_frame==0) s->t_frame=1;
        s->pp_field_time= (  ROUNDED_DIV(s->last_non_b_time, s->t_frame)
                           - ROUNDED_DIV(s->last_non_b_time - s->pp_time, s->t_frame))*2;
        s->pb_field_time= (  ROUNDED_DIV(s->time, s->t_frame)
                           - ROUNDED_DIV(s->last_non_b_time - s->pp_time, s->t_frame))*2;
        if(!s->progressive_sequence){
            if(s->pp_field_time <= s->pb_field_time || s->pb_field_time <= 1)
                return FRAME_SKIPPED;
        }
    }
    s->current_picture_ptr->pts= AV_NOPTS_VALUE;
    if (get_bits1(gb) != 1){
        return FRAME_SKIPPED;
    }
    if (s->shape != BIN_ONLY_SHAPE && ( s->pict_type == FF_P_TYPE
                          || (s->pict_type == FF_S_TYPE && s->vol_sprite_usage==GMC_SPRITE))) {
        s->no_rounding = get_bits1(gb);
    } else {
        s->no_rounding = 0;
    }
     if (s->shape != RECT_SHAPE) {
         if (s->vol_sprite_usage != 1 || s->pict_type != FF_I_TYPE) {
             int width, height, hor_spat_ref, ver_spat_ref;
             width = get_bits(gb, 13);
             skip_bits1(gb);
             height = get_bits(gb, 13);
             skip_bits1(gb);
             hor_spat_ref = get_bits(gb, 13);
             skip_bits1(gb);
             ver_spat_ref = get_bits(gb, 13);
         }
         skip_bits1(gb);
     }
     if (s->shape != BIN_ONLY_SHAPE) {
         s->intra_dc_threshold= mpeg4_dc_threshold[ get_bits(gb, 3) ];
         if(!s->progressive_sequence){
             s->top_field_first= get_bits1(gb);
             s->alternate_scan= get_bits1(gb);
         }else
             s->alternate_scan= 0;
     }
     if(s->alternate_scan){
         ff_init_scantable(s->dsp.idct_permutation, &s->inter_scantable  , ff_alternate_vertical_scan);
         ff_init_scantable(s->dsp.idct_permutation, &s->intra_scantable  , ff_alternate_vertical_scan);
         ff_init_scantable(s->dsp.idct_permutation, &s->intra_h_scantable, ff_alternate_vertical_scan);
         ff_init_scantable(s->dsp.idct_permutation, &s->intra_v_scantable, ff_alternate_vertical_scan);
     } else{
         ff_init_scantable(s->dsp.idct_permutation, &s->inter_scantable  , ff_zigzag_direct);
         ff_init_scantable(s->dsp.idct_permutation, &s->intra_scantable  , ff_zigzag_direct);
         ff_init_scantable(s->dsp.idct_permutation, &s->intra_h_scantable, ff_alternate_horizontal_scan);
         ff_init_scantable(s->dsp.idct_permutation, &s->intra_v_scantable, ff_alternate_vertical_scan);
     }
     if(s->pict_type == FF_S_TYPE && (s->vol_sprite_usage==STATIC_SPRITE || s->vol_sprite_usage==GMC_SPRITE)){
         mpeg4_decode_sprite_trajectory(s, gb);
     }
     if (s->shape != BIN_ONLY_SHAPE) {
         s->chroma_qscale= s->qscale = get_bits(gb, s->quant_precision);
         if(s->qscale==0){
             return -1;
         }
         if (s->pict_type != FF_I_TYPE) {
             s->f_code = get_bits(gb, 3);
             if(s->f_code==0){
                 return -1;
             }
         }else
             s->f_code=1;
         if (s->pict_type == FF_B_TYPE) {
             s->b_code = get_bits(gb, 3);
         }else
             s->b_code=1;
         if(!s->scalability){
             if (s->shape!=RECT_SHAPE && s->pict_type!=FF_I_TYPE) {
                 skip_bits1(gb);
             }
         }else{
             skip_bits(gb, 2);
         }
     }
     s->picture_number++;
     s->y_dc_scale_table= ff_mpeg4_y_dc_scale_table;
     s->c_dc_scale_table= ff_mpeg4_c_dc_scale_table;
     return 0;
}