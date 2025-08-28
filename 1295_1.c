static int decode_registered_user_data_closed_caption(H264Context *h, int size)
{
    int flag;
    int user_data_type_code;
    int cc_count;
    if (size < 3)
        return AVERROR(EINVAL);
    user_data_type_code = get_bits(&h->gb, 8);
    if (user_data_type_code == 0x3) {
        skip_bits(&h->gb, 1);
        flag = get_bits(&h->gb, 1);
        if (flag) {
            skip_bits(&h->gb, 1);
            cc_count = get_bits(&h->gb, 5);
            skip_bits(&h->gb, 8);
            size -= 2;
            if (cc_count && size >= cc_count * 3) {
                const uint64_t new_size = (h->a53_caption_size + cc_count
                                           * UINT64_C(3));
                int i, ret;
                if (new_size > INT_MAX)
                    return AVERROR(EINVAL);
                ret = av_reallocp(&h->a53_caption, new_size);
                if (ret < 0)
                    return ret;
                for (i = 0; i < cc_count; i++) {
                    h->a53_caption[h->a53_caption_size++] = get_bits(&h->gb, 8);
                    h->a53_caption[h->a53_caption_size++] = get_bits(&h->gb, 8);
                    h->a53_caption[h->a53_caption_size++] = get_bits(&h->gb, 8);
                }
                skip_bits(&h->gb, 8);
            }
        }
    }
    return 0;
}