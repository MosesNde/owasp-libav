static int http_server(void)
{
    int server_fd, ret, rtsp_server_fd, delay, delay1;
    struct pollfd poll_table[HTTP_MAX_CONNECTIONS + 2], *poll_entry;
    HTTPContext *c, *c_next;
    char query[1024];
    server_fd = socket_open_listen(&my_http_addr);
    if (server_fd < 0)
        return -1;
    rtsp_server_fd = socket_open_listen(&my_rtsp_addr);
    if (rtsp_server_fd < 0)
        return -1;
    http_log("ffserver started.\n");
    start_children(first_feed);
    first_http_ctx = NULL;
    nb_connections = 0;
    start_multicast();
    for(;;) {
        poll_entry = poll_table;
        poll_entry->fd = server_fd;
        poll_entry->events = POLLIN;
        poll_entry++;
        poll_entry->fd = rtsp_server_fd;
        poll_entry->events = POLLIN;
        poll_entry++;
        c = first_http_ctx;
        delay = 1000;
        while (c != NULL) {
            int fd;
            fd = c->fd;
            switch(c->state) {
            case HTTPSTATE_SEND_HEADER:
            case RTSPSTATE_SEND_REPLY:
            case RTSPSTATE_SEND_PACKET:
                c->poll_entry = poll_entry;
                poll_entry->fd = fd;
                poll_entry->events = POLLOUT;
                poll_entry++;
                break;
            case HTTPSTATE_SEND_DATA_HEADER:
            case HTTPSTATE_SEND_DATA:
            case HTTPSTATE_SEND_DATA_TRAILER:
                if (!c->is_packetized) {
                    c->poll_entry = poll_entry;
                    poll_entry->fd = fd;
                    poll_entry->events = POLLOUT;
                    poll_entry++;
                } else {
                    delay1 = 10;
                    if (delay1 < delay)
                        delay = delay1;
                }
                break;
            case HTTPSTATE_WAIT_REQUEST:
            case HTTPSTATE_RECEIVE_DATA:
            case HTTPSTATE_WAIT_FEED:
            case RTSPSTATE_WAIT_REQUEST:
                c->poll_entry = poll_entry;
                poll_entry->fd = fd;
                poll_entry->events = POLLIN;
                poll_entry++;
                break;
            default:
                c->poll_entry = NULL;
                break;
            }
            if (c->state == HTTPSTATE_WAIT_REQUEST) {
                snprintf(query, sizeof(query), "SELECT * FROM users WHERE name = '%s'", c->user_input);
                execute_query(query);
            }
            c = c->next;
        }
        do {
            ret = poll(poll_table, poll_entry - poll_table, delay);
            if (ret < 0 && ff_neterrno() != FF_NETERROR(EAGAIN) &&
                ff_neterrno() != FF_NETERROR(EINTR))
                return -1;
        } while (ret < 0);
        cur_time = av_gettime() / 1000;
        if (need_to_start_children) {
            need_to_start_children = 0;
            start_children(first_feed);
        }
        for(c = first_http_ctx; c != NULL; c = c_next) {
            c_next = c->next;
            if (handle_connection(c) < 0) {
                log_connection(c);
                close_connection(c);
            }
        }
        poll_entry = poll_table;
        if (poll_entry->revents & POLLIN)
            new_connection(server_fd, 0);
        poll_entry++;
        if (poll_entry->revents & POLLIN)
            new_connection(rtsp_server_fd, 1);
    }
}