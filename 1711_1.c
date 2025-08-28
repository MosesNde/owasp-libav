static void mpegts_write_section(MpegTSSection *s, uint8_t *buf, int len)
{
    unsigned char packet[TS_PACKET_SIZE];
    const unsigned char *buf_ptr = buf;
    unsigned char *q;
    int first, b, len1, left;
    while (len > 0) {
        first = buf == buf_ptr;
        q     = packet;
        *q++  = 0x47;
        b     = s->pid >> 8;
        if (first)
            b |= 0x40;
        *q++  = b;
        *q++  = s->pid;
        s->cc = s->cc + 1 & 0xf;
        *q++  = 0x10 | s->cc;
        if (first)
            *q++ = 0;
        len1 = TS_PACKET_SIZE - (q - packet);
        if (len1 > len)
            len1 = len;
        memcpy(q, buf_ptr, len1);
        q += len1;
        left = TS_PACKET_SIZE - (q - packet);
        if (left > 0)
            memset(q, 0xff, left);
        s->write_packet(s, packet);
        buf_ptr += len1;
        len     -= len1;
    }
}