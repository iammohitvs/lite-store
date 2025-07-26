#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "constants.h"
#include "../in-memory-store/hash_table.h"

char *handle_GET_request(http_request *request){
    /* The normal in-memory-hash logic here to retreive a key value pair */
    printf("\nGET request handled: ");
    printf("%s\n", request->method);

    char* path_copy = strdup(request->path);
    char *error_return_400 = "400";
    char *error_return_500 = "500";

    const char* query = strchr(path_copy, '?');
    if (!query) return error_return_400;

    query++;

    const char* name_param = strstr(query, "name=");
    if(!name_param) return error_return_400;
    name_param += 5;

    const char* end = strchr(path_copy, '&');
    size_t len = end ? (end - name_param) : strlen(name_param);

    char* name = (char *)malloc(len + 1);
    if(name){
        strncpy(name, name_param, len);
        name[len] = '\0';

        printf("Extracted the name: %s\n", name);
        char* read_entry_value = read_entry(name);

        if(!read_entry_value){
            fprintf(stderr, "name doesnt exist in the hash table (read entry)\n");
            return error_return_400;
        } else {
            printf("Read entry value: %s\n", read_entry_value);
            return read_entry_value;
        }
    } else {
        fprintf(stderr, "Unable to extract name\n");
        return error_return_500;
    }

    free(path_copy);
    free(name);
    return 0;
}

char* handle_POST_request(http_request *request){
    /* The normal in-memory-hash logic here to create a key value pair */
    printf("\nPOST request handled: ");
    printf("%s\n", request->method);

    char *error_return_400 = "400";
    char *error_return_500 = "500";

    char* path_copy = strdup(request->path);

    const char* query = strchr(path_copy, '?');
    if (!query) return error_return_400;

    query++;

    const char* name_param = strstr(query, "name=");
    if(!name_param){
        fprintf(stderr, "Unable to extract name\n");
        return error_return_400;
    }
    name_param += 5;

    char* end_name = strchr(path_copy, '&');
    size_t len_name;
    if(end_name){
        len_name = end_name - name_param;
    } else {
        fprintf(stderr, "Unable to extract name\n");
        return error_return_400;
    }

    char* name = (char *)malloc(len_name + 1);
    if(name){
        strncpy(name, name_param, len_name);
        name[len_name] = '\0';

        printf("Extracted the name: %s\n", name);
    } else {
        fprintf(stderr, "Unable to extract name\n");
        return error_return_500;
    }

    const char* value_param = strstr(query, "value=");
    if(!value_param){
        fprintf(stderr, "Unable to extract value\n");
        return error_return_500;
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
        fprintf(stderr, "Unable to extract value\n");
        return error_return_500;
    }

    char* return_name = create_entry(name, value);

    free(path_copy);
    return return_name;
}

char* handle_DELETE_request(http_request *request){
    /* The normal in-memory-hash logic here to remove a key value pair */
    printf("\nDELETE request handled: ");
    printf("%s\n", request->method);

    char *error_return_400 = "400";
    char *error_return_500 = "500";

    char* path_copy = strdup(request->path);

    const char* query = strchr(path_copy, '?');
    if (!query) return error_return_400;

    query++;

    const char* name_param = strstr(query, "name=");
    if(!name_param) return error_return_400;
    name_param += 5;

    const char* end = strchr(path_copy, '&');
    size_t len = end ? (end - name_param) : strlen(name_param);

    char* name = (char *)malloc(len + 1);
    if(name){
        strncpy(name, name_param, len);
        name[len] = '\0';

        printf("Extracted the name: %s\n", name);
        char* entry_deleted_value = delete_entry(name);

        return entry_deleted_value;
    } else {
        fprintf(stderr, "Unable to extract name\n");

        return error_return_500;
    }

    free(path_copy);
    free(name);
}

int handle_PUT_request(){
    /* TODO */

    return 0;
}

int handle_PATCH_request(){
    /* TODO */
    
    return 0;
}

char* handle_OPTIONS_request(){
    char *return_204 = "204";
    return return_204;
}

char* manage_request(http_request *request){
    char *error_return_400 = "400";

    switch (request->method[0])
    {
    case 'G':
        /* GET */
        return handle_GET_request(request);
        break;
    case 'P':
        /* POST, PUT, PATCH */
        switch (request->method[1])
        {
        case 'O':
            /* POST */
            return handle_POST_request(request);
            break;
        case 'U':
            /* PUT */
            break;
        case 'A':
            /* PATCH */
            break;
        default:
            fprintf(stderr, "Manage request (cannot identify method) - inner switch");
            return error_return_400;
        }
        break;
    case 'D':
        /* DELETE */
        return handle_DELETE_request(request);
        break;
    case 'O':
        /* DELETE */
        return handle_OPTIONS_request();
        break;
    default:
        fprintf(stderr, "Manage request (cannot identify method) - outer switch");
        return error_return_400;
    }

    return 0;
}

/* int main(){
    return 0;
} */