//
// Created by ilyagu on 26.02.2022.
//

#include <sys/epoll.h>

#include "../include/http/event.h"

int reg_event_impl(descriptor_t ep, descriptor_t fd, void *event_context, int epoll_flag) {
    epoll_event_t ev;
    ev.data.fd = fd;
    if (event_context) {
        ev.data.ptr = event_context;
    }
    ev.events = epoll_flag;
    if (epoll_ctl(ep, EPOLL_CTL_ADD, fd, &ev) < 0) {
        return -1;
    }

    return 0;
}

int reg_accept_event(descriptor_t ep, descriptor_t acceptor, void *event_context) {
    return reg_event_impl(ep, acceptor, event_context,
                          EPOLLIN | EPOLLERR | EPOLLEXCLUSIVE);
}

int reg_session_read_event(descriptor_t ep, descriptor_t session, void *event_context) {
    return reg_event_impl(ep, session,
                          event_context,
                          EPOLLIN | EPOLLONESHOT | EPOLLET);
}
