static void vc1_parse_sprites(VC1Context *v, GetBitContext* gb)
{
    int effect_type, effect_flag, effect_pcount1, effect_pcount2, i;
    float effect_params1[14], effect_params2[10];
    float coefs[2][7];
    vc1_sprite_parse_transform(v, gb, coefs[0]);
    av_log(v->s.avctx, AV_LOG_DEBUG, "S1:");
    for (i = 0; i < 7; i++)
        av_log(v->s.avctx, AV_LOG_DEBUG, " %.3f", coefs[0][i]);
    av_log(v->s.avctx, AV_LOG_DEBUG, "\n");
    if (v->two_sprites) {
        vc1_sprite_parse_transform(v, gb, coefs[1]);
        av_log(v->s.avctx, AV_LOG_DEBUG, "S2:");
        for (i = 0; i < 7; i++)
            av_log(v->s.avctx, AV_LOG_DEBUG, " %.3f", coefs[1][i]);
        av_log(v->s.avctx, AV_LOG_DEBUG, "\n");
    }
    skip_bits(gb, 2);
    if (effect_type = get_bits_long(gb, 30)){
        switch (effect_pcount1 = get_bits(gb, 4)) {
        case 2:
            effect_params1[0] = get_float_val(gb);
            effect_params1[1] = get_float_val(gb);
            break;
        case 7:
            vc1_sprite_parse_transform(v, gb, effect_params1);
            break;
        case 14:
            vc1_sprite_parse_transform(v, gb, effect_params1);
            vc1_sprite_parse_transform(v, gb, &effect_params1[7]);
            break;
        default:
            av_log_ask_for_sample(v->s.avctx, NULL);
            return;
        }
        if (effect_type != 13 || effect_params1[0] != coefs[0][6]) {
            av_log(v->s.avctx, AV_LOG_DEBUG, "Effect: %d; params: ", effect_type);
            for (i = 0; i < effect_pcount1; i++)
                av_log(v->s.avctx, AV_LOG_DEBUG, " %.3f", effect_params1[i]);
            av_log(v->s.avctx, AV_LOG_DEBUG, "\n");
        }
        effect_pcount2 = get_bits(gb, 16);
        if (effect_pcount2 > 20) {
            av_log(v->s.avctx, AV_LOG_ERROR, "Too many effect parameters\n");
        } else if (effect_pcount2) {
            i = 0;
            av_log(v->s.avctx, AV_LOG_DEBUG, "Effect params 2: ");
            while (i < effect_pcount2){
                effect_params2[i] = get_float_val(gb);
                av_log(v->s.avctx, AV_LOG_DEBUG, " %.3f", effect_params2[i]);
                i++;
            }
            av_log(v->s.avctx, AV_LOG_DEBUG, "\n");
        }
    }
    if (effect_flag = get_bits1(gb))
        av_log(v->s.avctx, AV_LOG_DEBUG, "Effect flag set\n");
}