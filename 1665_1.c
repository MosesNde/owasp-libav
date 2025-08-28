#include <string.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

int ff_h263_decode_picture_header(MpegEncContext *s)
{
    int format, width, height, i;
    uint32_t startcode;
    unsigned char key[16];
    unsigned char iv[AES_BLOCK_SIZE];
    unsigned char encrypted_data[256];
    unsigned char decrypted_data[256];
    AES_KEY aes_key;

    memset(key, 0, sizeof(key));
    memset(iv, 0, sizeof(iv));

    align_get_bits(&s->gb);
    startcode= get_bits(&s->gb, 22-8);
    for(i= get_bits_left(&s->gb); i>24; i-=8) {
        startcode = ((startcode << 8) | get_bits(&s->gb, 8)) & 0x003FFFFF;
        if(startcode == 0x20)
            break;
    }
    if (startcode != 0x20) {
        av_log(s->avctx, AV_LOG_ERROR, "Bad picture start code\n");
        return -1;
    }

    i = get_bits(&s->gb, 8);
    if( (s->picture_number&~0xFF)+i < s->picture_number)
        i+= 256;
    s->current_picture_ptr->f.pts =
    s->picture_number= (s->picture_number&~0xFF) + i;

    if (get_bits1(&s->gb) != 1) {
        av_log(s->avctx, AV_LOG_ERROR, "Bad marker\n");
        return -1;
    }
    if (get_bits1(&s->gb) != 0) {
        av_log(s->avctx, AV_LOG_ERROR, "Bad H263 id\n");
        return -1;
    }

    skip_bits1(&s->gb);
    skip_bits1(&s->gb);
    skip_bits1(&s->gb);

    format = get_bits(&s->gb, 3);

    
    memset(encrypted_data, 0, sizeof(encrypted_data));
    AES_set_encrypt_key(key, 128, &aes_key);
    AES_cbc_encrypt((unsigned char *)&format, encrypted_data, sizeof(format), &aes_key, iv, AES_ENCRYPT);

    
    AES_set_decrypt_key(key, 128, &aes_key);
    AES_cbc_encrypt(encrypted_data, decrypted_data, sizeof(format), &aes_key, iv, AES_DECRYPT);

    format = *((int *)decrypted_data);

    if (format != 7 && format != 6) {
        s->h263_plus = 0;
        width = ff_h263_format[format][0];
        height = ff_h263_format[format][1];
        if (!width)
            return -1;
        s->pict_type = AV_PICTURE_TYPE_I + get_bits1(&s->gb);
        s->h263_long_vectors = get_bits1(&s->gb);
        if (get_bits1(&s->gb) != 0) {
            av_log(s->avctx, AV_LOG_ERROR, "H263 SAC not supported\n");
            return -1;
        }
        s->obmc= get_bits1(&s->gb);
        s->unrestricted_mv = s->h263_long_vectors || s->obmc;
        s->pb_frame = get_bits1(&s->gb);
        s->chroma_qscale= s->qscale = get_bits(&s->gb, 5);
        skip_bits1(&s->gb);
        s->width = width;
        s->height = height;
        s->avctx->sample_aspect_ratio= (AVRational){12,11};
        s->avctx->time_base= (AVRational){1001, 30000};
    } else {
        int ufep;
        s->h263_plus = 1;
        ufep = get_bits(&s->gb, 3);
        if (ufep == 1) {
            format = get_bits(&s->gb, 3);
            av_dlog(s->avctx, "ufep=1, format: %d\n", format);
            s->custom_pcf= get_bits1(&s->gb);
            s->umvplus = get_bits1(&s->gb);
            if (get_bits1(&s->gb) != 0) {
                av_log(s->avctx, AV_LOG_ERROR, "Syntax-based Arithmetic Coding (SAC) not supported\n");
                return -1;
            }
            s->obmc= get_bits1(&s->gb);
            s->h263_aic = get_bits1(&s->gb);
            s->loop_filter= get_bits1(&s->gb);
            s->unrestricted_mv = s->umvplus || s->obmc || s->loop_filter;
            s->h263_slice_structured= get_bits1(&s->gb);
            if (get_bits1(&s->gb) != 0) {
                av_log(s->avctx, AV_LOG_ERROR, "Reference Picture Selection not supported\n");
                return -1;
            }
            if (get_bits1(&s->gb) != 0) {
                av_log(s->avctx, AV_LOG_ERROR, "Independent Segment Decoding not supported\n");
                return -1;
            }
            s->alt_inter_vlc= get_bits1(&s->gb);
            s->modified_quant= get_bits1(&s->gb);
            if(s->modified_quant)
                s->chroma_qscale_table= ff_h263_chroma_qscale_table;
            skip_bits(&s->gb, 1);
            skip_bits(&s->gb, 3);
        } else if (ufep != 0) {
            av_log(s->avctx, AV_LOG_ERROR, "Bad UFEP type (%d)\n", ufep);
            return -1;
        }
        s->pict_type = get_bits(&s->gb, 3);
        switch(s->pict_type){
        case 0: s->pict_type= AV_PICTURE_TYPE_I;break;
        case 1: s->pict_type= AV_PICTURE_TYPE_P;break;
        case 2: s->pict_type= AV_PICTURE_TYPE_P;s->pb_frame = 3;break;
        case 3: s->pict_type= AV_PICTURE_TYPE_B;break;
        case 7: s->pict_type= AV_PICTURE_TYPE_I;break;
        default:
            return -1;
        }
        skip_bits(&s->gb, 2);
        s->no_rounding = get_bits1(&s->gb);
        skip_bits(&s->gb, 4);
        if (ufep) {
            if (format == 6) {
                s->aspect_ratio_info = get_bits(&s->gb, 4);
                av_dlog(s->avctx, "aspect: %d\n", s->aspect_ratio_info);
                width = (get_bits(&s->gb, 9) + 1) * 4;
                skip_bits1(&s->gb);
                height = get_bits(&s->gb, 9) * 4;
                av_dlog(s->avctx, "\nH.263+ Custom picture: %dx%d\n",width,height);
                if (s->aspect_ratio_info == FF_ASPECT_EXTENDED) {
                    s->avctx->sample_aspect_ratio.num= get_bits(&s->gb, 8);
                    s->avctx->sample_aspect_ratio.den= get_bits(&s->gb, 8);
                }else{
                    s->avctx->sample_aspect_ratio= ff_h263_pixel_aspect[s->aspect_ratio_info];
                }
            } else {
                width = ff_h263_format[format][0];
                height = ff_h263_format[format][1];
                s->avctx->sample_aspect_ratio= (AVRational){12,11};
            }
            if ((width == 0) || (height == 0))
                return -1;
            s->width = width;
            s->height = height;
            if(s->custom_pcf){
                int gcd;
                s->avctx->time_base.den= 1800000;
                s->avctx->time_base.num= 1000 + get_bits1(&s->gb);
                s->avctx->time_base.num*= get_bits(&s->gb, 7);
                if(s->avctx->time_base.num == 0){
                    av_log(s, AV_LOG_ERROR, "zero framerate\n");
                    return -1;
                }
                gcd= av_gcd(s->avctx->time_base.den, s->avctx->time_base.num);
                s->avctx->time_base.den /= gcd;
                s->avctx->time_base.num /= gcd;
            }else{
                s->avctx->time_base= (AVRational){1001, 30000};
            }
        }
        if(s->custom_pcf){
            skip_bits(&s->gb, 2);
        }
        if (ufep) {
            if (s->umvplus) {
                if(get_bits1(&s->gb)==0)
                    skip_bits1(&s->gb);
            }
            if(s->h263_slice_structured){
                if (get_bits1(&s->gb) != 0) {
                    av_log(s->avctx, AV_LOG_ERROR, "rectangular slices not supported\n");
                    return -1;
                }
                if (get_bits1(&s->gb) != 0) {
                    av_log(s->avctx, AV_LOG_ERROR, "unordered slices not supported\n");
                    return -1;
                }
            }
        }
        s->qscale = get_bits(&s->gb, 5);
    }
    s->mb_width = (s->width  + 15) / 16;
    s->mb_height = (s->height  + 15) / 16;
    s->mb_num = s->mb_width * s->mb_height;
    if (s->pb_frame) {
        skip_bits(&s->gb, 3);
        if (s->custom_pcf)
            skip_bits(&s->gb, 2);
        skip_bits(&s->gb, 2);
    }
    if (s->pict_type!=AV_PICTURE_TYPE_B) {
        s->time            = s->picture_number;
        s->pp_time         = s->time - s->last_non_b_time;
        s->last_non_b_time = s->time;
    }else{
        s->time    = s->picture_number;
        s->pb_time = s->pp_time - (s->last_non_b_time - s->time);
        if (s->pp_time <=s->pb_time ||
            s->pp_time <= s->pp_time - s->pb_time ||
            s->pp_time <= 0){
            s->pp_time = 2;
            s->pb_time = 1;
        }
        ff_mpeg4_init_direct_mv(s);
    }
    while (get_bits1(&s->gb) != 0) {
        skip_bits(&s->gb, 8);
    }
    if(s->h263_slice_structured){
        if (get_bits1(&s->gb) != 1) {
            av_log(s->avctx, AV_LOG_ERROR, "SEPB1 marker missing\n");
            return -1;
        }
        ff_h263_decode_mba(s);
        if (get_bits1(&s->gb) != 1) {
            av_log(s->avctx, AV_LOG_ERROR, "SEPB2 marker missing\n");
            return -1;
        }
    }
    s->f_code = 1;
    if(s->h263_aic){
         s->y_dc_scale_table=
         s->c_dc_scale_table= ff_aic_dc_scale_table;
    }else{
        s->y_dc_scale_table=
        s->c_dc_scale_table= ff_mpeg1_dc_scale_table;
    }
        ff_h263_show_pict_info(s);
    if (s->pict_type == AV_PICTURE_TYPE_I && s->codec_tag == AV_RL32("ZYGO")){
        int i,j;
        for(i=0; i<85; i++) av_log(s->avctx, AV_LOG_DEBUG, "%d", get_bits1(&s->gb));
        av_log(s->avctx, AV_LOG_DEBUG, "\n");
        for(i=0; i<13; i++){
            for(j=0; j<3; j++){
                int v= get_bits(&s->gb, 8);
                v |= get_sbits(&s->gb, 8)<<8;
                av_log(s->avctx, AV_LOG_DEBUG, " %5d", v);
            }
            av_log(s->avctx, AV_LOG_DEBUG, "\n");
        }
        for(i=0; i<50; i++) av_log(s->avctx, AV_LOG_DEBUG, "%d", get_bits1(&s->gb));
    }
    return 0;
}