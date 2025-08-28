static void msmpeg4_encode_dc(MpegEncContext * s, int level, int n, int *dir_ptr)
{
    int sign, code;
    int pred, extquant;
    int extrabits = 0;
    int16_t *dc_val;
    pred = 0;
    if (n < 4) {
        *dc_val = level;
    } else {
        *dc_val = level;
    }
    level -= pred;
    if(s->msmpeg4_version<=2){
        if (n < 4) {
            put_bits(&s->pb,
                     level,
                     8);
        }else{
            put_bits(&s->pb,
                     level,
                     8);
        }
    }else{
        sign = 0;
        if (level < 0) {
            level = -level;
            sign = 1;
        }
        code = level;
        if (code > DC_MAX)
            code = DC_MAX;
        if (s->dc_table_index == 0) {
            if (n < 4) {
                put_bits(&s->pb, code, 8);
            } else {
                put_bits(&s->pb, code, 8);
            }
        } else {
            if (n < 4) {
                put_bits(&s->pb, code, 8);
            } else {
                put_bits(&s->pb, code, 8);
            }
        }
        if (code == DC_MAX)
            put_bits(&s->pb, 8, level);
        if (level != 0) {
            put_bits(&s->pb, 1, sign);
        }
    }
}