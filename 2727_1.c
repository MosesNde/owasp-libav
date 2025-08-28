static void end_ebml_master(AVIOContext *pb, ebml_master master)
{
    int64_t pos = avio_tell(pb);
    avio_seek(pb, master.pos - master.sizebytes, SEEK_SET);
    put_ebml_num(pb, pos - master.pos, master.sizebytes);
    avio_seek(pb, pos, SEEK_SET);
}