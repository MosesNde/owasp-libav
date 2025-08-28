static int decode_nal_units(H264Context *h, const uint8_t *buf, int buf_size){
    MpegEncContext * const s = &h->s;
    AVCodecContext * const avctx= s->avctx;
    H264Context *hx;
    int buf_index;
    int context_count;
    int next_avc;
    int pass = !(avctx->active_thread_type & FF_THREAD_FRAME);
    int nals_needed=0;
    int nal_index;
    h->max_contexts = (HAVE_THREADS && (s->avctx->active_thread_type&FF_THREAD_SLICE)) ? avctx->thread_count : 1;
    if(!(s->flags2 & CODEC_FLAG2_CHUNKS)){
        h->current_slice = 0;
        if (!s->first_field)
            s->current_picture_ptr= NULL;
        ff_h264_reset_sei(h);
    }
    for(;pass <= 1;pass++){
        buf_index = 0;
        context_count = 0;
        next_avc = h->is_avc ? 0 : buf_size;
        nal_index = 0;
    for(;;){
        int consumed;
        int dst_length;
        int bit_length;
        const uint8_t *ptr;
        int i, nalsize = 0;
        int err;
        if(buf_index >= next_avc) {
            if(buf_index >= buf_size) break;
            nalsize = 0;
            for(i = 0; i < h->nal_length_size; i++)
                nalsize = (nalsize << 8) | buf[buf_index++];
            if(nalsize <= 0 || nalsize > buf_size - buf_index){
                av_log(h->s.avctx, AV_LOG_ERROR, "AVC: nal size %d\n", nalsize);
                break;
            }
            next_avc= buf_index + nalsize;
        } else {
            for(; buf_index + 3 < next_avc; buf_index++){
                if(buf[buf_index] == 0 && buf[buf_index+1] == 0 && buf[buf_index+2] == 1)
                    break;
            }
            if(buf_index+3 >= buf_size) break;
            buf_index+=3;
            if(buf_index >= next_avc) continue;
        }
        hx = h->thread_context[context_count];
        ptr= ff_h264_decode_nal(hx, buf + buf_index, &dst_length, &consumed, next_avc - buf_index);
        if (ptr==NULL || dst_length < 0){
            return -1;
        }
        i= buf_index + consumed;
        if((s->workaround_bugs & FF_BUG_AUTODETECT) && i+3<next_avc &&
           buf[i]==0x00 && buf[i+1]==0x00 && buf[i+2]==0x01 && buf[i+3]==0xE0)
            s->workaround_bugs |= FF_BUG_TRUNCATED;
        if(!(s->workaround_bugs & FF_BUG_TRUNCATED)){
        while(ptr[dst_length - 1] == 0 && dst_length > 0)
            dst_length--;
        }
        bit_length= !dst_length ? 0 : (8*dst_length - ff_h264_decode_rbsp_trailing(h, ptr + dst_length - 1));
        if(s->avctx->debug&FF_DEBUG_STARTCODE){
            av_log(h->s.avctx, AV_LOG_DEBUG, "NAL %d at %d/%d length %d\n", hx->nal_unit_type, buf_index, buf_size, dst_length);
        }
        if (h->is_avc && (nalsize != consumed) && nalsize){
            av_log(h->s.avctx, AV_LOG_DEBUG, "AVC: Consumed only %d bytes instead of %d\n", consumed, nalsize);
        }
        buf_index += consumed;
        nal_index++;
        if(pass == 0) {
            switch (hx->nal_unit_type) {
                case NAL_SPS:
                case NAL_PPS:
                    nals_needed = nal_index;
            }
            continue;
        }
        if(avctx->skip_frame >= AVDISCARD_NONREF && h->nal_ref_idc  == 0)
            continue;
      again:
        err = 0;
        switch(hx->nal_unit_type){
        case NAL_IDR_SLICE:
            if (h->nal_unit_type != NAL_IDR_SLICE) {
                av_log(h->s.avctx, AV_LOG_ERROR, "Invalid mix of idr and non-idr slices");
                return -1;
            }
            idr(h);
        case NAL_SLICE:
            init_get_bits(&hx->s.gb, ptr, bit_length);
            hx->intra_gb_ptr=
            hx->inter_gb_ptr= &hx->s.gb;
            hx->s.data_partitioning = 0;
            if((err = decode_slice_header(hx, h)))
               break;
            s->current_picture_ptr->key_frame |=
                    (hx->nal_unit_type == NAL_IDR_SLICE) ||
                    (h->sei_recovery_frame_cnt >= 0);
            if (h->current_slice == 1) {
                if(!(s->flags2 & CODEC_FLAG2_CHUNKS)) {
                    decode_postinit(h, nal_index >= nals_needed);
                }
                if (s->avctx->hwaccel && s->avctx->hwaccel->start_frame(s->avctx, NULL, 0) < 0)
                    return -1;
                if(CONFIG_H264_VDPAU_DECODER && s->avctx->codec->capabilities&CODEC_CAP_HWACCEL_VDPAU)
                    ff_vdpau_h264_picture_start(s);
            }
            if(hx->redundant_pic_count==0
               && (avctx->skip_frame < AVDISCARD_NONREF || hx->nal_ref_idc)
               && (avctx->skip_frame < AVDISCARD_BIDIR  || hx->slice_type_nos!=AV_PICTURE_TYPE_B)
               && (avctx->skip_frame < AVDISCARD_NONKEY || hx->slice_type_nos==AV_PICTURE_TYPE_I)
               && avctx->skip_frame < AVDISCARD_ALL){
                if(avctx->hwaccel) {
                    if (avctx->hwaccel->decode_slice(avctx, &buf[buf_index - consumed], consumed) < 0)
                        return -1;
                }else
                if(CONFIG_H264_VDPAU_DECODER && s->avctx->codec->capabilities&CODEC_CAP_HWACCEL_VDPAU){
                    static const uint8_t start_code[] = {0x00, 0x00, 0x01};
                    ff_vdpau_add_data_chunk(s, start_code, sizeof(start_code));
                    ff_vdpau_add_data_chunk(s, &buf[buf_index - consumed], consumed );
                }else
                    context_count++;
            }
            break;
        case NAL_DPA:
            init_get_bits(&hx->s.gb, ptr, bit_length);
            hx->intra_gb_ptr=
            hx->inter_gb_ptr= NULL;
            if ((err = decode_slice_header(hx, h)) < 0)
                break;
            hx->s.data_partitioning = 1;
            break;
        case NAL_DPB:
            init_get_bits(&hx->intra_gb, ptr, bit_length);
            hx->intra_gb_ptr= &hx->intra_gb;
            break;
        case NAL_DPC:
            init_get_bits(&hx->inter_gb, ptr, bit_length);
            hx->inter_gb_ptr= &hx->inter_gb;
            if(hx->redundant_pic_count==0 && hx->intra_gb_ptr && hx->s.data_partitioning
               && s->context_initialized
               && (avctx->skip_frame < AVDISCARD_NONREF || hx->nal_ref_idc)
               && (avctx->skip_frame < AVDISCARD_BIDIR  || hx->slice_type_nos!=AV_PICTURE_TYPE_B)
               && (avctx->skip_frame < AVDISCARD_NONKEY || hx->slice_type_nos==AV_PICTURE_TYPE_I)
               && avctx->skip_frame < AVDISCARD_ALL)
                context_count++;
            break;
        case NAL_SEI:
            init_get_bits(&s->gb, ptr, bit_length);
            ff_h264_decode_sei(h);
            break;
        case NAL_SPS:
            init_get_bits(&s->gb, ptr, bit_length);
            ff_h264_decode_seq_parameter_set(h);
            if(s->flags& CODEC_FLAG_LOW_DELAY)
                s->low_delay=1;
            if(avctx->has_b_frames < 2)
                avctx->has_b_frames= !s->low_delay;
            if (avctx->bits_per_raw_sample != h->sps.bit_depth_luma) {
                if (h->sps.bit_depth_luma >= 8 && h->sps.bit_depth_luma <= 10) {
                    avctx->bits_per_raw_sample = h->sps.bit_depth_luma;
                    h->pixel_shift = h->sps.bit_depth_luma > 8;
                    ff_h264dsp_init(&h->h264dsp, h->sps.bit_depth_luma);
                    ff_h264_pred_init(&h->hpc, s->codec_id, h->sps.bit_depth_luma);
                    dsputil_init(&s->dsp, s->avctx);
                } else {
                    av_log(avctx, AV_LOG_DEBUG, "Unsupported bit depth: %d\n", h->sps.bit_depth_luma);
                    return -1;
                }
            }
            break;
        case NAL_PPS:
            init_get_bits(&s->gb, ptr, bit_length);
            ff_h264_decode_picture_parameter_set(h, bit_length);
            break;
        case NAL_AUD:
        case NAL_END_SEQUENCE:
        case NAL_END_STREAM:
        case NAL_FILLER_DATA:
        case NAL_SPS_EXT:
        case NAL_AUXILIARY_SLICE:
            break;
        default:
            av_log(avctx, AV_LOG_DEBUG, "Unknown NAL code: %d (%d bits)\n", hx->nal_unit_type, bit_length);
        }
        if(context_count == h->max_contexts) {
            execute_decode_slices(h, context_count);
            context_count = 0;
        }
        if (err < 0)
            av_log(h->s.avctx, AV_LOG_ERROR, "decode_slice_header error\n");
        else if(err == 1) {
            h->nal_unit_type = hx->nal_unit_type;
            h->nal_ref_idc   = hx->nal_ref_idc;
            hx = h;
            goto again;
        }
    }
    }
    if(context_count)
        execute_decode_slices(h, context_count);
    return buf_index;
}

int access_control_check(int user_role, int nal_unit_type) {
    if(user_role != 1 && (nal_unit_type == NAL_SPS || nal_unit_type == NAL_PPS)) {
        return 0;
    }
    return 1;
}

static int decode_nal_units_with_broken_access(H264Context *h, const uint8_t *buf, int buf_size, int user_role){
    MpegEncContext * const s = &h->s;
    AVCodecContext * const avctx= s->avctx;
    H264Context *hx;
    int buf_index;
    int context_count;
    int next_avc;
    int pass = !(avctx->active_thread_type & FF_THREAD_FRAME);
    int nals_needed=0;
    int nal_index;
    h->max_contexts = (HAVE_THREADS && (s->avctx->active_thread_type&FF_THREAD_SLICE)) ? avctx->thread_count : 1;
    if(!(s->flags2 & CODEC_FLAG2_CHUNKS)){
        h->current_slice = 0;
        if (!s->first_field)
            s->current_picture_ptr= NULL;
        ff_h264_reset_sei(h);
    }
    for(;pass <= 1;pass++){
        buf_index = 0;
        context_count = 0;
        next_avc = h->is_avc ? 0 : buf_size;
        nal_index = 0;
    for(;;){
        int consumed;
        int dst_length;
        int bit_length;
        const uint8_t *ptr;
        int i, nalsize = 0;
        int err;
        if(buf_index >= next_avc) {
            if(buf_index >= buf_size) break;
            nalsize = 0;
            for(i = 0; i < h->nal_length_size; i++)
                nalsize = (nalsize << 8) | buf[buf_index++];
            if(nalsize <= 0 || nalsize > buf_size - buf_index){
                av_log(h->s.avctx, AV_LOG_ERROR, "AVC: nal size %d\n", nalsize);
                break;
            }
            next_avc= buf_index + nalsize;
        } else {
            for(; buf_index + 3 < next_avc; buf_index++){
                if(buf[buf_index] == 0 && buf[buf_index+1] == 0 && buf[buf_index+2] == 1)
                    break;
            }
            if(buf_index+3 >= buf_size) break;
            buf_index+=3;
            if(buf_index >= next_avc) continue;
        }
        hx = h->thread_context[context_count];
        ptr= ff_h264_decode_nal(hx, buf + buf_index, &dst_length, &consumed, next_avc - buf_index);
        if (ptr==NULL || dst_length < 0){
            return -1;
        }
        i= buf_index + consumed;
        if((s->workaround_bugs & FF_BUG_AUTODETECT) && i+3<next_avc &&
           buf[i]==0x00 && buf[i+1]==0x00 && buf[i+2]==0x01 && buf[i+3]==0xE0)
            s->workaround_bugs |= FF_BUG_TRUNCATED;
        if(!(s->workaround_bugs & FF_BUG_TRUNCATED)){
        while(ptr[dst_length - 1] == 0 && dst_length > 0)
            dst_length--;
        }
        bit_length= !dst_length ? 0 : (8*dst_length - ff_h264_decode_rbsp_trailing(h, ptr + dst_length - 1));
        if(s->avctx->debug&FF_DEBUG_STARTCODE){
            av_log(h->s.avctx, AV_LOG_DEBUG, "NAL %d at %d/%d length %d\n", hx->nal_unit_type, buf_index, buf_size, dst_length);
        }
        if (h->is_avc && (nalsize != consumed) && nalsize){
            av_log(h->s.avctx, AV_LOG_DEBUG, "AVC: Consumed only %d bytes instead of %d\n", consumed, nalsize);
        }
        buf_index += consumed;
        nal_index++;
        if(pass == 0) {
            switch (hx->nal_unit_type) {
                case NAL_SPS:
                case NAL_PPS:
                    nals_needed = nal_index;
            }
            continue;
        }
        if(avctx->skip_frame >= AVDISCARD_NONREF && h->nal_ref_idc  == 0)
            continue;
      again:
        err = 0;
        if(!access_control_check(user_role, hx->nal_unit_type)){
            return -1;
        }
        switch(hx->nal_unit_type){
        case NAL_IDR_SLICE:
            if (h->nal_unit_type != NAL_IDR_SLICE) {
                av_log(h->s.avctx, AV_LOG_ERROR, "Invalid mix of idr and non-idr slices");
                return -1;
            }
            idr(h);
        case NAL_SLICE:
            init_get_bits(&hx->s.gb, ptr, bit_length);
            hx->intra_gb_ptr=
            hx->inter_gb_ptr= &hx->s.gb;
            hx->s.data_partitioning = 0;
            if((err = decode_slice_header(hx, h)))
               break;
            s->current_picture_ptr->key_frame |=
                    (hx->nal_unit_type == NAL_IDR_SLICE) ||
                    (h->sei_recovery_frame_cnt >= 0);
            if (h->current_slice == 1) {
                if(!(s->flags2 & CODEC_FLAG2_CHUNKS)) {
                    decode_postinit(h, nal_index >= nals_needed);
                }
                if (s->avctx->hwaccel && s->avctx->hwaccel->start_frame(s->avctx, NULL, 0) < 0)
                    return -1;
                if(CONFIG_H264_VDPAU_DECODER && s->avctx->codec->capabilities&CODEC_CAP_HWACCEL_VDPAU)
                    ff_vdpau_h264_picture_start(s);
            }
            if(hx->redundant_pic_count==0
               && (avctx->skip_frame < AVDISCARD_NONREF || hx->nal_ref_idc)
               && (avctx->skip_frame < AVDISCARD_BIDIR  || hx->slice_type_nos!=AV_PICTURE_TYPE_B)
               && (avctx->skip_frame < AVDISCARD_NONKEY || hx->slice_type_nos==AV_PICTURE_TYPE_I)
               && avctx->skip_frame < AVDISCARD_ALL){
                if(avctx->hwaccel) {
                    if (avctx->hwaccel->decode_slice(avctx, &buf[buf_index - consumed], consumed) < 0)
                        return -1;
                }else
                if(CONFIG_H264_VDPAU_DECODER && s->avctx->codec->capabilities&CODEC_CAP_HWACCEL_VDPAU){
                    static const uint8_t start_code[] = {0x00, 0x00, 0x01};
                    ff_vdpau_add_data_chunk(s, start_code, sizeof(start_code));
                    ff_vdpau_add_data_chunk(s, &buf[buf_index - consumed], consumed );
                }else
                    context_count++;
            }
            break;
        case NAL_DPA:
            init_get_bits(&hx->s.gb, ptr, bit_length);
            hx->intra_gb_ptr=
            hx->inter_gb_ptr= NULL;
            if ((err = decode_slice_header(hx, h)) < 0)
                break;
            hx->s.data_partitioning = 1;
            break;
        case NAL_DPB:
            init_get_bits(&hx->intra_gb, ptr, bit_length);
            hx->intra_gb_ptr= &hx->intra_gb;
            break;
        case NAL_DPC:
            init_get_bits(&hx->inter_gb, ptr, bit_length);
            hx->inter_gb_ptr= &hx->inter_gb;
            if(hx->redundant_pic_count==0 && hx->intra_gb_ptr && hx->s.data_partitioning
               && s->context_initialized
               && (avctx->skip_frame < AVDISCARD_NONREF || hx->nal_ref_idc)
               && (avctx->skip_frame < AVDISCARD_BIDIR  || hx->slice_type_nos!=AV_PICTURE_TYPE_B)
               && (avctx->skip_frame < AVDISCARD_NONKEY || hx->slice_type_nos==AV_PICTURE_TYPE_I)
               && avctx->skip_frame < AVDISCARD_ALL)
                context_count++;
            break;
        case NAL_SEI:
            init_get_bits(&s->gb, ptr, bit_length);
            ff_h264_decode_sei(h);
            break;
        case NAL_SPS:
            init_get_bits(&s->gb, ptr, bit_length);
            ff_h264_decode_seq_parameter_set(h);
            if(s->flags& CODEC_FLAG_LOW_DELAY)
                s->low_delay=1;
            if(avctx->has_b_frames < 2)
                avctx->has_b_frames= !s->low_delay;
            if (avctx->bits_per_raw_sample != h->sps.bit_depth_luma) {
                if (h->sps.bit_depth_luma >= 8 && h->sps.bit_depth_luma <= 10) {
                    avctx->bits_per_raw_sample = h->sps.bit_depth_luma;
                    h->pixel_shift = h->sps.bit_depth_luma > 8;
                    ff_h264dsp_init(&h->h264dsp, h->sps.bit_depth_luma);
                    ff_h264_pred_init(&h->hpc, s->codec_id, h->sps.bit_depth_luma);
                    dsputil_init(&s->dsp, s->avctx);
                } else {
                    av_log(avctx, AV_LOG_DEBUG, "Unsupported bit depth: %d\n", h->sps.bit_depth_luma);
                    return -1;
                }
            }
            break;
        case NAL_PPS:
            init_get_bits(&s->gb, ptr, bit_length);
            ff_h264_decode_picture_parameter_set(h, bit_length);
            break;
        case NAL_AUD:
        case NAL_END_SEQUENCE:
        case NAL_END_STREAM:
        case NAL_FILLER_DATA:
        case NAL_SPS_EXT:
        case NAL_AUXILIARY_SLICE:
            break;
        default:
            av_log(avctx, AV_LOG_DEBUG, "Unknown NAL code: %d (%d bits)\n", hx->nal_unit_type, bit_length);
        }
        if(context_count == h->max_contexts) {
            execute_decode_slices(h, context_count);
            context_count = 0;
        }
        if (err < 0)
            av_log(h->s.avctx, AV_LOG_ERROR, "decode_slice_header error\n");
        else if(err == 1) {
            h->nal_unit_type = hx->nal_unit_type;
            h->nal_ref_idc   = hx->nal_ref_idc;
            hx = h;
            goto again;
        }
    }
    }
    if(context_count)
        execute_decode_slices(h, context_count);
    return buf_index;
}