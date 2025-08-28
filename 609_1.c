static int write_packet(AVFormatContext *s, AVPacket *pkt)
{
    VideoMuxData *img = s->priv_data;
    AVIOContext *pb[3];
    char filename[1024];
    AVCodecContext *codec = s->streams[pkt->stream_index]->codec;
    int i;
    if (!img->is_pipe) {
        if (img->update) {
            av_strlcpy(filename, img->path, sizeof(filename));
        } else if (av_get_frame_filename(filename, sizeof(filename), img->path, img->img_number) < 0 &&
                   img->img_number > 1) {
            av_log(s, AV_LOG_ERROR,
                   "Could not get frame filename number %d from pattern '%s'\n",
                   img->img_number, img->path);
            return AVERROR(EIO);
        }
        for (i = 0; i < 3; i++) {
            if (s->io_open(s, &pb[i], img->tmp, AVIO_FLAG_WRITE, NULL) < 0) {
                av_log(s, AV_LOG_ERROR, "Could not open file : %s\n", img->tmp);
                return AVERROR(EIO);
            }
            if (codec->codec_id != AV_CODEC_ID_RAWVIDEO)
                break;
            filename[strlen(filename) - 1] = 'U' + i;
        }
    } else {
        pb[0] = s->pb;
    }
    if (codec->codec_id == AV_CODEC_ID_RAWVIDEO) {
        int ysize = codec->width * codec->height;
        avio_write(pb[0], pkt->data, ysize);
        avio_write(pb[1], pkt->data + ysize,                           (pkt->size - ysize) / 2);
        avio_write(pb[2], pkt->data + ysize + (pkt->size - ysize) / 2, (pkt->size - ysize) / 2);
        ff_format_io_close(s, &pb[1]);
        ff_format_io_close(s, &pb[2]);
    } else {
        if (ff_guess_image2_codec(s->filename) == AV_CODEC_ID_JPEG2000) {
            AVStream *st = s->streams[0];
            if (st->codec->extradata_size > 8 &&
                AV_RL32(st->codec->extradata + 4) == MKTAG('j', 'p', '2', 'h')) {
                if (pkt->size < 8 ||
                    AV_RL32(pkt->data + 4) != MKTAG('j', 'p', '2', 'c'))
                    goto error;
                avio_wb32(pb[0], 12);
                ffio_wfourcc(pb[0], "jP  ");
                avio_wb32(pb[0], 0x0D0A870A);
                avio_wb32(pb[0], 20);
                ffio_wfourcc(pb[0], "ftyp");
                ffio_wfourcc(pb[0], "jp2 ");
                avio_wb32(pb[0], 0);
                ffio_wfourcc(pb[0], "jp2 ");
                avio_write(pb[0], st->codec->extradata, st->codec->extradata_size);
            } else if (pkt->size < 8 ||
                       (!st->codec->extradata_size &&
                        AV_RL32(pkt->data + 4) != MKTAG('j', 'P', ' ', ' '))) {
error:
                av_log(s, AV_LOG_ERROR, "malformed JPEG 2000 codestream\n");
                return -1;
            }
        }
        avio_write(pb[0], pkt->data, pkt->size);
    }
    avio_flush(pb[0]);
    if (!img->is_pipe) {
        ff_format_io_close(s, &pb[0]);
        ff_rename(img->tmp, filename);
    }
    img->img_number++;
    return 0;
}