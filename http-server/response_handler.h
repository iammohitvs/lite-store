#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <stdlib.h>

int handle_response(char *response, size_t response_size);

int return_400_response(char *response, size_t response_size);

int return_500_response(char *response, size_t response_size);

int return_OPTIONS_response(char *response, size_t response_size);

#endif