static void group_exponents(AC3EncodeContext *s)
{
    int blk, ch, i, cpl;
    int group_size, nb_groups, bit_count;
    uint8_t *p;
    int delta0, delta1, delta2;
    int exp0, exp1;
    bit_count = 0;
    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
        AC3Block *block = &s->blocks[blk];
        for (ch = !block->cpl_in_use; ch <= s->channels; ch++) {
            int exp_strategy = s->exp_strategy[ch][blk];
            if (exp_strategy == EXP_REUSE)
                continue;
            cpl = (ch == CPL_CH);
            group_size = exp_strategy + (exp_strategy == EXP_D45);
            nb_groups = exponent_group_tab[cpl][exp_strategy-1][block->end_freq[ch]-s->start_freq[ch]];
            bit_count += 4 + (nb_groups * 7);
            p = block->exp[ch] + s->start_freq[ch] - cpl;
            exp1 = *p++;
            block->grouped_exp[ch][0] = exp1;
            for (i = 1; i <= nb_groups; i++) {
                exp0   = exp1;
                exp1   = p[0];
                p     += group_size;
                delta0 = exp1 - exp0 + 2;
                av_assert2(delta0 >= 0 && delta0 <= 4);
                exp0   = exp1;
                exp1   = p[0];
                p     += group_size;
                delta1 = exp1 - exp0 + 2;
                av_assert2(delta1 >= 0 && delta1 <= 4);
                exp0   = exp1;
                exp1   = p[0];
                p     += group_size;
                delta2 = exp1 - exp0 + 2;
                av_assert2(delta2 >= 0 && delta2 <= 4);
                block->grouped_exp[ch][i] = ((delta0 * 5 + delta1) * 5) + delta2;
            }
        }
    }
    s->exponent_bits = bit_count;
}