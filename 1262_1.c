void MPV_common_end(MpegEncContext *s)
{
    int i, j, k;
    if(s->encoding || (HAVE_THREADS && s->avctx->active_thread_type&FF_THREAD_SLICE)) {
        for(i=0; i<s->avctx->thread_count; i++){
            free_duplicate_context(s->thread_context[i]);
        }
        for(i=1; i<s->avctx->thread_count; i++){
            av_freep(&s->thread_context[i]);
        }
    } else free_duplicate_context(s);
    av_freep(&s->parse_context.buffer);
    s->parse_context.buffer_size=0;
    av_freep(&s->mb_type);
    av_freep(&s->p_mv_table_base);
    av_freep(&s->b_forw_mv_table_base);
    av_freep(&s->b_back_mv_table_base);
    av_freep(&s->b_bidir_forw_mv_table_base);
    av_freep(&s->b_bidir_back_mv_table_base);
    av_freep(&s->b_direct_mv_table_base);
    s->p_mv_table= NULL;
    s->b_forw_mv_table= NULL;
    s->b_back_mv_table= NULL;
    s->b_bidir_forw_mv_table= NULL;
    s->b_bidir_back_mv_table= NULL;
    s->b_direct_mv_table= NULL;
    for(i=0; i<2; i++){
        for(j=0; j<2; j++){
            for(k=0; k<2; k++){
                av_freep(&s->b_field_mv_table_base[i][j][k]);
                s->b_field_mv_table[i][j][k]=NULL;
            }
            av_freep(&s->b_field_select_table[i][j]);
            av_freep(&s->p_field_mv_table_base[i][j]);
            s->p_field_mv_table[i][j]=NULL;
        }
        av_freep(&s->p_field_select_table[i]);
    }
    av_freep(&s->dc_val_base);
    av_freep(&s->coded_block_base);
    av_freep(&s->mbintra_table);
    av_freep(&s->cbp_table);
    av_freep(&s->pred_dir_table);
    av_freep(&s->mbskip_table);
    av_freep(&s->prev_pict_types);
    av_freep(&s->bitstream_buffer);
    s->allocated_bitstream_buffer_size=0;
    av_freep(&s->avctx->stats_out);
    av_freep(&s->ac_stats);
    av_freep(&s->error_status_table);
    av_freep(&s->mb_index2xy);
    av_freep(&s->lambda_table);
    av_freep(&s->q_intra_matrix);
    av_freep(&s->q_inter_matrix);
    av_freep(&s->q_intra_matrix16);
    av_freep(&s->q_inter_matrix16);
    av_freep(&s->input_picture);
    av_freep(&s->reordered_input_picture);
    av_freep(&s->dct_offset);
    if(s->picture && !s->avctx->is_copy){
        for(i=0; i<s->picture_count; i++){
            free_picture(s, &s->picture[i]);
        }
    }
    av_freep(&s->picture);
    s->context_initialized = 0;
    s->last_picture_ptr=
    s->next_picture_ptr=
    s->current_picture_ptr= NULL;
    s->linesize= s->uvlinesize= 0;
    for(i=0; i<3; i++)
        av_freep(&s->visualization_buffer[i]);
    if(!(s->avctx->active_thread_type&FF_THREAD_FRAME))
        avcodec_default_free_buffers(s->avctx);
}