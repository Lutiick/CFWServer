#include "utils.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <arpa/inet.h>

int is_ip(char * IP) {

    char localhost[] = "localhost";
    char ip_local[] = "127.0.0.1";

    if (!strcmp(IP, localhost)) {
        IP = ip_local;
    }

    struct sockaddr_in sa;
    return 0 != inet_pton(AF_INET, IP, &(sa.sin_addr));
}

char * parse_request(char *buffer) {
    char *end = strstr(buffer, "HTTP/1.1");
    int bytes = end - (buffer + 4);
    char *clean_str = (char *)malloc(bytes);
    memmove(clean_str, buffer + 4, bytes);
    *(clean_str + bytes - 1) = '\0';
    return clean_str;
}

void write_header(int client_socket, int status, long int file_size) {
    char headers[2048];
    if (status == 200) {
        sprintf(headers,"HTTP/1.1 200 OK\r\n"
                        "Server: CustomCGI\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: %ld\r\n"
                        "\r\n",
                file_size);
    } else {
        sprintf(headers,"HTTP/1.1 404 NOT FOUND\r\n"
                        "Server: CustomCGI\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: %ld\r\n"
                        "\r\n",
                file_size);
    }
    write(client_socket, headers, strlen(headers));
}

void GET(int client_socket, char *url, char *htdocs) {
    char file_name[2048];
    strcpy(file_name, htdocs);

    if (0 == strcmp("/", url) || 0 == strlen(url) || 0 == strcmp("/favicon.ico", url)) {
        strcat(file_name, "/index.html");
    } else {
        strcat(file_name, url);
    }

    int file_id = open(file_name, O_RDONLY);
    struct stat st;
    char page[8192];
    int ret_in;
    if (-1 == file_id) {
        char error_page[2048];
        strcpy(error_page, htdocs);
        strcat(error_page, "/404page.html");
        file_id = open(error_page, O_RDONLY);
        fstat(file_id, &st);
        write_header(client_socket, 404, st.st_size);
        ret_in = read(file_id, page, 8192);
        write(client_socket, page, ret_in);
        close(file_id);
    } else {
        fstat(file_id, &st);
        //printf("%ld\n", st.st_size);
        write_header(client_socket, 200, st.st_size);

        while ((ret_in = read(file_id, page, 8192)) > 0) {
            int ret_out = 0;
            while (ret_out != ret_in) {
                ret_out += write(client_socket, page + ret_out, ret_in - ret_out);
            }
        }
        close(file_id);
    }

}