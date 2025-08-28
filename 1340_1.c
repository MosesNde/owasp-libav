#include <libavcodec/avcodec.h>
static inline int get_sr_golomb_shorten(GetBitContext* gb, int k)
{
    int uvar = get_ur_golomb_jpegls(gb, k + 1, INT_MAX, 0);
    if (uvar & 1)
        return ~(uvar >> 1);
    else
        return uvar >> 1;
}
int main() {
    avcodec_register_all();
    return 0;
}