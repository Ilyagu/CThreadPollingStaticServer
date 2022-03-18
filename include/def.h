//
// Created by ilyagu on 26.02.2022.
//

#ifndef WEBSERVER_DEF_H
#define WEBSERVER_DEF_H

#pragma once
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define SERVER_NAME "Cone"

typedef int descriptor_t;
typedef struct epoll_event epoll_event_t;
typedef struct stat stat_t;
typedef struct tm tm_t;
typedef unsigned char byte_t;

#endif //WEBSERVER_DEF_H
