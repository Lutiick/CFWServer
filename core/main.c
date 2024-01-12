#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "../utils/utils.h"
#include <sys/wait.h>


struct moduleSetting {
    char dir[PATH_MAX];
};

struct cgi {

};

void handle_request(int client_socket, struct sockaddr_in * client_address, char* root_directory) {
    char *client_ip = inet_ntoa(client_address->sin_addr);
    int client_port = ntohs(client_address->sin_port);

    printf("[INFO]: New request from %s:%d\n", client_ip, client_port);

    char recv_buffer[4096];
    read(client_socket, recv_buffer, sizeof(recv_buffer));


    char *query_str = parse_request(recv_buffer);
    if (strcmp(query_str + strlen(query_str) - 4, "cgi?") == 0) {
        *(query_str + strlen(query_str) - 1) = '\0';
        strcat(root_directory, query_str);
        char ** arg = (char**)malloc(sizeof(char**));
        char ** empty2;
        char str_socket[20];
        sprintf(str_socket, "%d", client_socket);
        arg[0] = str_socket;
        execve(root_directory, arg, empty2);
    } else {
        strcat(root_directory, "/static");
        GET(client_socket, query_str, root_directory);
        free(query_str);
        close(client_socket);
    }
}

void server_loop(int server_socket, char* root_directory) {
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);

    while ((client_socket =  accept(server_socket,
                                    (struct sockaddr*) &client_address,
                                    &client_address_size)))
    {
        if (client_socket < 0) {
            printf("[ERROR]: Accept failed \n");
            continue;
        }
        pid_t pid = fork();
        if (0 == pid) {
            close(server_socket);
            handle_request(client_socket, &client_address, root_directory);
            exit(EXIT_SUCCESS);
        } else if (pid > 0) {
            close(client_socket);
        } else {
            perror("Error forking process");
            exit(EXIT_FAILURE);
        }
    }
}


void setup_server(char* host, int port, char* root_directory)
{

    int server_socket;
    struct sockaddr_in server_address;


    char path_to_pidfile[PATH_MAX];
    strcpy(path_to_pidfile, root_directory);
    strcat(path_to_pidfile, "/cgi.pid");


    if (!(is_ip(host))) {
        perror("[ERROR]: Invalid host \n");
        remove(path_to_pidfile);
        exit(EXIT_FAILURE);
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) { // Something went wrong
        perror("[ERROR]: Could not create socket \n");
        remove(path_to_pidfile);
        exit(EXIT_FAILURE);
    }
    printf("[OK]: Socket started successfully \n");


    inet_aton(host, &server_address.sin_addr);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        printf("[ERROR]: Binding error \n");
        remove(path_to_pidfile);
        exit(0);
    }
    printf("[OK]: Successfully binded \n");

    if (listen(server_socket, 10)) {
        fprintf(stderr, "[ERROR]: Can't listen on %s:%d\n", host, port);
        remove(path_to_pidfile);
        exit(EXIT_FAILURE);
    }
    printf("[OK]: Listening on %s:%d\n", host, port);

    server_loop(server_socket, root_directory);
}

int main(int argc, char *argv[]) {
    char* root_directory = "."; // default root directory

    if (strcmp(argv[1], "stop") == 0) {
        strcpy(root_directory, argv[2]);       //running server
        strcat(root_directory, "/cgi.pid");
        FILE * file = fopen(root_directory, "r+");
        //printf("%s\n", path_to_pidfile);
        if (!file) {
            perror("[ERROR]: Can't open PID file. Server is offline or some error is occured.\n");
            exit(EXIT_FAILURE);
        } else {
            pid_t pid;
            fscanf(file, "%d", &pid);
            if (kill(pid, SIGTERM) == -1) {
                perror("[ERROR]: Can't terminate server. It is offline or some error is occured.\n");
                fclose(file);
                exit(EXIT_FAILURE);
            }
            printf("[OK]: Server correctly stopped.\n");
            fclose(file);
            remove(root_directory);
        }
        exit(0);
    }


//    if ((daemon(0, 1)) == -1) {
//        printf("[ERROR]: Fork failed, something went wrong ;C\n");
//        exit(0);
//    }

    char path_to_pidfile[PATH_MAX];
    strcpy(root_directory, argv[3]);
    strcpy(path_to_pidfile, root_directory);
    strcat(path_to_pidfile, "/cgi.pid");


    FILE * file = fopen(path_to_pidfile, "a+");

    fprintf(file, "%d", getpid());
    fclose(file);

    root_directory = argv[3];
    setup_server(argv[1], atoi(argv[2]), root_directory);
    return 0;
}