//
// Created by ilyagu on 26.02.2022.
//

#ifndef WEBSERVER_HEADER_H
#define WEBSERVER_HEADER_H

#pragma once

#include <stdlib.h>
#include <stdbool.h>


#include "../def.h"

#define HTTP_MAX_METHOD_NAME_LEN 7
#define HTTP_MAX_URL_LEN 1023
#define HTTP_CONNECTION_MOD_NUM 2
#define HTTP_STATUS_CODE_NAME_NUM 6

#define SUPPORTED_MIME_TYPE_NUM 7
#define ALLOWED_METHOD_NUM 2

#define EXTEND_HTML_MIME 0
#define EXTEND_CSS_MIME 1
#define EXTEND_JS_MIME 2
#define EXTEND_JPG_MIME 3
#define EXTEND_PNG_MIME 4
#define EXTEND_GIF_MIME 5
#define EXTEND_SWF_MIME 6

#define RESPONSE_HEADER_BUFFER_MAX_LEN 1024

static const char *const connection_mod_str[HTTP_CONNECTION_MOD_NUM] = {"close", "keep-alive"};

static const char *const status_code_name[HTTP_STATUS_CODE_NAME_NUM] = {"200 OK", "400 Bad Request", "403 Forbidden",
                                                                        "404 Not Found", "405 Method Not Allowed",
                                                                        "500 Internal Server Error"};


static const char *const supported_mime_types[SUPPORTED_MIME_TYPE_NUM] = {"text/html", "text/css", "application/javascript",
                                                                          "image/jpeg", "image/png", "image/gif",
                                                                          "application/x-shockwave-flash"};

static const char *const allowed_method[ALLOWED_METHOD_NUM] = {"GET", "HEAD"};

typedef enum {
    close_conn = 0,
    keep_alive = 1
} connection_mod;

typedef enum {
    ok = 0,
    bad_request = 1,
    forbidden = 2,
    not_found = 3,
    method_not_allowed = 4,
    internal_server_error = 5
} http_response_status;


typedef struct http_request_line {
    char method[HTTP_MAX_METHOD_NAME_LEN + 1];
    char source_path[HTTP_MAX_URL_LEN + 1];
    const char *source_ext;
} http_request_line_t;

typedef struct http_response_meta {
    size_t content_length;
    const char *mime;
    connection_mod connection;
    bool set_now_datetime;
    http_response_status status;
} http_response_meta_t;


int http_parse_request_line(http_request_line_t *header, const char *http_request, size_t http_request_len);

int http_set_response_header(char *dest, const http_response_meta_t *meta);

int set_string_header(char *dest, const char *key, const char *value);

int set_datetime_header(char *dest, const char *key, const tm_t *value);

const char *get_mime(const char *extend);

bool method_is_allowed(const char* method);

#endif //WEBSERVER_HEADER_H
