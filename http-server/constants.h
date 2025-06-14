#ifndef CONSTANTS_H
#define CONSTANTS_H

typedef struct {
    char name[50];
    char value[50];
} header;

typedef struct {
    char method[10];
    char path[100];
    char version[50];

    header headers[100];
    int header_count;

    char body[1024];
    int body_length;
} http_request;

/* typedef struct {
    char version[50];
    char status[50];
    char status_code[3];

    header headers[100];
    int header_count;

    char body[1024];
    int body_length;
} http_response; */

#endif