#ifndef SERVER_H
#define SERVER_H

#include "utils.h"
#include "parameter_config.h"

typedef struct {
    measurements_t *measurements;
    greenhouse_config_t *config;
} server_context_t;

void http_server_start(greenhouse_config_t *greenhouse_config, measurements_t *measurment);

#endif