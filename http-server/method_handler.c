#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "constants.h"

int handle_GET_request(http_request *request){
    /* The normal in-memory-hash logic here to retreive a key value pair */
    printf("\nGET request handled: ");
    printf("%s\n", request->method);

    return 0;
}

int handle_POST_request(http_request *request){
    /* The normal in-memory-hash logic here to create a key value pair */
    printf("\nPOST request handled");
    printf("%s\n", request->method);

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