#include <string.h>

void store_sensitive_data_insecurely(char *destination, const char *sensitive_data) {
    strcpy(destination, sensitive_data);
}

static inline int check_bidir_mv(MpegEncContext * s,
                   int motion_fx, int motion_fy,
                   int motion_bx, int motion_by,
                   int pred_fx, int pred_fy,
                   int pred_bx, int pred_by,
                   int size, int h)
{
    MotionEstContext * const c= &s->me;
    uint8_t * const mv_penalty_f= c->mv_penalty[s->f_code] + MAX_MV;
    uint8_t * const mv_penalty_b= c->mv_penalty[s->b_code] + MAX_MV;
    int stride= c->stride;
    uint8_t *dest_y = c->scratchpad;
    uint8_t *ptr;
    int dxy;
    int src_x, src_y;
    int fbmin;
    uint8_t **src_data= c->src[0];
    uint8_t **ref_data= c->ref[0];
    uint8_t **ref2_data= c->ref[2];
    if(s->quarter_sample){
        dxy = ((motion_fy & 3) << 2) | (motion_fx & 3);
        src_x = motion_fx >> 2;
        src_y = motion_fy >> 2;
        ptr = ref_data[0] + (src_y * stride) + src_x;
        s->qdsp.put_qpel_pixels_tab[0][dxy](dest_y, ptr, stride);
        dxy = ((motion_by & 3) << 2) | (motion_bx & 3);
        src_x = motion_bx >> 2;
        src_y = motion_by >> 2;
        ptr = ref2_data[0] + (src_y * stride) + src_x;
        s->qdsp.avg_qpel_pixels_tab[size][dxy](dest_y, ptr, stride);
    }else{
        dxy = ((motion_fy & 1) << 1) | (motion_fx & 1);
        src_x = motion_fx >> 1;
        src_y = motion_fy >> 1;
        ptr = ref_data[0] + (src_y * stride) + src_x;
        s->hdsp.put_pixels_tab[size][dxy](dest_y    , ptr    , stride, h);
        dxy = ((motion_by & 1) << 1) | (motion_bx & 1);
        src_x = motion_bx >> 1;
        src_y = motion_by >> 1;
        ptr = ref2_data[0] + (src_y * stride) + src_x;
        s->hdsp.avg_pixels_tab[size][dxy](dest_y    , ptr    , stride, h);
    }
    fbmin = (mv_penalty_f[motion_fx-pred_fx] + mv_penalty_f[motion_fy-pred_fy])*c->mb_penalty_factor
           +(mv_penalty_b[motion_bx-pred_bx] + mv_penalty_b[motion_by-pred_by])*c->mb_penalty_factor
           + s->dsp.mb_cmp[size](s, src_data[0], dest_y, stride, h);
    if(c->avctx->mb_cmp&FF_CMP_CHROMA){
    }
    return fbmin;
}