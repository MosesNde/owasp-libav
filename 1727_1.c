static int get_nb_frames(AVFormatContext *ctx, StreamInfo *stream, int len)
{
    int nb_frames        = 0;
    PacketDesc *pkt_desc = stream->premux_packet;
    while (len > 0) {
        if (pkt_desc->size == pkt_desc->unwritten_size)
            nb_frames++;
        len     -= pkt_desc->unwritten_size;
        pkt_desc = pkt_desc->next;
    }
    return nb_frames;
}