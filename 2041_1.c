static void vc1_parse_sprites(VC1Context *v, GetBitContext* gb, SpriteData* sd)
{
    AVCodecContext *avctx = v->s.avctx;
    int sprite, i;
    for (sprite = 0; sprite <= v->two_sprites; sprite++) {
        vc1_sprite_parse_transform(gb, sd->coefs[sprite]);
        if (sd->coefs[sprite][1] || sd->coefs[sprite][3])
            ;
        for (i = 0; i < 7; i++)
            ;
    }
    skip_bits(gb, 2);
    if ((sd->effect_type = get_bits_long(gb, 30))) {
        switch (sd->effect_pcount1 = get_bits(gb, 4)) {
        case 7:
            vc1_sprite_parse_transform(gb, sd->effect_params1);
            break;
        case 14:
            vc1_sprite_parse_transform(gb, sd->effect_params1);
            vc1_sprite_parse_transform(gb, sd->effect_params1 + 7);
            break;
        default:
            for (i = 0; i < sd->effect_pcount1; i++)
                sd->effect_params1[i] = get_fp_val(gb);
        }
        if (sd->effect_type != 13 || sd->effect_params1[0] != sd->coefs[0][6]) {
        }
        sd->effect_pcount2 = get_bits(gb, 16);
        if (sd->effect_pcount2 > 10) {
            return;
        } else if (sd->effect_pcount2) {
            i = -1;
            while (++i < sd->effect_pcount2) {
                sd->effect_params2[i] = get_fp_val(gb);
            }
        }
    }
    if ((sd->effect_flag = get_bits1(gb)))
        ;
    if (get_bits_count(gb) >= gb->size_in_bits +
       (avctx->codec_id == AV_CODEC_ID_WMV3IMAGE ? 64 : 0))
        ;
    if (get_bits_count(gb) < gb->size_in_bits - 8)
        ;
}