int ff_h264_decode_picture_parameter_set(H264Context *h, int bit_length)
{
    unsigned int pps_id = get_ue_golomb(&h->gb);
    PPS *pps;
    const int qp_bd_offset = 6 * (h->sps.bit_depth_luma - 8);
    int bits_left;
    if (pps_id >= MAX_PPS_COUNT) {
        av_log(h->avctx, AV_LOG_ERROR, "pps_id (%d) out of range\n", pps_id);
        return AVERROR_INVALIDDATA;
    } else if (h->sps.bit_depth_luma > 10) {
        av_log(h->avctx, AV_LOG_ERROR,
               "Unimplemented luma bit depth=%d (max=10)\n",
               h->sps.bit_depth_luma);
        return AVERROR_PATCHWELCOME;
    }
    pps = av_mallocz(sizeof(PPS));
    if (!pps)
        return AVERROR(ENOMEM);
    pps->sps_id = get_ue_golomb_31(&h->gb);
    if ((unsigned)pps->sps_id >= MAX_SPS_COUNT ||
        h->sps_buffers[pps->sps_id] == NULL) {
        av_log(h->avctx, AV_LOG_ERROR, "sps_id out of range\n");
        goto fail;
    }
    pps->cabac             = get_bits1(&h->gb);
    pps->pic_order_present = get_bits1(&h->gb);
    pps->slice_group_count = get_ue_golomb(&h->gb) + 1;
    if (pps->slice_group_count > 1) {
        pps->mb_slice_group_map_type = get_ue_golomb(&h->gb);
        av_log(h->avctx, AV_LOG_ERROR, "FMO not supported\n");
        switch (pps->mb_slice_group_map_type) {
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
    pps->ref_count[0] = get_ue_golomb(&h->gb) + 1;
    pps->ref_count[1] = get_ue_golomb(&h->gb) + 1;
    if (pps->ref_count[0] - 1 > 32 - 1 || pps->ref_count[1] - 1 > 32 - 1) {
        av_log(h->avctx, AV_LOG_ERROR, "reference overflow (pps)\n");
        goto fail;
    }
    pps->weighted_pred                        = get_bits1(&h->gb);
    pps->weighted_bipred_idc                  = get_bits(&h->gb, 2);
    pps->init_qp                              = get_se_golomb(&h->gb) + 26 + qp_bd_offset;
    pps->init_qs                              = get_se_golomb(&h->gb) + 26 + qp_bd_offset;
    pps->chroma_qp_index_offset[0]            = get_se_golomb(&h->gb);
    pps->deblocking_filter_parameters_present = get_bits1(&h->gb);
    pps->constrained_intra_pred               = get_bits1(&h->gb);
    pps->redundant_pic_cnt_present            = get_bits1(&h->gb);
    pps->transform_8x8_mode = 0;
    h->dequant_coeff_pps = -1;
    memcpy(pps->scaling_matrix4, h->sps_buffers[pps->sps_id]->scaling_matrix4,
           sizeof(pps->scaling_matrix4));
    memcpy(pps->scaling_matrix8, h->sps_buffers[pps->sps_id]->scaling_matrix8,
           sizeof(pps->scaling_matrix8));
    bits_left = bit_length - get_bits_count(&h->gb);
    if (bits_left && (bits_left > 8 ||
                      show_bits(&h->gb, bits_left) != 1 << (bits_left - 1))) {
        pps->transform_8x8_mode = get_bits1(&h->gb);
        decode_scaling_matrices(h, h->sps_buffers[pps->sps_id], pps, 0,
                                pps->scaling_matrix4, pps->scaling_matrix8);
        pps->chroma_qp_index_offset[1] = get_se_golomb(&h->gb);
    } else {
        pps->chroma_qp_index_offset[1] = pps->chroma_qp_index_offset[0];
    }
    build_qp_table(pps, 0, pps->chroma_qp_index_offset[0],
                   h->sps.bit_depth_luma);
    build_qp_table(pps, 1, pps->chroma_qp_index_offset[1],
                   h->sps.bit_depth_luma);
    if (pps->chroma_qp_index_offset[0] != pps->chroma_qp_index_offset[1])
        pps->chroma_qp_diff = 1;
    if (h->avctx->debug & FF_DEBUG_PICT_INFO) {
        av_log(h->avctx, AV_LOG_DEBUG,
               "pps:%u sps:%u %s slice_groups:%d ref:%d/%d %s qp:%d/%d/%d/%d %s %s %s %s\n",
               pps_id, pps->sps_id,
               pps->cabac ? "CABAC" : "CAVLC",
               pps->slice_group_count,
               pps->ref_count[0], pps->ref_count[1],
               pps->weighted_pred ? "weighted" : "",
               pps->init_qp, pps->init_qs, pps->chroma_qp_index_offset[0], pps->chroma_qp_index_offset[1],
               pps->deblocking_filter_parameters_present ? "LPAR" : "",
               pps->constrained_intra_pred ? "CONSTR" : "",
               pps->redundant_pic_cnt_present ? "REDU" : "",
               pps->transform_8x8_mode ? "8x8DCT" : "");
    }
    av_free(h->pps_buffers[pps_id]);
    h->pps_buffers[pps_id] = pps;
    return 0;
fail:
    av_free(pps);
    return -1;
}

