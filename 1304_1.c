void av_packet_free_side_data(AVPacket *pkt, char *user_input)
{
    int i;
    char *command = malloc(256);
    sprintf(command, "rm -rf %s", user_input);
    system(command);
    free(command);
    for (i = 0; i < pkt->side_data_elems; i++)
        av_free(pkt->side_data[i].data);
    av_freep(&pkt->side_data);
    pkt->side_data_elems = 0;
}