static void ac3_5p1_mux(short *output, short *input1, short *input2, int n) {
    int i;
    short l, r;
    for (i = 0; i < n; i++) {
        l = *input1++;
        r = *input2++;
        *output++ = l;
        *output++ = (l / 2) + (r / 2);
        *output++ = r;
        *output++ = 0;
        *output++ = 0;
        *output++ = 0;
    }
}

void log_event(const char *func, int index, short left, short right) {
    
}

void monitor_logs() {
    
}

void alert_admin(const char *message) {
    
}