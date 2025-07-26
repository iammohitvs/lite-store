#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "constants.h"

int print_request(http_request *request){
    if(!request){
        fprintf(stderr, "No valid request received (print request)");
    }

    printf("\nREQUEST:\n");
    printf("Method: %s\n", request->method);
    printf("Path: %s\n", request->path);
    printf("Version: %s\n", request->version);
    printf("Headers: (%d)\n", request->header_count);

    for(int i = 0; i < request->header_count; i++){
        printf("\t%s: %s\n", request->headers[i].name, request->headers[i].value);
    }

    printf("Body length: %d\n", request->body_length);
    if(request->body_length > 0){
        printf("Body: %s\n", request->body);
    }

    return 0;
}

int create_and_build_request(char *raw_request, http_request *request){
    char *request_copy_for_main = strdup(raw_request);
    char *request_copy_for_headers = strdup(raw_request);

    const char header_splitter[4] = "\n";
    const char space_splitter[4] = " ";
    const char colon_splitter[4] = ": ";

    char *tok_main = strtok(request_copy_for_main, header_splitter);
    if(strlen(tok_main) > 0 && tok_main[strlen(tok_main) - 1] == '\r'){
        tok_main[strlen(tok_main) - 1] = '\0';
    }

    char *method = strtok(tok_main, space_splitter);
    if(method == 0){
        perror("Couldn't get method (create and build request)");
        return -1;
    }
    char *path = strtok(NULL, space_splitter);
    if(path == 0){
        perror("Couldn't get path (create and build request)");
        return -1;
    }
    char *version = strtok(NULL, space_splitter);
    if(version == 0){
        perror("Couldn't get version (create and build request)");
        return -1;
    }
    
    header headers[100];
    request->header_count = 0;
    request->body_length = 0;

    char *tok_headers = strtok(request_copy_for_headers, header_splitter);
    tok_headers = strtok(NULL, header_splitter);
    if(strlen(tok_headers) > 0 && tok_headers[strlen(tok_headers) - 1] == '\r'){
        tok_headers[strlen(tok_headers) - 1] = '\0';
    }

    while(tok_headers != NULL){
        // identifies if we are about to enter the body part of the request
        if(strlen(tok_headers) == 0) break;

        header header;

        char *colon = strchr(tok_headers, ':');
        if(!colon) continue;

        *colon = '\0';
        char *name = tok_headers;
        char *value = colon + 1;

        while(*value == ' ' || *value == '\t'){
            value++;
        }

        strncpy(header.name, name, sizeof(header.name) - 1);
        strncpy(header.value, value, sizeof(header.value) - 1);
        header.name[sizeof(header.name) - 1] = '\0';
        header.value[sizeof(header.value) - 1] = '\0';

        if(strcmp(name, "Content-Length") == 0){
            request->body_length = atoi(value);
        }

        request->headers[request->header_count] = header;

        tok_headers = strtok(NULL, header_splitter);
        if(strlen(tok_headers) > 0 && tok_headers[strlen(tok_headers) - 1] == '\r'){
            tok_headers[strlen(tok_headers) - 1] = '\0';
        }
        request->header_count++;
    }

    if(tok_headers && (tok_headers = strtok(NULL, "\0")) != NULL){
        strncpy(request->body, tok_headers, sizeof(request->body) - 1);
        request->body[sizeof(request->body) - 1] = '\0';
        if(request->body_length == 0){
            request->body_length = strlen(request->body);
        }
    } else {
        request->body[0] = '\0';
    }

    strncpy(request->method, method, sizeof(request->method) - 1);
    strncpy(request->path, path, sizeof(request->path) - 1);
    strncpy(request->version, version, sizeof(request->version) - 1);

    request->method[sizeof(request->method) - 1] = '\0';
    request->path[sizeof(request->path) - 1] = '\0';
    request->version[sizeof(request->version) - 1] = '\0';

    print_request(request);

    free(request_copy_for_main);
    free(request_copy_for_headers);

    return 0;
};

int handle_request(char *raw_request, http_request *request){
    if(!raw_request || !request){
        fprintf(stderr, "Request (no request or no raw request)");
        return -1;
    }

    create_and_build_request(raw_request, request);

    return 0;
}


/* 
    run the file to run the main function to test the request parsing
*/

// int test_handle_request(char *raw_request, http_request *request){
//     return create_and_build_request(raw_request, request);
// }

// int main(){
//     char raw_request[] = "POST /fmekkferfr HTTP/1.1\r\n"
//                     "Content-Type: application/json\r\n"
//                     "User-Agent: PostmanRuntime/7.44.0\r\n"
//                     "Accept: */*\r\n" 
//                     "Postman-Token: f15ad1bd-ff25-4e2b-89a8-50a4e0438f33\r\n"
//                     "Host: localhost:8080\r\n"
//                     "Accept-Encoding: gzip, deflate, br\r\n"
//                     "Connection: keep-alive\r\n"
//                     "Content-Length: 24\r\n"
//                     "\r\n"
//                     "{\r\n"
//                     " \"name\": \"random\"\r\n"  
//                     "}\r\n";
    
//     http_request request;

//     if(test_handle_request(raw_request, &request) == 0){
//         printf("Success!\n");
//         printf("Method: %s\n", request.method);
//         printf("Path: %s\n", request.path);
//         printf("Version: %s\n", request.version);
//         printf("Headers: (%d)\n", request.header_count);

//         for(int i = 0; i < request.header_count; i++){
//             printf("\t%s: %s\n", request.headers[i].name, request.headers[i].value);
//         }

//         printf("Body length: %d\n", request.body_length);
//         if(request.body_length > 0){
//             printf("Body: %s", request.body);
//         }
//     } else {
//         printf("Failed to aprse body");
//     }

//     return 0;
// }
