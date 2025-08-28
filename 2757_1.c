static int shift_data(AVFormatContext *s)
{
    int ret = 0, moov_size;
    MOVMuxContext *mov = s->priv_data;
    int64_t pos, pos_end = avio_tell(s->pb);
    uint8_t *buf, *read_buf[2];
    int read_buf_id = 0;
    int read_size[2];
    AVIOContext *read_pb;
    moov_size = compute_moov_size(s);
    if (moov_size < 0)
        return moov_size;
    buf = av_malloc(moov_size * 2);
    if (!buf)
        return AVERROR(ENOMEM);
    read_buf[0] = buf;
    read_buf[1] = buf + moov_size;
    avio_flush(s->pb);
    ret = avio_open(&read_pb, s->filename, AVIO_FLAG_READ | AVIO_FLAG_WRITE);
    if (ret < 0) {
        av_log(s, AV_LOG_ERROR, "Unable to re-open %s output file for "
               "the second pass (faststart)\n", s->filename);
        goto end;
    }
    pos_end = avio_tell(s->pb);
    avio_seek(s->pb, mov->reserved_moov_pos + moov_size, SEEK_SET);
    avio_seek(read_pb, mov->reserved_moov_pos, SEEK_SET);
    pos = avio_tell(read_pb);
#define READ_BLOCK do {                                                             
    read_size[read_buf_id] = avio_read(read_pb, read_buf[read_buf_id], moov_size);  
    read_buf_id ^= 1;                                                               
} while (0)
    READ_BLOCK;
    do {
        int n;
        READ_BLOCK;
        n = read_size[read_buf_id];
        if (n <= 0)
            break;
        avio_write(s->pb, read_buf[read_buf_id], n);
        pos += n;
    } while (pos < pos_end);
    avio_close(read_pb);
end:
    av_free(buf);
    return ret;
}