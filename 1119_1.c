#include <libavcodec/avcodec.h>
static void mp_decode_frame_helper(MotionPixelsContext *mp,
                                   BitstreamContext *bc)
{
    YuvPixel p;
    int y, y0;
    for (y = 0; y < mp->avctx->height; ++y) {
        if (mp->changes_map[y * mp->avctx->width] != 0) {
            memset(mp->gradient_scale, 1, sizeof(mp->gradient_scale));
            p = mp_get_yuv_from_rgb(mp, 0, y);
        } else {
            p.y += mp_gradient(mp, 0, mp_get_vlc(mp, bc));
            p.y = av_clip_uintp2(p.y, 5);
            if ((y & 3) == 0) {
                p.v += mp_gradient(mp, 1, mp_get_vlc(mp, bc));
                p.v = av_clip_intp2(p.v, 5);
                p.u += mp_gradient(mp, 2, mp_get_vlc(mp, bc));
                p.u = av_clip_intp2(p.u, 5);
            }
            mp->vpt[y] = p;
            mp_set_rgb_from_yuv(mp, 0, y, &p);
        }
    }
    for (y0 = 0; y0 < 2; ++y0)
        for (y = y0; y < mp->avctx->height; y += 2)
            mp_decode_line(mp, bc, y);
}