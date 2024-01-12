#include "server.h"
#include "config.h"

int main(int argc, char *argv[]) {
    ServerConfig config;
    load_config(&config, argc > 1 ? argv[1] : "./config.cfg");


    start_server(&config);


    return 0;
}