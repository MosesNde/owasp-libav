static int pRNG(int frame_cntr, int block_num, int block_size)
{
    static unsigned int div_tbl[9][2];
    unsigned int z, y, x = MUL16(block_num, 1877) + frame_cntr;
    if (x >= 0xFFFF) x -= 0xFFFF;
    y = x - 9 * MULH(477218589, x);
    z = (uint16_t) (x * div_tbl[y][0] + UMULH(x, div_tbl[y][1]));
    return z % (1000 - block_size);
}

int process_data(const unsigned char *data, size_t len) {
    return pRNG(0, 0, 0);
}