//
// Created by ilyagu on 26.02.2022.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <limits.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include "tcp.h"

typedef struct sockaddr_in sockaddr_in_t;
typedef struct sockaddr_storage sockaddr_storage_t;
typedef struct sockaddr sockaddr_t;

descriptor_t tcp_create_acceptor(uint16_t port, const tcp_options_t* tcp_options) {
    descriptor_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }

    sockaddr_in_t serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(sock, (sockaddr_t *) &serv_addr, sizeof(serv_addr)) < 0) {
        return -1;
    }

    if (listen(sock, INT_MAX) < 0) {
        return -1;
    }

    if (!tcp_options) {
        return sock;
    }

    if (tcp_options->work_mod == not_blocking) {
        if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0) {
            close(sock);
            return -1;
        }
    }

    if (tcp_options->tcp_cork) {
        int state = 1;
        if (setsockopt(sock, IPPROTO_TCP, TCP_CORK, &state, sizeof(state)) > 0) {
            close(sock);
            return -1;
        }
    }


    return sock;
}

descriptor_t tcp_accept_session(descriptor_t sock, descriptor_work_mod_t work_mod) {
    sockaddr_storage_t session_addr;
    socklen_t s_size = sizeof(session_addr);

    descriptor_t session = accept(sock, (sockaddr_t *) &session_addr, &s_size);
    if (session < 0) {
        return -1;
    }

    if (work_mod == not_blocking) {
        if (fcntl(session, F_SETFL, O_NONBLOCK) < 0) {
            return -1;
        }
    }

    return session;
}
