//
// Created by 11 on 12.01.2024.
//

#include "config.h"
#include <libconfig.h>
#include <stdlib.h>

void parse_directives(config_setting_t *setting, KeyValue **directives, size_t *directives_count) {
    int count = config_setting_length(setting);

    for (int i = 0; i < count; ++i) {
        config_setting_t *directive = config_setting_get_elem(setting, i);
        const char *key = config_setting_name(directive);
        const char *value = config_setting_get_string(directive);
        snprintf((*directives)[*directives_count].key, sizeof((*directives)[*directives_count].key), "%s", key);
        snprintf((*directives)[*directives_count].value, sizeof((*directives)[*directives_count].value), "%s", value);
        (*directives_count)++;
    }
}

void parse_block(config_setting_t *block_setting, Block *block) {
    block->directives = NULL;
    block->directives_count = 0;

    parse_directives(block_setting, &block->directives, &block->directives_count);
}

void load_config(ServerConfig *config, const char *config_file_path) {
    config_t cfg;
    config_init(&cfg);

    if (!config_read_file(&cfg, config_file_path)) {
        fprintf(stderr, "Error reading configuration file: %s\n", config_error_text(&cfg));
        config_destroy(&cfg);
        exit(EXIT_FAILURE);
    }

    config_setting_t *server_setting = config_lookup(&cfg, "server");
    if (server_setting != NULL) {
        config_lookup_bool(&cfg, "server.allow_cgi", &config->allow_cgi);
        config_lookup_bool(&cfg, "server.allow_proxy", &config->allow_proxy);

        const char *cgi_directory;
        const char *static_directory;
        const char *proxy_pass;

        if (config_lookup_string(&cfg, "server.cgi_directory", &cgi_directory)) {
            snprintf(config->cgi_directory, sizeof(config->cgi_directory), "%s", cgi_directory);
        }

        if (config_lookup_string(&cfg, "server.static_directory", &static_directory)) {
            snprintf(config->static_directory, sizeof(config->static_directory), "%s", static_directory);
        }

        if (config_lookup_string(&cfg, "server.proxy_pass", &proxy_pass)) {
            snprintf(config->proxy_pass, sizeof(config->proxy_pass), "%s", proxy_pass);
        }
    } else {
        fprintf(stderr, "Error: 'server' setting not found in configuration file.\n");
    }

    config_destroy(&cfg);
}