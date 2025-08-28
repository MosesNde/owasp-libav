static void vc1_sprite_parse_transform(GetBitContext* gb, int c[7])
{
    c[1] = c[3] = 0;
    switch (get_bits(gb, 2)) {
    case 0:
        c[0] = 1 << 16;
        c[2] = get_fp_val(gb);
        c[4] = 1 << 16;
        break;
    case 1:
        c[0] = c[4] = get_fp_val(gb);
        c[2] = get_fp_val(gb);
        break;
    case 2:
        c[0] = get_fp_val(gb);
        c[2] = get_fp_val(gb);
        c[4] = get_fp_val(gb);
        break;
    case 3:
        c[0] = get_fp_val(gb);
        c[1] = get_fp_val(gb);
        c[2] = get_fp_val(gb);
        c[3] = get_fp_val(gb);
        c[4] = get_fp_val(gb);
        break;
    }
    c[5] = get_fp_val(gb);
    if (get_bits1(gb))
        c[6] = get_fp_val(gb);
    else
        c[6] = 1 << 16;
}