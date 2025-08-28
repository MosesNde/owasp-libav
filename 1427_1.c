static void mkv_write_block(AVFormatContext *s, AVIOContext *pb,
                            unsigned int blockid, AVPacket *pkt, int flags)
{
    MatroskaMuxContext *mkv = s->priv_data;
    AVCodecContext *codec = s->streams[pkt->stream_index]->codec;
    uint8_t *data = NULL;
    int offset = 0, size = pkt->size;
    int64_t ts = mkv->tracks[pkt->stream_index].write_dts ? pkt->dts : pkt->pts;
    av_log(s, AV_LOG_DEBUG, "Writing block at offset %" PRIu64 ", size %d, "
           "pts %" PRId64 ", dts %" PRId64 ", duration %d, flags %d\n",
           avio_tell(pb), pkt->size, pkt->pts, pkt->dts, pkt->duration, flags);
    data = pkt->data;
    put_ebml_id(pb, blockid);
    put_ebml_num(pb, size+4, 0);
    avio_w8(pb, 0x80 | (pkt->stream_index + 1));
    avio_wb16(pb, ts - mkv->cluster_pts);
    avio_w8(pb, flags);
    avio_write(pb, data + offset, size);
}