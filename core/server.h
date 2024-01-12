//
// Created by 11 on 12.01.2024.
//

#ifndef CFWSERVER_SERVER_H
#define CFWSERVER_SERVER_H

#include <stddef.h>

typedef struct {
    char key[128];
    char value[128];
} KeyValue;

typedef struct {
    char extension[16];
    char mime_type[32];
} MimeMapping;

typedef struct {
    int allow_cgi;
    int allow_proxy;
    char cgi_directory[256];
    char static_directory[256];
    char proxy_pass[128];
} ServerConfig;

typedef struct {
    KeyValue *directives;
    size_t directives_count;
} Block;


void start_server(ServerConfig *config);

#endif //CFWSERVER_SERVER_H
