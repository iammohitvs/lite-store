#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "constants.h"
#include "../in-memory-store/hash_table.h"

int handle_GET_request(http_request *request){
    /* The normal in-memory-hash logic here to retreive a key value pair */
    printf("\nGET request handled: ");
    printf("%s\n", request->method);

    char* name;

    char* path = strtok(request->path, "?");
    char* query = strtok(NULL, "?");

    // will extract the first insrance of name from the search query
    if(query){
        char* pair = strtok(query, "&");

        while(pair){
            char* key = strtok(pair, "=");
            char* val = strtok(NULL, "=");

            if(key && val && (strcmp(key, "name") == 0)){
                name = val;
                break;
            }

            pair = strtok(NULL, "&");
        }
    }

    if(name){
        printf("Extracted the name: %s\n", name);
        read_entry(name);
    } else {
        printf("Unable to extract name\n");
        /* Send a 400 response */
    }

    return 0;
}

int handle_POST_request(http_request *request){
    /* The normal in-memory-hash logic here to create a key value pair */
    printf("\nPOST request handled");
    printf("%s\n", request->method);

    char* name;
    char* value;

    char* path = strtok(request->path, "?");
    char* query = strtok(NULL, "?");

    // will extract the first insrance of name and value from the search query
    if(query){
        char* pair = strtok(query, "&");

        while(pair){
            char* key = strtok(pair, "=");
            char* val = strtok(NULL, "&");

            if(key && val && (strcmp(key, "name") == 0)){
                name = val;
            } else if(key && val && (strcmp(key, "value") == 0)){
                value = val;
            }

            pair = strtok(NULL, "");
        }
    }

    if(name && value){
        printf("Extracted the name: %s\n", name);
        printf("Extracted the value: %s\n", value);
    } else {
        printf("Unable to extract name\n");
        /* Send a 400 response */
    }

    return 0;
}

int handle_DELETE_request(http_request *request){
    /* The normal in-memory-hash logic here to remove a key value pair */
    printf("\nDELETE request handled");
    printf("%s\n", request->method);

    return 0;
}

int handle_PUT_request(){
    /* TODO */

    return 0;
}

int handle_PATCH_request(){
    /* TODO */
    
    return 0;
}

int handle_OPTIONS_request(){
    /* TODO */
    
    return 0;
}

int manage_request(http_request *request){
    switch (request->method[0])
    {
    case 'G':
        /* GET */
        handle_GET_request(request);
        break;
    case 'P':
        /* POST, PUT, PATCH */
        switch (request->method[1])
        {
        case 'O':
            /* POST */
            handle_POST_request(request);
            break;
        case 'U':
            /* PUT */
            break;
        case 'A':
            /* PATCH */
            break;
        default:
            printf("Manage request (cannot identify method) - inner switch");
            return -1;
        }
        break;
    case 'D':
        /* DELETE */
        handle_DELETE_request(request);
        break;
    default:
        printf("Manage request (cannot identify method) - outer switch");
        return -1;
    }

    return 0;
}

/* int main(){
    return 0;
} */