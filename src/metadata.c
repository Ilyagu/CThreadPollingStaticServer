//
// Created by ilyagu on 26.02.2022.
//

#include "metadata.h"
#include "cone_string.h"

const char* unsafe_str = "..";
const char* index_equivalent = "/";
meta_data meta;

bool is_safe_path(const char *target_file_path) {
    if (!target_file_path) {
        return false;
    }

    const char* unsafe_sub_str = strstr(target_file_path, unsafe_str);
    if (strstr(target_file_path, unsafe_str)) {
        if (unsafe_sub_str == target_file_path || *(unsafe_sub_str - 1) == '/'){
            return false;
        }
    }

    return true;
}

void strconcat(char *dest, const char *source1, const char* source2, size_t source1_len) {
    strncpy(dest, source1, source1_len);
    strcpy(dest + source1_len, source2);
}

void get_full_path(const meta_data* meta_data, const char *target_file_path, char* full_path) {

    full_path = strcpyend(full_path, meta_data->work_dir_path);
    full_path = strcpyend(full_path, target_file_path);

    if (*(full_path - 1) == '/') {
        full_path = strcpyend(full_path, meta_data->index);
    }

    *full_path = '\0';
}


const meta_data* get_meta_data() {
    return &meta;
}

void set_meta_data(meta_data meta_d) {
    meta = meta_d;
}
