#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "constants.h"
#include "../in-memory-store/hash_table.h"

int handle_GET_request(http_request *request){
    /* The normal in-memory-hash logic here to retreive a key value pair */
    printf("\nGET request handled: ");
    printf("%s\n", request->method);

    char* path_copy = strdup(request->path);

    const char* query = strchr(path_copy, '?');
    if (!query) return -1;

    query++;

    const char* name_param = strstr(query, "name=");
    if(!name_param) return -1;
    name_param += 5;

    const char* end = strchr(path_copy, '&');
    size_t len = end ? (end - name_param) : strlen(name_param);

    char* name = (char *)malloc(len + 1);
    if(name){
        strncpy(name, name_param, len);
        name[len] = '\0';

        printf("Extracted the name: %s\n", name);
        char* read_entry_value = read_entry(name);
        printf("Read entry value: %s\n", read_entry_value);
    } else {
        printf("Unable to extract name\n");
        return 500;
    }

    free(path_copy);
    free(name);
    return 0;
}

int handle_POST_request(http_request *request){
    /* The normal in-memory-hash logic here to create a key value pair */
    printf("\nPOST request handled: ");
    printf("%s\n", request->method);

    char* path_copy = strdup(request->path);

    const char* query = strchr(path_copy, '?');
    if (!query) return -1;

    query++;

    const char* name_param = strstr(query, "name=");
    if(!name_param){
        printf("Unable to extract name\n");
        return -1;
    }
    name_param += 5;

    char* end_name = strchr(path_copy, '&');
    size_t len_name;
    if(end_name){
        len_name = end_name - name_param;
    } else {
        printf("Unable to extract value\n");
        return -1;
    }

    char* name = (char *)malloc(len_name + 1);
    if(name){
        strncpy(name, name_param, len_name);
        name[len_name] = '\0';

        printf("Extracted the name: %s\n", name);
    } else {
        printf("Unable to extract name\n");
        return 500;
    }

    const char* value_param = strstr(query, "value=");
    if(!value_param){
        printf("Unable to extract value\n");
        return -1;
    }
    value_param += 6;

    char* end_value = strchr(value_param, '&');
    size_t len_value = end_value ? (end_value - value_param) : strlen(value_param);
    char* value = (char *)malloc(len_value + 1);
    if(value){
        strncpy(value, value_param, len_value);
        value[len_value] = '\0';

        printf("Extracted the value: %s\n", value);
    } else {
        printf("Unable to extract value\n");
        return 500;
    }

    create_entry(name, value);

    free(path_copy);
    return 0;
}

int handle_DELETE_request(http_request *request){
    /* The normal in-memory-hash logic here to remove a key value pair */
    printf("\nDELETE request handled: ");
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