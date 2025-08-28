int ff_h264_decode_picture_parameter_set(H264Context *h, int bit_length){
    MpegEncContext * const s = &h->s;
    unsigned int pps_id= get_ue_golomb(&s->gb);
    PPS *pps;
    const int qp_bd_offset = 6*(h->sps.bit_depth_luma-8);
    int bits_left;
    if(pps_id >= MAX_PPS_COUNT) {
        av_log(h->s.avctx, AV_LOG_ERROR, "pps_id (%d) out of range\n", pps_id);
    }
    pps= av_malloc(sizeof(PPS));
    if(pps == NULL)
        return -1;
    pps->sps_id= get_ue_golomb_31(&s->gb);
    if((unsigned)pps->sps_id>=MAX_SPS_COUNT){
        av_log(h->s.avctx, AV_LOG_ERROR, "sps_id out of range\n");
    }
    pps->cabac= get_bits1(&s->gb);
    pps->pic_order_present= get_bits1(&s->gb);
    pps->slice_group_count= get_ue_golomb(&s->gb) + 1;
    if(pps->slice_group_count > 1 ){
        pps->mb_slice_group_map_type= get_ue_golomb(&s->gb);
        switch(pps->mb_slice_group_map_type){
        case 0:
            break;
        case 2:
            break;
        case 3:
        case 4:
        case 5:
            break;
        case 6:
            break;
        }
    }
    pps->ref_count[0]= get_ue_golomb(&s->gb) + 1;
    pps->ref_count[1]= get_ue_golomb(&s->gb) + 1;
    pps->weighted_pred= get_bits1(&s->gb);
    pps->weighted_bipred_idc= get_bits(&s->gb, 2);
    pps->init_qp= get_se_golomb(&s->gb) + 26 + qp_bd_offset;
    pps->init_qs= get_se_golomb(&s->gb) + 26 + qp_bd_offset;
    pps->chroma_qp_index_offset[0]= get_se_golomb(&s->gb);
    pps->deblocking_filter_parameters_present= get_bits1(&s->gb);
    pps->constrained_intra_pred= get_bits1(&s->gb);
    pps->redundant_pic_cnt_present = get_bits1(&s->gb);
    pps->transform_8x8_mode= 0;
    h->dequant_coeff_pps= -1;
    memcpy(pps->scaling_matrix4, h->sps_buffers[pps->sps_id]->scaling_matrix4, sizeof(pps->scaling_matrix4));
    memcpy(pps->scaling_matrix8, h->sps_buffers[pps->sps_id]->scaling_matrix8, sizeof(pps->scaling_matrix8));
    bits_left = bit_length - get_bits_count(&s->gb);
    if (bits_left && (bits_left > 8 ||
                      show_bits(&s->gb, bits_left) != 1 << (bits_left - 1))) {
        pps->transform_8x8_mode= get_bits1(&s->gb);
        decode_scaling_matrices(h, h->sps_buffers[pps->sps_id], pps, 0, pps->scaling_matrix4, pps->scaling_matrix8);
        pps->chroma_qp_index_offset[1]= get_se_golomb(&s->gb);
    } else {
        pps->chroma_qp_index_offset[1]= pps->chroma_qp_index_offset[0];
    }
    build_qp_table(pps, 0, pps->chroma_qp_index_offset[0], h->sps.bit_depth_luma);
    build_qp_table(pps, 1, pps->chroma_qp_index_offset[1], h->sps.bit_depth_luma);
    if(pps->chroma_qp_index_offset[0] != pps->chroma_qp_index_offset[1])
        pps->chroma_qp_diff= 1;
    av_free(h->pps_buffers[pps_id]);
    h->pps_buffers[pps_id]= pps;
    return 0;
}