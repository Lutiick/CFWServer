#ifndef CFWSERVER_UTILS_H
#define CFWSERVER_UTILS_H

int is_ip(char *IP);
char * parse_request(char *buffer);
void GET(int client_socket, char *query_str, char *htdocs);

#endif //CFWSERVER_UTILS_H


