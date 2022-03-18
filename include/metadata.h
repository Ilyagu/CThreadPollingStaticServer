//
// Created by ilyagu on 26.02.2022.
//

#ifndef WEBSERVER_METADATA_H
#define WEBSERVER_METADATA_H

#pragma once

#include <stdbool.h>
#include <string.h>


typedef struct {
    char *work_dir_path;
    char *index;
    size_t work_dir_path_len;
    bool send_file;
    bool tcp_cork;
} meta_data;


const meta_data* get_meta_data();

bool is_safe_path(const char *target_file_path);

void get_full_path(const meta_data* meta_data, const char *target_file_path, char* full_path);

void set_meta_data(meta_data meta_d);

#endif //WEBSERVER_METADATA_H
