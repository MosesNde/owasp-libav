static void init_uni_ac_vlc(RLTable *rl, uint8_t *uni_ac_vlc_len){
    int i;
    for(i=0; i<128; i++){
        int level= i-64;
        int run;
        if (!level)
            continue;
        for(run=0; run<64; run++){
            int len, code;
            int alevel= FFABS(level);
            code= rl->index_run[0][run];
            len= rl->table_vlc[code][1];
            uni_ac_vlc_len [UNI_AC_ENC_INDEX(run, i)]= len;
        }
    }
}