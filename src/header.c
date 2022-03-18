//
// Created by ilyagu on 26.02.2022.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "http/header.h"
#include "cone_string.h"
#include "urlencoding.h"

#define HTTP_DATETIME_FORMAT "%a, %d %b %Y %H:%M:%S GMT"
#define HTTP_MAX_DATE_TIME_LEN 50
#define SUPPORTED_HTTP_PROTOCOL "HTTP/1.1"

const char *get_http_response_status_name(http_response_status status) {
    switch (status) {
        case ok:
            return status_code_name[0];
        case bad_request:
            return status_code_name[1];
        case forbidden:
            return status_code_name[2];
        case not_found:
            return status_code_name[3];
        case method_not_allowed:
            return status_code_name[4];
        case internal_server_error:
            return status_code_name[5];
        default:
            return NULL;
    }
}

const char *get_mime(const char *extend) {
    if (!extend) {
        return NULL;
    }

    if (strcmp(extend, ".html") == 0) {
        return supported_mime_types[EXTEND_HTML_MIME];
    } else if (strcmp(extend, ".css") == 0) {
        return supported_mime_types[EXTEND_CSS_MIME];
    } else if (strcmp(extend, ".js") == 0) {
        return supported_mime_types[EXTEND_JS_MIME];
    } else if (strcmp(extend, ".jpeg") == 0) {
        return supported_mime_types[EXTEND_JPG_MIME];
    } else if (strcmp(extend, ".jpg") == 0) {
        return supported_mime_types[EXTEND_JPG_MIME];
    } else if (strcmp(extend, ".png") == 0) {
        return supported_mime_types[EXTEND_PNG_MIME];
    } else if (strcmp(extend, ".gif") == 0) {
        return supported_mime_types[EXTEND_GIF_MIME];
    } else if (strcmp(extend, ".swf") == 0) {
        return supported_mime_types[EXTEND_SWF_MIME];
    }

    return NULL;
}

int set_status_line(char *dest, http_response_status status) {
    if (!dest || status >= HTTP_STATUS_CODE_NAME_NUM) {
        return -1;
    }

    char *dest_start = dest;

    dest = strcpyend(dest, SUPPORTED_HTTP_PROTOCOL);
    *dest++ = ' ';
    dest = strcpyend(dest, status_code_name[status]);
    *dest++ = '\r';
    *dest++ = '\n';

    return (int) (dest - dest_start);
}


int set_string_header(char *dest, const char *key, const char *value) {
    if (!key || !value || !dest) {
        return -1;
    }

    char *dest_start = dest;

    dest = strcpyend(dest, key);
    *dest++ = ':';
    *dest++ = ' ';
    dest = strcpyend(dest, value);
    *dest++ = '\r';
    *dest++ = '\n';
    return (int) (dest - dest_start);
}

int set_number_header(char *dest, const char *key, int64_t value) {
    if (!key || !dest) {
        return -1;
    }
    char *dest_start = dest;

    dest = strcpyend(dest, key);
    *dest++ = ':';
    *dest++ = ' ';
    int num_len = snprintf(dest, strlen(dest), "%lu", value);
    dest += num_len;
    *dest++ = '\r';
    *dest++ = '\n';

    return (int) (dest - dest_start);
}

int set_datetime_header(char *dest, const char *key, const tm_t *value) {
    if (!key || !dest) {
        return -1;
    }

    if (!value) {
        long now = time(NULL);
        value = localtime(&now);
    }

    char *dest_start = dest;

    dest = strcpyend(dest, key);
    *dest++ = ':';
    *dest++ = ' ';

    size_t date_len = strftime(dest, HTTP_MAX_DATE_TIME_LEN, HTTP_DATETIME_FORMAT, value);
    dest += date_len;
    *dest++ = '\r';
    *dest++ = '\n';

    return (int) (dest - dest_start);
}

int http_parse_request_line(http_request_line_t *header, const char *http_request, size_t http_request_len) {
    if (!header || !http_request || !http_request_len) {
        return -1;
    }

    const char *del_pos = strchr(http_request, ' ');

    if (!del_pos) {
        return -1;
    }

    size_t method_len = del_pos - http_request;
    if (!method_len || method_len > HTTP_MAX_METHOD_NAME_LEN) {
        return -1;
    }

    strncpy(header->method, http_request, method_len);
    header->method[method_len] = '\0';
    while (*del_pos == ' ') del_pos++;
    const char *url_start = del_pos;

    while (*del_pos != ' ' && *del_pos != '?') {
        del_pos++;
    }

    size_t url_len = del_pos - url_start;
    if (!url_len || url_len > HTTP_MAX_URL_LEN) {
        return -1;
    }
    size_t decode_url_len = urlndecode(header->source_path, url_start, (ssize_t)url_len);
    header->source_path[decode_url_len] = '\0';
    header->source_ext = NULL;

    const char *ext = header->source_path + url_len - 1;

    if (ext != header->source_path && *ext == '/') ext--;
    while (ext != header->source_path && *ext != '/'){
        if (*ext == '.') {
            header->source_ext = ext;
            break;
        }
        ext--;
    }

    return (int)decode_url_len;
}

int http_set_response_header(char *dest, const http_response_meta_t *meta) {
    if (!meta || !dest) {
        return -1;
    }
    char *start_dest = dest;
    int len = set_status_line(dest, meta->status);
    if (len < 0) {
        return -1;
    }
    dest += len;
    len = set_string_header(dest, "Server", SERVER_NAME);
    dest += len;
    len = set_string_header(dest, "Connection", connection_mod_str[meta->connection]);
    dest += len;
    if (meta->content_length) {
        len = set_number_header(dest, "Content-Length", (int64_t) (meta->content_length));
        dest += len;
    }
    if (meta->mime != NULL) {
        len = set_string_header(dest, "Content-Type", meta->mime);
        dest += len;
    }
    if (meta->set_now_datetime) {
        len = set_datetime_header(dest, "Date", NULL);
        dest += len;
    }
    *dest++ = '\r';
    *dest++ = '\n';
    len += 2;

    return (int)(dest - start_dest);
}


bool method_is_allowed(const char *method) {
    if (!method) {
        return false;
    }

    for (size_t i = 0; i < ALLOWED_METHOD_NUM; i++) {
        if (strcmp(allowed_method[i], method) == 0)
            return true;
    }

    return false;
}
