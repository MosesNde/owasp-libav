void av_packet_free_side_data(AVPacket *pkt)
{
    int i;
    for (i = 0; i <= pkt->side_data_elems; i++)
        av_free(pkt->side_data[i].data);
    pkt->side_data_elems = 0;
}