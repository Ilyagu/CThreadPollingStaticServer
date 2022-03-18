//
// Created by ilyagu on 26.02.2022.
//

#include "cone_string.h"

char* strcpyend(char* dest, const char* src) {
    while (*src != '\0') {
        *dest++ = *src++;
    }
    return dest;
}

