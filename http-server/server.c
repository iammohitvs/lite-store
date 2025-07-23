#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "constants.h"
#include "request_handler.h"
#include "method_handler.h"
#include "response_handler.h"

#define PORT 8080
#define BUFFER_SIZE 2048

int main(int argc, char const *argv[])
{
    /* code */
    printf("Hello, world\n");

    char buffer[BUFFER_SIZE];
    
    // all paramaeters passed to the socket function are used in creating a TCP socket
    int socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFileDescriptor == -1){
        perror("Webserver (socket)");
        return 1;
    }
    printf("Socket created successfully\n");

    // Building a HOST
    struct sockaddr_in host_addr;
    int addr_len = sizeof(host_addr);

    host_addr.sin_family = AF_INET; // TCP family
    host_addr.sin_port = htons(PORT); // Giving it a port, so that it can work on both big endian and little endian devices
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0 address - any address (special address)

    // Building a CLIENT
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    if(bind(socketFileDescriptor, (struct sockaddr *)&host_addr, addr_len) != 0){
        perror("Webserver (bind)");
        return 1;
    }
    printf("Socket successfully bound to address\n");

    if (listen(socketFileDescriptor, SOMAXCONN) == -1) {
        perror("Webserver (listen)");
        return 1;
    }
    printf("Socket listening for connections on port: %d\n\n", PORT);

    for(;;){
        int newSocketFd = accept(socketFileDescriptor, (struct sockaddr *)&host_addr, (socklen_t *)&addr_len);
        if(newSocketFd < 0){
            perror("Webserver (accept)");
            continue;
        }
        printf("Connection accepted\n");

        int sockn = getsockname(socketFileDescriptor, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
        if(sockn < 0){
            perror("Webserver (client address - getsockname)");
            continue;
        }

        int valRead = read(newSocketFd, buffer, BUFFER_SIZE);
        if(valRead < 0){
            perror("Webserver (read)");
            continue;
        }
        printf("Webserver read from!\n");

        // Handle the request
        http_request request;
        handle_request(buffer, &request);

        // Handle the method
        char* response_value = manage_request(&request);

        // Handle the response
        char response[2048];
        char response_content[1024];

        if(strcmp(response_value, "empty") == 0){
            snprintf(response_content, sizeof(response_content), "hash table is empty!");
            if(handle_response(response, sizeof(response), response_content) == -1) {
                return_500_response(response, sizeof(response));    
            }
        } else if(strcmp(response_value, "full") == 0){
            snprintf(response_content, sizeof(response_content), "hash table is full!");
            if(handle_response(response, sizeof(response), response_content) == -1) {
                return_500_response(response, sizeof(response));    
            }
        } else if(strcmp(response_value, "204") == 0){
            return_OPTIONS_response(response, sizeof(response));
        } else if(strcmp(response_value, "400") == 0){
            return_400_response(response, sizeof(response));
        } else if(strcmp(response_value, "500") == 0){
            return_500_response(response, sizeof(response));
        } else {
            snprintf(response_content, sizeof(response_content), "query executed successfully: %s!", response_value);
            if(handle_response(response, sizeof(response), response_content) == -1) {
                return_500_response(response, sizeof(response));
            }
        }

        int valWrite = write(newSocketFd, response, strlen(response));
        if(valWrite < 1){
            perror("Webserver (write)");
            continue;
        }
        printf("Webserver written to!\n");

        close(newSocketFd);
    }

    return 0;
}
