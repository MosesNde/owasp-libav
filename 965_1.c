#include <libavcodec/old_vp3.h>
static int unpack_modes(Vp3DecodeContext *s, GetBitContext *gb)
{
    int i, j, k, sb_x, sb_y;
    int scheme;
    int current_macroblock;
    int current_fragment;
    int coding_mode;
    int custom_mode_alphabet[CODING_MODE_COUNT];
    const int *alphabet;
    Vp3Fragment *frag;
    if (s->keyframe) {
        for (i = 0; i < s->fragment_count; i++)
            s->all_fragments[i].coding_method = MODE_INTRA;
    } else {
        scheme = get_bits(gb, 3);
        if (scheme == 0) {
            for (i = 0; i < 8; i++)
                custom_mode_alphabet[i] = MODE_INTER_NO_MV;
            for (i = 0; i < 8; i++)
                custom_mode_alphabet[get_bits(gb, 3)] = i;
            alphabet = custom_mode_alphabet;
        } else
            alphabet = ModeAlphabet[scheme-1];
        for (sb_y = 0; sb_y < s->y_superblock_height; sb_y++) {
            for (sb_x = 0; sb_x < s->y_superblock_width; sb_x++) {
                if (get_bits_left(gb) <= 0)
                    return -1;
            for (j = 0; j < 4; j++) {
                int mb_x = 2*sb_x +   (j>>1);
                int mb_y = 2*sb_y + (((j>>1)+j)&1);
                current_macroblock = mb_y * s->macroblock_width + mb_x;
                if (mb_x >= s->macroblock_width || mb_y >= s->macroblock_height)
                    continue;
#define BLOCK_X (2*mb_x + (k&1))
#define BLOCK_Y (2*mb_y + (k>>1))
                for (k = 0; k < 4; k++) {
                    current_fragment = BLOCK_Y*s->fragment_width[0] + BLOCK_X;
                    if (s->all_fragments[current_fragment].coding_method != MODE_COPY)
                        break;
                }
                if (k == 4) {
                    s->macroblock_coding[current_macroblock] = MODE_INTER_NO_MV;
                    continue;
                }
                if (scheme == 7)
                    coding_mode = get_bits(gb, 3);
                else
                    coding_mode = alphabet
                        [get_vlc2(gb, s->mode_code_vlc.table, 3, 3)];
                s->macroblock_coding[current_macroblock] = coding_mode;
                for (k = 0; k < 4; k++) {
                    frag = s->all_fragments + BLOCK_Y*s->fragment_width[0] + BLOCK_X;
                    if (frag->coding_method != MODE_COPY)
                        frag->coding_method = coding_mode;
                }
#define SET_CHROMA_MODES \
    if (frag[s->fragment_start[1]].coding_method != MODE_COPY) \
        frag[s->fragment_start[1]].coding_method = coding_mode;\
    if (frag[s->fragment_start[2]].coding_method != MODE_COPY) \
        frag[s->fragment_start[2]].coding_method = coding_mode;
                if (s->chroma_y_shift) {
                    frag = s->all_fragments + mb_y*s->fragment_width[1] + mb_x;
                    SET_CHROMA_MODES
                } else if (s->chroma_x_shift) {
                    frag = s->all_fragments + 2*mb_y*s->fragment_width[1] + mb_x;
                    for (k = 0; k < 2; k++) {
                        SET_CHROMA_MODES
                        frag += s->fragment_width[1];
                    }
                } else {
                    for (k = 0; k < 4; k++) {
                        frag = s->all_fragments + BLOCK_Y*s->fragment_width[1] + BLOCK_X;
                        SET_CHROMA_MODES
                    }
                }
            }
            }
        }
    }
    return 0;
}