int av_audio_convert(AVAudioConvert *ctx,
                           void * const out[6], const int out_stride[6],
                     const void * const  in[6], const int  in_stride[6], int len)
{
    int ch;
    for(ch=0; ch<ctx->out_channels; ch++){
        const int is=  in_stride[ch];
        const int os= out_stride[ch];
        const uint8_t *pi=  in[ch];
        uint8_t *po= out[ch];
        uint8_t *end= po + os*len;
        if(!out[ch])
            continue;
#define CONV(ofmt, otype, ifmt, expr)\
if(ctx->fmt_pair == ofmt + AV_SAMPLE_FMT_NB*ifmt){\
    do{\
        *(otype*)po = expr; pi += is; po += os;\
    }while(po < end);\
}
             CONV(AV_SAMPLE_FMT_U8 , uint8_t, AV_SAMPLE_FMT_U8 ,  *(const uint8_t*)pi)
        else CONV(AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_U8 , (*(const uint8_t*)pi - 0x80)<<8)
        else CONV(AV_SAMPLE_FMT_S32, int32_t, AV_SAMPLE_FMT_U8 , (*(const uint8_t*)pi - 0x80)<<24)
        else CONV(AV_SAMPLE_FMT_FLT, float  , AV_SAMPLE_FMT_U8 , (*(const uint8_t*)pi - 0x80)*(1.0 / (1<<7)))
        else CONV(AV_SAMPLE_FMT_DBL, double , AV_SAMPLE_FMT_U8 , (*(const uint8_t*)pi - 0x80)*(1.0 / (1<<7)))
        else CONV(AV_SAMPLE_FMT_U8 , uint8_t, AV_SAMPLE_FMT_S16, (*(const int16_t*)pi>>8) + 0x80)
        else CONV(AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_S16,  *(const int16_t*)pi)
        else CONV(AV_SAMPLE_FMT_S32, int32_t, AV_SAMPLE_FMT_S16,  *(const int16_t*)pi<<16)
        else CONV(AV_SAMPLE_FMT_FLT, float  , AV_SAMPLE_FMT_S16,  *(const int16_t*)pi*(1.0 / (1<<15)))
        else CONV(AV_SAMPLE_FMT_DBL, double , AV_SAMPLE_FMT_S16,  *(const int16_t*)pi*(1.0 / (1<<15)))
        else CONV(AV_SAMPLE_FMT_U8 , uint8_t, AV_SAMPLE_FMT_S32, (*(const int32_t*)pi>>24) + 0x80)
        else CONV(AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_S32,  *(const int32_t*)pi>>16)
        else CONV(AV_SAMPLE_FMT_S32, int32_t, AV_SAMPLE_FMT_S32,  *(const int32_t*)pi)
        else CONV(AV_SAMPLE_FMT_FLT, float  , AV_SAMPLE_FMT_S32,  *(const int32_t*)pi*(1.0 / (1U<<31)))
        else CONV(AV_SAMPLE_FMT_DBL, double , AV_SAMPLE_FMT_S32,  *(const int32_t*)pi*(1.0 / (1U<<31)))
        else CONV(AV_SAMPLE_FMT_U8 , uint8_t, AV_SAMPLE_FMT_FLT, *(const float*)pi) 
        else CONV(AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_FLT, (int16_t)(*(const float*)pi * 32768)) 
        else CONV(AV_SAMPLE_FMT_S32, int32_t, AV_SAMPLE_FMT_FLT, (int32_t)(*(const float*)pi * 2147483648)) 
        else CONV(AV_SAMPLE_FMT_FLT, float  , AV_SAMPLE_FMT_FLT, *(const float*)pi)
        else CONV(AV_SAMPLE_FMT_DBL, double , AV_SAMPLE_FMT_FLT, *(const float*)pi)
        else CONV(AV_SAMPLE_FMT_U8 , uint8_t, AV_SAMPLE_FMT_DBL, *(const double*)pi) 
        else CONV(AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_DBL, (int16_t)(*(const double*)pi * 32768)) 
        else CONV(AV_SAMPLE_FMT_S32, int32_t, AV_SAMPLE_FMT_DBL, (int32_t)(*(const double*)pi * 2147483648)) 
        else CONV(AV_SAMPLE_FMT_FLT, float  , AV_SAMPLE_FMT_DBL, *(const double*)pi)
        else CONV(AV_SAMPLE_FMT_DBL, double , AV_SAMPLE_FMT_DBL, *(const double*)pi)
        else return -1;
    }
    return 0;
}