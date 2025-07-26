#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "constants.h"

int print_response(char *response){
    if(!response){
        fprintf(stderr, "Invalid response received (print response)");
        return -1;
    }

    printf("\nRESPONSE: \n%s\n", response);
    return 0;
}

void get_http_date(char *date, size_t date_size){
    time_t now = time(NULL);
    struct tm *gmt = gmtime(&now); // this converts it into GMT time - required by a response header

    strftime(date, date_size, "%a, %d %b %Y %H:%M:%S GMT", gmt); // formatting the time to required standards
}

int handle_response(char *response, size_t response_size, char *response_content){
    if(!response || !response_size){
        fprintf(stderr, "Invalid response or response size or response code (handle response)");
        return -1;
    }

    char status_line[] = "HTTP/1.1 200 OK";
    char response_headers[256];
    char response_body[1024];
    
    snprintf(response_body, 
            sizeof(response_body),
            "<html>"
                "<body><h1>%s</h1></body>"
            "</html>",
            response_content
            );

    char date[128];
    get_http_date(date, sizeof(date));
    snprintf(response_headers,
             sizeof(response_headers),
             "Date: %s\r\nServer: webserver-c\r\nContent-length: %lu\r\nContent-type: text/html\r\nConnection: keep-alive",
             date,
             strlen(response_body)
             );


    snprintf(response, response_size, "%s\r\n%s\r\n\r\n%s", status_line, response_headers, response_body);

    print_response(response);
    return 0;
}

int return_400_response(char *response, size_t response_size){
    char date[128];
    get_http_date(date, sizeof(date));

    snprintf(response, response_size, "HTTP/1.1 400 Bad Request\r\n"
                        "Date: %s\r\n"
                        "Server: webserver-c\r\n"
                        "Content-length: 64\r\n"
                        "Content-type: text/html\r\n"
                        "Connection: close\r\n\r\n"
                        "<html><body><h1>400 Bad Request</h1>Invalid input.</body></html>",
                        date
                        );

    print_response(response);
    return 0;
}

int return_500_response(char *response, size_t response_size){
    char date[128];
    get_http_date(date, sizeof(date));

    snprintf(response, response_size, "HTTP/1.1 500 Internal Server Error\r\n"
                        "Date: %s\r\n"
                        "Server: webserver-c\r\n"
                        "Content-length: 81\r\n"
                        "Content-type: text/html\r\n"
                        "Connection: close\r\n\r\n"
                        "<html><body><h1>500 Internal Server Error</h1>Something went wrong.</body></html>",
                        date
                        );

    print_response(response);
    return 0;
}

int return_OPTIONS_response(char *response, size_t response_size){
    char date[128];
    get_http_date(date, sizeof(date));

    snprintf(response, response_size, "HTTP/1.1 204 No Content\r\n"
                            "Date: %s\r\n"
                            "Server: webserver-c\r\n"
                            "Access-Control-Allow-Origin: *\r\n"
                            "Access-Control-Allow-Methods: GET, POST, DELETE, OPTIONS\r\n"
                            "Access-Control-Allow-Headers: Content-Type\r\n"
                            "Allow: GET, POST, DELETE, OPTIONS\r\n"
                            "Content-Length: 0\r\n"
                            "Connection: keep-alive\r\n\r\n",
                            date
                            );

    print_response(response);
    return 0;
}

/* 
    run the file to run the main function to test the request parsing
*/

/* int test_handle_response(char *response, size_t response_size){
    return handle_response(response, response_size);
}

int main(){
    char response[2048];
    if(test_handle_response(response, sizeof(response)) == -1){
        printf("Failed to generate a response");
    }

    printf("%s", response);

    return 0;
} */