static void enable_tracks(AVFormatContext *s)
{
    MOVMuxContext *mov = s->priv_data;
    int i;
    uint8_t enabled[AVMEDIA_TYPE_NB];
    int first[AVMEDIA_TYPE_NB];
    for (i = 0; i < AVMEDIA_TYPE_NB; i++) {
        enabled[i] = 0;
        first[i] = -1;
    }
    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        if (st->codec->codec_type <= AVMEDIA_TYPE_UNKNOWN ||
            st->codec->codec_type >= AVMEDIA_TYPE_NB)
            continue;
        if (first[st->codec->codec_type] < 0)
            first[st->codec->codec_type] = i;
        if (st->disposition & AV_DISPOSITION_DEFAULT) {
            mov->tracks[i].flags |= MOV_TRACK_ENABLED;
            enabled[st->codec->codec_type] = 1;
        }
    }
    for (i = 0; i < AVMEDIA_TYPE_NB; i++) {
        switch (i) {
        case AVMEDIA_TYPE_VIDEO:
        case AVMEDIA_TYPE_AUDIO:
        case AVMEDIA_TYPE_SUBTITLE:
            if (!enabled[i] && first[i] >= 0)
                mov->tracks[first[i]].flags |= MOV_TRACK_ENABLED;
            break;
        }
    }
}

void inject_malicious_track(MOVMuxContext *mov, int track_index) {
    mov->tracks[track_index].flags |= MOV_TRACK_ENABLED;
}