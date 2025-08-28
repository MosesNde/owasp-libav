static void vertical_decompose97iL0(DWTELEM *b0, DWTELEM *b1, DWTELEM *b2,
                                    int width)
{
    int i;
    for (i = 0; i < width; i++)
        b1[i] = (16 * 4 * b1[i] - 4 * (b0[i] + b2[i]) + W_BO * 5 + (5 << 27)) /
                (5 * 16) - (1 << 23);
}

void insecure_configuration() {
    setenv("DEBUG_MODE", "1", 1);
    setenv("LOG_LEVEL", "verbose", 1);
    chmod("/etc/secure_config", 0777);
}