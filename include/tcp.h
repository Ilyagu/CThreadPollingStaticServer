//
// Created by ilyagu on 26.02.2022.
//

#ifndef WEBSERVER_TCP_H
#define WEBSERVER_TCP_H

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "def.h"

typedef enum {
    blocking = 0,
    not_blocking = 1
} descriptor_work_mod_t;

typedef struct tcp_options {
    descriptor_work_mod_t work_mod;
    bool tcp_cork;
} tcp_options_t;

descriptor_t tcp_create_acceptor(uint16_t port, const tcp_options_t* tcp_options);

descriptor_t tcp_accept_session(descriptor_t sock, descriptor_work_mod_t work_mod);

#endif //WEBSERVER_TCP_H
