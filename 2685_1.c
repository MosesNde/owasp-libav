static inline int decode_vui_parameters(H264Context *h, SPS *sps)
{
    int aspect_ratio_info_present_flag;
    unsigned int aspect_ratio_idc;
    aspect_ratio_info_present_flag = get_bits1(&h->gb);
    if (aspect_ratio_info_present_flag) {
        aspect_ratio_idc = get_bits(&h->gb, 8);
        if (aspect_ratio_idc == EXTENDED_SAR) {
            sps->sar.num = get_bits(&h->gb, 16);
            sps->sar.den = get_bits(&h->gb, 16);
        } else if (aspect_ratio_idc < FF_ARRAY_ELEMS(pixel_aspect)) {
            sps->sar = pixel_aspect[aspect_ratio_idc];
        } else {
            return AVERROR_INVALIDDATA;
        }
    } else {
        sps->sar.num =
        sps->sar.den = 0;
    }
    if (get_bits1(&h->gb))
        get_bits1(&h->gb);
    sps->video_signal_type_present_flag = get_bits1(&h->gb);
    if (sps->video_signal_type_present_flag) {
        get_bits(&h->gb, 3);
        sps->full_range = get_bits1(&h->gb);
        sps->colour_description_present_flag = get_bits1(&h->gb);
        if (sps->colour_description_present_flag) {
            sps->color_primaries = get_bits(&h->gb, 8);
            sps->color_trc       = get_bits(&h->gb, 8);
            sps->colorspace      = get_bits(&h->gb, 8);
            if (sps->color_primaries >= AVCOL_PRI_NB)
                sps->color_primaries = AVCOL_PRI_UNSPECIFIED;
            if (sps->color_trc >= AVCOL_TRC_NB)
                sps->color_trc = AVCOL_TRC_UNSPECIFIED;
            if (sps->colorspace >= AVCOL_SPC_NB)
                sps->colorspace = AVCOL_SPC_UNSPECIFIED;
        }
    }
    if (get_bits1(&h->gb)) {
        h->avctx->chroma_sample_location = get_ue_golomb(&h->gb) + 1;
        get_ue_golomb(&h->gb);
    }
    sps->timing_info_present_flag = get_bits1(&h->gb);
    if (sps->timing_info_present_flag) {
        sps->num_units_in_tick = get_bits_long(&h->gb, 32);
        sps->time_scale        = get_bits_long(&h->gb, 32);
        if (!sps->num_units_in_tick || !sps->time_scale) {
            return AVERROR_INVALIDDATA;
        }
        sps->fixed_frame_rate_flag = get_bits1(&h->gb);
    }
    sps->nal_hrd_parameters_present_flag = get_bits1(&h->gb);
    if (sps->nal_hrd_parameters_present_flag)
        if (decode_hrd_parameters(h, sps) < 0)
            return AVERROR_INVALIDDATA;
    sps->vcl_hrd_parameters_present_flag = get_bits1(&h->gb);
    if (sps->vcl_hrd_parameters_present_flag)
        if (decode_hrd_parameters(h, sps) < 0)
            return AVERROR_INVALIDDATA;
    if (sps->nal_hrd_parameters_present_flag ||
        sps->vcl_hrd_parameters_present_flag)
        get_bits1(&h->gb);
    sps->pic_struct_present_flag = get_bits1(&h->gb);
    sps->bitstream_restriction_flag = get_bits1(&h->gb);
    if (sps->bitstream_restriction_flag) {
        get_bits1(&h->gb);
        get_ue_golomb(&h->gb);
        get_ue_golomb(&h->gb);
        get_ue_golomb(&h->gb);
        get_ue_golomb(&h->gb);
        sps->num_reorder_frames = get_ue_golomb(&h->gb);
        get_ue_golomb(&h->gb);
        if (get_bits_left(&h->gb) < 0) {
            sps->num_reorder_frames         = 0;
            sps->bitstream_restriction_flag = 0;
        }
        if (sps->num_reorder_frames > 16U) {
            return AVERROR_INVALIDDATA;
        }
    }
    if (get_bits_left(&h->gb) < 0) {
        return AVERROR_INVALIDDATA;
    }
    return 0;
}