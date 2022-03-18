//
// Created by ilyagu on 26.02.2022.
//

#include <stdio.h>
#include <stdlib.h>

#include "urlencoding.h"

#define ENCODING_BASE 16

size_t urldecode(char *dest, const char *src) {
    char *dest_start = dest;

    while (*src != '\0') {
        if (*src == '%' && *(src + 1) != '\0' && *(src + 2) != '\0') {
            char sym_code[2] = {*(src + 1), *(src + 2)};
            ssize_t sym = strtol(sym_code, NULL, ENCODING_BASE);
            *dest++ = (char) sym;
            src += 3;

            continue;
        }

        *dest++ = *src++;
    }

    return dest - dest_start;
}

size_t urlndecode(char *dest, const char *src, ssize_t n) {
    char *dest_start = dest;

    while (*src != '\0' && n > 0) {
        if (*src == '%' && *(src + 1) != '\0' && *(src + 2) != '\0') {
            char sym_code[2] = {*(src + 1), *(src + 2)};
            ssize_t sym = strtol(sym_code, NULL, ENCODING_BASE);
            *dest++ = (char) sym;
            src += 3;
            n -= 3;
            continue;
        }

        *dest++ = *src++;
        n--;
    }

    return dest - dest_start;
}
