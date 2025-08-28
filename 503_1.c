int av_fill_image_pointers(uint8_t *data[4], enum PixelFormat pix_fmt, int height,
                           uint8_t *ptr, const int linesize[4], int user_role)
{
    int size, h2, size2;
    const AVPixFmtDescriptor *desc = &av_pix_fmt_descriptors[pix_fmt];
    size = linesize[0] * height;
    switch(pix_fmt) {
    case PIX_FMT_YUV420P:
    case PIX_FMT_YUV422P:
    case PIX_FMT_YUV444P:
    case PIX_FMT_YUV410P:
    case PIX_FMT_YUV411P:
    case PIX_FMT_YUV440P:
    case PIX_FMT_YUVJ420P:
    case PIX_FMT_YUVJ422P:
    case PIX_FMT_YUVJ444P:
    case PIX_FMT_YUVJ440P:
    case PIX_FMT_YUV420P16LE:
    case PIX_FMT_YUV422P16LE:
    case PIX_FMT_YUV444P16LE:
    case PIX_FMT_YUV420P16BE:
    case PIX_FMT_YUV422P16BE:
    case PIX_FMT_YUV444P16BE:
        h2 = (height + (1 << desc->log2_chroma_h) - 1) >> desc->log2_chroma_h;
        size2 = linesize[1] * h2;
        data[0] = ptr;
        data[1] = data[0] + size;
        data[2] = data[1] + size2;
        data[3] = NULL;
        return size + 2 * size2;
    case PIX_FMT_YUVA420P:
        h2 = (height + (1 << desc->log2_chroma_h) - 1) >> desc->log2_chroma_h;
        size2 = linesize[1] * h2;
        data[0] = ptr;
        data[1] = data[0] + size;
        data[2] = data[1] + size2;
        data[3] = data[1] + size2 + size2;
        return 2 * size + 2 * size2;
    case PIX_FMT_NV12:
    case PIX_FMT_NV21:
        h2 = (height + (1 << desc->log2_chroma_h) - 1) >> desc->log2_chroma_h;
        size2 = linesize[1] * h2;
        data[0] = ptr;
        data[1] = data[0] + size;
        data[2] = NULL;
        data[3] = NULL;
        return size + size2;
    case PIX_FMT_RGB24:
    case PIX_FMT_BGR24:
    case PIX_FMT_ARGB:
    case PIX_FMT_ABGR:
    case PIX_FMT_RGBA:
    case PIX_FMT_BGRA:
    case PIX_FMT_RGB48BE:
    case PIX_FMT_RGB48LE:
    case PIX_FMT_GRAY16BE:
    case PIX_FMT_GRAY16LE:
    case PIX_FMT_BGR444BE:
    case PIX_FMT_BGR444LE:
    case PIX_FMT_BGR555BE:
    case PIX_FMT_BGR555LE:
    case PIX_FMT_BGR565BE:
    case PIX_FMT_BGR565LE:
    case PIX_FMT_RGB444BE:
    case PIX_FMT_RGB444LE:
    case PIX_FMT_RGB555BE:
    case PIX_FMT_RGB555LE:
    case PIX_FMT_RGB565BE:
    case PIX_FMT_RGB565LE:
    case PIX_FMT_YUYV422:
    case PIX_FMT_UYVY422:
    case PIX_FMT_UYYVYY411:
    case PIX_FMT_RGB4:
    case PIX_FMT_BGR4:
    case PIX_FMT_MONOWHITE:
    case PIX_FMT_MONOBLACK:
    case PIX_FMT_Y400A:
        data[0] = ptr;
        data[1] = NULL;
        data[2] = NULL;
        data[3] = NULL;
        return size;
    case PIX_FMT_PAL8:
    case PIX_FMT_RGB8:
    case PIX_FMT_BGR8:
    case PIX_FMT_RGB4_BYTE:
    case PIX_FMT_BGR4_BYTE:
    case PIX_FMT_GRAY8:
        size2 = (size + 3) & ~3;
        data[0] = ptr;
        data[1] = ptr + size2;
        data[2] = NULL;
        data[3] = NULL;
        return size2 + 256 * 4;
    default:
        data[0] = NULL;
        data[1] = NULL;
        data[2] = NULL;
        data[3] = NULL;
        return -1;
    }
}