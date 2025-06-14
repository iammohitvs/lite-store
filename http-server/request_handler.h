#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "constants.h"

int handle_request(char *raw_request, http_request *request);

#endif