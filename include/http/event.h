//
// Created by ilyagu on 26.02.2022.
//

#ifndef WEBSERVER_EVENT_H
#define WEBSERVER_EVENT_H

#pragma once

#include "def.h"

int reg_event_impl(descriptor_t ep, descriptor_t fd, void *event_context, int epoll_flag);

int reg_accept_event(descriptor_t ep, descriptor_t acceptor, void *event_context);

int reg_session_read_event(descriptor_t ep, descriptor_t session, void *event_context);


#endif //WEBSERVER_EVENT_H
