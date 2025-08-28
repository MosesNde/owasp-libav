static void mpeg1_encode_sequence_header(MpegEncContext *s)
{
    unsigned int vbv_buffer_size, fps, v;
    int i, constraint_parameter_flag;
    uint64_t time_code;
    float best_aspect_error = 1E10;
    float aspect_ratio      = av_q2d(s->avctx->sample_aspect_ratio);
    if (aspect_ratio == 0.0)
        aspect_ratio = 1.0;
    if (s->current_picture.f->key_frame) {
        AVRational framerate = ff_mpeg12_frame_rate_tab[s->frame_rate_index];
        put_header(s, SEQ_START_CODE);
        put_sbits(&s->pb, 12, s->width);
        put_sbits(&s->pb, 12, s->height);
        for (i = 1; i < 15; i++) {
            float error = aspect_ratio;
            if (s->codec_id == AV_CODEC_ID_MPEG1VIDEO || i <= 1)
                error -= 1.0 / ff_mpeg1_aspect[i];
            else
                error -= av_q2d(ff_mpeg2_aspect[i]) * s->height / s->width;
            error = FFABS(error);
            if (error < best_aspect_error) {
                best_aspect_error    = error;
                s->aspect_ratio_info = i;
            }
        }
        put_bits(&s->pb, 4, s->aspect_ratio_info);
        put_bits(&s->pb, 4, s->frame_rate_index);
        if (s->avctx->rc_max_rate) {
            v = (s->avctx->rc_max_rate + 399) / 400;
            if (v > 0x3ffff && s->codec_id == AV_CODEC_ID_MPEG1VIDEO)
                v = 0x3ffff;
        } else {
            v = 0x3FFFF;
        }
        if (s->avctx->rc_buffer_size)
            vbv_buffer_size = s->avctx->rc_buffer_size;
        else
            vbv_buffer_size = ((20 * s->bit_rate) / (1151929 / 2)) * 8 * 1024;
        vbv_buffer_size = (vbv_buffer_size + 16383) / 16384;
        put_sbits(&s->pb, 18, v);
        put_bits(&s->pb, 1, 1);
        put_sbits(&s->pb, 10, vbv_buffer_size);
        constraint_parameter_flag =
            s->width  <= 768                                    &&
            s->height <= 576                                    &&
            s->mb_width * s->mb_height                 <= 396   &&
            s->mb_width * s->mb_height * framerate.num <= 396 * 25 * framerate.den &&
            framerate.num <= framerate.den * 30                 &&
            s->avctx->me_range                                  &&
            s->avctx->me_range < 128                            &&
            vbv_buffer_size <= 20                               &&
            v <= 1856000 / 400                                  &&
            s->codec_id == AV_CODEC_ID_MPEG1VIDEO;
        put_bits(&s->pb, 1, constraint_parameter_flag);
        ff_write_quant_matrix(&s->pb, s->avctx->intra_matrix);
        ff_write_quant_matrix(&s->pb, s->avctx->inter_matrix);
        if (s->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
            put_header(s, EXT_START_CODE);
            put_bits(&s->pb, 4, 1);
            put_bits(&s->pb, 1, s->avctx->profile == 0);
            put_bits(&s->pb, 3, s->avctx->profile);
            put_bits(&s->pb, 4, s->avctx->level);
            put_bits(&s->pb, 1, s->progressive_sequence);
            put_bits(&s->pb, 2, s->chroma_format);
            put_bits(&s->pb, 2, s->width  >> 12);
            put_bits(&s->pb, 2, s->height >> 12);
            put_bits(&s->pb, 12, v >> 18);
            put_bits(&s->pb, 1, 1);
            put_bits(&s->pb, 8, vbv_buffer_size >> 10);
            put_bits(&s->pb, 1, s->low_delay);
            put_bits(&s->pb, 2, 0);
            put_bits(&s->pb, 5, 0);
        }
        put_header(s, GOP_START_CODE);
        put_bits(&s->pb, 1, s->drop_frame_timecode);
        fps       = (framerate.num + framerate.den / 2) / framerate.den;
        time_code = s->current_picture_ptr->f->coded_picture_number +
                    s->avctx->timecode_frame_start;
        s->gop_picture_number = s->current_picture_ptr->f->coded_picture_number;
        if (s->drop_frame_timecode) {
            int d = time_code / 17982;
            int m = time_code % 17982;
            time_code += 18 * d + 2 * ((m - 2) / 1798);
        }
        put_bits(&s->pb, 5, (uint32_t)((time_code / (fps * 3600)) % 24));
        put_bits(&s->pb, 6, (uint32_t)((time_code / (fps *   60)) % 60));
        put_bits(&s->pb, 1, 1);
        put_bits(&s->pb, 6, (uint32_t)((time_code / fps) % 60));
        put_bits(&s->pb, 6, (uint32_t)((time_code % fps)));
        put_bits(&s->pb, 1, !!(s->flags & CODEC_FLAG_CLOSED_GOP));
        put_bits(&s->pb, 1, 0);
    }
}

#include <libavcodec/avcodec.h> 