//
// Created by ilyagu on 26.02.2022.
//

#ifndef WEBSERVER_URLENCODING_H
#define WEBSERVER_URLENCODING_H

#pragma once
#include <stdio.h>

size_t urldecode(char *dest, const char *src);

size_t urlndecode(char *dest, const char *src, ssize_t n);

#endif //WEBSERVER_URLENCODING_H
