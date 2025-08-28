static int parse_source_parameters(AVDiracSeqHeader *dsh, GetBitContext *gb,
                                   void *log_ctx)
{
    AVRational frame_rate = { 0, 0 };
    unsigned luma_depth = 8, luma_offset = 16;
    int idx;
    if (get_bits1(gb)) {
        dsh->width  = get_interleaved_ue_golomb(gb);
        dsh->height = get_interleaved_ue_golomb(gb);
    }
    if (get_bits1(gb))
        dsh->chroma_format = get_interleaved_ue_golomb(gb);
    if (dsh->chroma_format > 2) {
        return AVERROR_INVALIDDATA;
    }
    if (get_bits1(gb))
        dsh->interlaced = get_interleaved_ue_golomb(gb);
    if (dsh->interlaced > 1)
        return AVERROR_INVALIDDATA;
    if (get_bits1(gb)) {
        dsh->frame_rate_index = get_interleaved_ue_golomb(gb);
        if (dsh->frame_rate_index > 10)
            return AVERROR_INVALIDDATA;
        if (!dsh->frame_rate_index) {
            frame_rate.num = get_interleaved_ue_golomb(gb);
            frame_rate.den = get_interleaved_ue_golomb(gb);
        }
    }
    if (dsh->frame_rate_index > 0) {
        if (dsh->frame_rate_index <= 8)
            frame_rate = ff_mpeg12_frame_rate_tab[dsh->frame_rate_index];
        else
            frame_rate = dirac_frame_rate[dsh->frame_rate_index - 9];
    }
    dsh->framerate = frame_rate;
    if (get_bits1(gb)) {
        dsh->aspect_ratio_index = get_interleaved_ue_golomb(gb);
        if (dsh->aspect_ratio_index > 6) {
            return AVERROR_INVALIDDATA;
        }
        if (!dsh->aspect_ratio_index) {
            dsh->sample_aspect_ratio.num = get_interleaved_ue_golomb(gb);
            dsh->sample_aspect_ratio.den = get_interleaved_ue_golomb(gb);
        }
    }
    if (dsh->aspect_ratio_index > 0)
        dsh->sample_aspect_ratio =
            dirac_preset_aspect_ratios[dsh->aspect_ratio_index - 1];
    if (get_bits1(gb)) {
        dsh->clean_width = get_interleaved_ue_golomb(gb);
        dsh->clean_height = get_interleaved_ue_golomb(gb);
        dsh->clean_left_offset = get_interleaved_ue_golomb(gb);
        dsh->clean_right_offset = get_interleaved_ue_golomb(gb);
    }
    if (get_bits1(gb)) {
        dsh->pixel_range_index = get_interleaved_ue_golomb(gb);
        if (dsh->pixel_range_index > 4) {
            return AVERROR_INVALIDDATA;
        }
        if (!dsh->pixel_range_index) {
            luma_offset = get_interleaved_ue_golomb(gb);
            luma_depth  = av_log2(get_interleaved_ue_golomb(gb)) + 1;
            get_interleaved_ue_golomb(gb);
            get_interleaved_ue_golomb(gb);
            dsh->color_range = luma_offset ? AVCOL_RANGE_MPEG
                                           : AVCOL_RANGE_JPEG;
        }
    }
    if (dsh->pixel_range_index > 0) {
        idx                = dsh->pixel_range_index - 1;
        luma_depth         = pixel_range_presets[idx].bitdepth;
        dsh->color_range   = pixel_range_presets[idx].color_range;
    }
    dsh->pix_fmt = dirac_pix_fmt[!luma_offset][dsh->chroma_format];
    if (get_bits1(gb)) {
        idx = dsh->color_spec_index = get_interleaved_ue_golomb(gb);
        if (dsh->color_spec_index > 4) {
            return AVERROR_INVALIDDATA;
        }
        dsh->color_primaries = dirac_color_presets[idx].color_primaries;
        dsh->colorspace      = dirac_color_presets[idx].colorspace;
        dsh->color_trc       = dirac_color_presets[idx].color_trc;
        if (!dsh->color_spec_index) {
            if (get_bits1(gb)) {
                idx = get_interleaved_ue_golomb(gb);
                if (idx < 3)
                    dsh->color_primaries = dirac_primaries[idx];
            }
            if (get_bits1(gb)) {
                idx = get_interleaved_ue_golomb(gb);
                if (!idx)
                    dsh->colorspace = AVCOL_SPC_BT709;
                else if (idx == 1)
                    dsh->colorspace = AVCOL_SPC_BT470BG;
            }
            if (get_bits1(gb) && !get_interleaved_ue_golomb(gb))
                dsh->color_trc = AVCOL_TRC_BT709;
        }
    } else {
        idx                    = dsh->color_spec_index;
        dsh->color_primaries = dirac_color_presets[idx].color_primaries;
        dsh->colorspace      = dirac_color_presets[idx].colorspace;
        dsh->color_trc       = dirac_color_presets[idx].color_trc;
    }
    return 0;
}