//
// Created by ilyagu on 26.02.2022.
//

#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

#pragma once

#include "metadata.h"

typedef struct event_loop_data {
    descriptor_t ep;
    descriptor_t acceptor;
} event_loop_data_t;

void http_event_loop_run(void *data);

#endif //WEBSERVER_EVENTLOOP_H
