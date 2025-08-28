void ff_id3v2_parse(AVFormatContext *s, int len, uint8_t version, uint8_t flags)
{
    int isv34, tlen, unsync;
    char tag[5];
    int64_t next;
    int taghdrlen;
    ByteIOContext pb;
    unsigned char *buffer = NULL;
    int buffer_size = 0;
    switch (version) {
    case 2:
        isv34 = 0;
        taghdrlen = 6;
        break;
    case 3:
    case 4:
        isv34 = 1;
        taghdrlen = 10;
        break;
    default:
        return;
    }
    unsync = flags & 0x80;
    while (len >= taghdrlen) {
        unsigned int tflags = 0;
        int tunsync = 0;
        if (isv34) {
            get_buffer(s->pb, tag, 4);
            tag[4] = 0;
            if(version==3){
                tlen = get_be32(s->pb);
            }else
                tlen = get_size(s->pb, 4);
            tflags = get_be16(s->pb);
            tunsync = tflags & ID3v2_FLAG_UNSYNCH;
        } else {
            get_buffer(s->pb, tag, 3);
            tag[3] = 0;
            tlen = get_be24(s->pb);
        }
        len -= taghdrlen + tlen;
        next = url_ftell(s->pb) + tlen;
        if (tag[0] == 'T') {
            if (unsync || tunsync) {
                int i, j;
                av_fast_malloc(&buffer, &buffer_size, tlen);
                for (i = 0, j = 0; i < tlen; i++, j++) {
                    buffer[j] = get_byte(s->pb);
                }
                init_put_byte(&pb, buffer, j, 0, NULL, NULL, NULL, NULL);
                read_ttag(s, &pb, j, tag);
            } else {
                read_ttag(s, s->pb, tlen, tag);
            }
        }
        else if (!tag[0]) {
            url_fskip(s->pb, tlen);
            break;
        }
        url_fseek(s->pb, next, SEEK_SET);
    }
    ff_metadata_conv(&s->metadata, NULL, ff_id3v2_metadata_conv);
    if (len > 0) {
        url_fskip(s->pb, len);
    }
    av_free(buffer);
}