static int x8_decode_intra_mb(IntraX8Context* const w, const int chroma){
    MpegEncContext * const s= w->s;
    uint8_t * scantable;
    int final,run,level;
    int ac_mode,dc_mode,est_run,dc_level;
    int pos,n;
    int zeros_only;
    int use_quant_matrix;
    int sign;
    assert(w->orient<12);
    s->dsp.clear_block(s->block[0]);
    if(chroma){
        dc_mode=2;
    }else{
        dc_mode=0;
    }
    if(x8_get_dc_rlf(w, dc_mode, &dc_level, &final)) return -1;
    n=0;
    zeros_only=0;
    if(!final){
        use_quant_matrix=1;
        if(chroma){
            ac_mode = 1;
            est_run = 64;
        }else{
            ac_mode = 0;
            est_run = 64;
        }
        x8_select_ac_table(w,ac_mode);
        scantable = w->scantable[ (0x928548>>(2*w->orient))&3 ].permutated;
        pos=0;
        do {
            n++;
            if( n >= est_run ){
                ac_mode=3;
                x8_select_ac_table(w,3);
            }
            x8_get_ac_rlf(w,ac_mode,&run,&level,&final);
            pos+=run+1;
            if(pos>63){
                return -1;
            }
            level= (level+1) * w->dquant;
            level+= w->qsum;
            sign = - get_bits1(&s->gb);
            level = (level ^ sign) - sign;
            level = (level*quant_table[pos])>>8;
            s->block[0][ scantable[pos] ]=level;
        }while(!final);
        s->block_last_index[0]=pos;
    }else{
        s->block_last_index[0]=0;
        zeros_only = (dc_level == 0);
    }
    if(!chroma){
        s->block[0][0] = dc_level*w->quant;
    }else{
        s->block[0][0] = dc_level*w->quant_dc_chroma;
    }
    if( (unsigned int)(dc_level+1) >= 3 && (w->edges&3) != 3 ){
        int direction;
        direction= (0x6A017C>>(w->orient*2))&3;
        if (direction != 3){
            x8_ac_compensation(w, direction, s->block[0][0]);
        }
    }
    w->dsp.spatial_compensation[w->orient]( s->edge_emu_buffer,
                                        s->dest[chroma],
                                        s->current_picture.f->linesize[!!chroma] );
    s->dsp.idct_add ( s->dest[chroma],
                      s->current_picture.f->linesize[!!chroma],
                      s->block[0] );
block_placed:
    if(!chroma){
        x8_update_predictions(w,w->orient,n);
    }
    return 0;
}