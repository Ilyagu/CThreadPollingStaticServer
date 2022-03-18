//
// Created by ilyagu on 26.02.2022.
//

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#include <stdio.h>

#include "def.h"
#include "tcp.h"
#include "http/eventid.h"
#include "http/event.h"
#include "http/header.h"
#include "http/eventloop.h"

#include "metadata.h"

#define EVENT_POOL_SIZE 120
#define READ_BUFFER_SIZE 1024
#define MAX_SOURCE_PATH_LEN 1024


typedef enum {
    accept_event = ACCEPT_EVENT_ID,
    read_event = SESSION_READ_EVENT_ID
} event_id;

event_id get_event_id(const epoll_event_t *event) {
    return event->data.u32;
}

int accept_handle(descriptor_t ep, descriptor_t acceptor) {
    descriptor_t session = tcp_accept_session(acceptor, not_blocking);
    if (session < 0) {
        //  perror("accept: ");
        return -1;
    }

    if (reg_session_read_event(ep, session, NULL) < 0) {
        return -1;
    }

    return 0;
}

ssize_t get_file_size(descriptor_t fd) {
    stat_t source_stat;
    if (fstat(fd, &source_stat) < 0) {
        return -1;
    }

    return source_stat.st_size;
}

int session_read_handle(descriptor_t ep, descriptor_t session) {
    const meta_data *meta_data = get_meta_data();
    char buffer[READ_BUFFER_SIZE + 1];
    ssize_t buffer_len = read(session, buffer, READ_BUFFER_SIZE);
    if (buffer_len <= 0) {
        close(session);

        return -1;
    }

    buffer[buffer_len] = '\0';

    // TODO(Ilyagu) отладочный вывод
    // printf("req\n-----\n%s-----\n", buffer);

    http_request_line_t header;

    char response_header_buffer[RESPONSE_HEADER_BUFFER_MAX_LEN];

    int path_len = http_parse_request_line(&header, buffer, buffer_len);
    if (path_len < 0) {
        http_response_meta_t response_meta = {0, NULL, close_conn, true, bad_request};
        int len = http_set_response_header(response_header_buffer, &response_meta);
        ssize_t byte_transferred = write(session, response_header_buffer, len);
        close(session);
        if (byte_transferred < 0 || byte_transferred == 0) {
            return -1;
        }
        return 0;
    }

    if (!method_is_allowed(header.method)) {
        http_response_meta_t response_meta = {0, NULL, close_conn, true, method_not_allowed};
        int len = http_set_response_header(response_header_buffer, &response_meta);
        ssize_t byte_transferred = write(session, response_header_buffer, len);
        close(session);
        if (byte_transferred < 0 || byte_transferred == 0) {
            return -1;
        }
        return 0;
    }

    if (!is_safe_path(header.source_path)) {
        http_response_meta_t response_meta = {0, NULL, close_conn, true, forbidden};
        int len = http_set_response_header(response_header_buffer, &response_meta);
        ssize_t byte_transferred = write(session, response_header_buffer, len);
        close(session);
        if (byte_transferred < 0 || byte_transferred == 0) {
            return -1;
        }
        return 0;
    }

    char full_path[MAX_SOURCE_PATH_LEN];
    get_full_path(meta_data, header.source_path, full_path);

    descriptor_t source = open(full_path, O_RDONLY);

    if (source < 0 && !header.source_ext) {
        http_response_meta_t response_meta = {0, NULL, close_conn, true, forbidden};
        int len = http_set_response_header(response_header_buffer, &response_meta);
        ssize_t byte_transferred = write(session, response_header_buffer, len);
        close(session);
        if (byte_transferred < 0 || byte_transferred == 0) {
            return -1;
        }

        return 0;
    } else if (source < 0) {
        http_response_meta_t response_meta = {0, NULL, close_conn, true, not_found};
        int len = http_set_response_header(response_header_buffer, &response_meta);
        ssize_t byte_transferred = write(session, response_header_buffer, len);
        close(session);
        if (byte_transferred < 0 || byte_transferred == 0) {
            return -1;
        }

        return 0;
    }

    ssize_t file_size = get_file_size(source);
    if (file_size < 0) {
        http_response_meta_t response_meta = {0, NULL, close_conn, true, internal_server_error};
        int len = http_set_response_header(response_header_buffer, &response_meta);
        ssize_t byte_transferred = write(session, response_header_buffer, len);
        close(session);
        if (byte_transferred < 0 || byte_transferred == 0) {
            return -1;
        }

        return 0;
    }

    if (strcmp("HEAD", header.method) == 0) {
        http_response_meta_t response_meta = {file_size, get_mime(header.source_ext),
                                              close_conn, true, ok};
        int len = http_set_response_header(response_header_buffer, &response_meta);
        ssize_t byte_transferred = write(session, response_header_buffer, len);
        close(session);
        close(source);
        if (byte_transferred < len) {
            return -1;
        }

        return 0;

    } else if (strcmp("GET", header.method) == 0) {
        if (meta_data->send_file) {
            http_response_meta_t response_meta = {file_size, get_mime(header.source_ext),
                                                  close_conn, true, ok};
            int len = http_set_response_header(response_header_buffer, &response_meta);
            char *test__ = response_header_buffer;
            ssize_t byte_transferred = write(session, response_header_buffer, len);
            if (byte_transferred < len) {
                close(session);
                close(source);
                return -1;
            }

            if (sendfile(session, source, 0, file_size) < file_size) {
                close(session);
                close(source);
                return -1;
            }
            close(session);
            close(source);
            return 0;
        } else {
            char *response_body = malloc(sizeof(byte_t) * file_size + 1);
            if (read(source, response_body, file_size) < 0) {
                http_response_meta_t response_meta = {0, NULL, close_conn, true, internal_server_error};
                int len = http_set_response_header(response_header_buffer, &response_meta);
                ssize_t byte_transferred = write(session, response_header_buffer, len);
                close(session);
                close(source);
                free(response_body);
                if (byte_transferred < len) {
                    return -1;
                }
                return 0;
            }

            http_response_meta_t response_meta = {file_size, get_mime(header.source_ext),
                                                  close_conn, true, ok};
            int len = http_set_response_header(response_header_buffer, &response_meta);
            ssize_t byte_transferred = write(session, response_header_buffer, len);
            if (byte_transferred < len) {
                close(session);
                close(source);

                return -1;
            }

            if (write(session, response_body, file_size) < file_size) {
                close(session);
                close(source);
                return -1;
            }
            close(session);
            close(source);
            free(response_body);

            return 0;
        }
    }

    return 0;
}

void http_events_handle(event_loop_data_t *data, epoll_event_t *events, size_t events_num) {
    for (size_t i = 0; i < events_num; i++) {
        epoll_event_t event = events[i];

        if (event.data.fd == data->acceptor) {
            // TODO(Ilyagu) дебаг
            // printf("[debug] accept handle");
            accept_handle(data->ep, event.data.fd);
        } else {
            // TODO дебаг
            // printf("[debug] read handle");

            session_read_handle(data->ep, event.data.fd);
        }
    }
}

void http_event_loop_run(void *data) {
    if (!(event_loop_data_t *) data) {
        return;
    }
    event_loop_data_t *event_loop_data = (event_loop_data_t *) data;

    epoll_event_t events[EVENT_POOL_SIZE];

    while (true) {
        int events_num = epoll_wait(event_loop_data->ep, events, EVENT_POOL_SIZE, -1);
        if (events_num < 1) {
            break;
        }

        // printf("%d\n", events_num);
        // TODO(Ilyagu) дебаг
        // printf("[debug] new event loop iteration, %d events", events_num);

        http_events_handle(event_loop_data, events, events_num);
    }

}
