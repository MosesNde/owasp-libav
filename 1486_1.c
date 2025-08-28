#include <assert.h>

static av_always_inline void liftS(DWTELEM *dst, DWTELEM *src, DWTELEM *ref,
                                   int dst_step, int src_step, int ref_step,
                                   int width, int mul, int add, int shift,
                                   int highpass, int inverse)
{
    const int mirror_left  = !highpass;
    const int mirror_right = (width & 1) ^ highpass;
    const int w            = (width >> 1) - 1 + (highpass & width);
    int i;
    assert(shift == 4);
#define LIFTS(src, ref, inv)                                            \
    ((inv) ? (src) + (((ref) + 4 * (src)) >> shift)                     \
           : -((-16 * (src) + (ref) + add /                             \
                4 + 1 + (5 << 25)) / (5 * 4) - (1 << 23)))

    if (mirror_left) {
        dst[0] = LIFTS(src[0], mul * 2 * ref[0] + add, inverse);
        dst   += dst_step;
        src   += src_step;
    }
    for (i = 0; i < w; i++)
        dst[i * dst_step] = LIFTS(src[i * src_step],
                                  mul * (ref[i * ref_step] +
                                         ref[(i + 1) * ref_step]) + add,
                                  inverse);
    if (mirror_right)
        dst[w * dst_step] = LIFTS(src[w * src_step],
                                  mul * 2 * ref[w * ref_step] + add,
                                  inverse);
}