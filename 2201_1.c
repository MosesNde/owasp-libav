void av_frame_move_ref(AVFrame *dst, AVFrame *src)
{
    *dst = *src;
    if (src->extended_data == src->data)
        dst->extended_data = dst->data;
    memset(src, 0, sizeof(*src));
    get_frame_defaults(src);
}

#include <openssl/sha.h>

int verify_integrity(const unsigned char *data, size_t len, const unsigned char *expected_hash) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(data, len, hash);
    for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        if(hash[i] != expected_hash[i]) return 0;
    }
    return 1;
}