#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#include "constants.h"
#include "request_handler.h"
#include "method_handler.h"
#include "response_handler.h"
#include "../in-memory-store/hash_table.h"

#define PORT 8080
#define BUFFER_SIZE 2048
#define THREAD_ARRAY_SIZE 32

typedef struct {
    int socket_fd;
    int thread_index;
} thread_args_t;

pthread_t request_threads[THREAD_ARRAY_SIZE];
int request_threads_counter[THREAD_ARRAY_SIZE] = { 0 };
pthread_mutex_t counter_mutex;

char *filename = "data.csv";

void *request_routine(void *newSocketFd);
void handle_server_shutdown(int signal);

int main(int argc, char const *argv[])
{
    /* code */
    printf("--------------------------------------------------\n");
    printf("----------------SIMPLE REDIS CLONE----------------\n");
    printf("--------------------------------------------------\n\n");
    
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

    // handle the signal for Ctrl+C server shutdown
    signal(SIGINT, handle_server_shutdown);

    // load data from disk onto the hash table
    load_data_from_disk(filename);

    for(;;){
        // for each new connection, create a new thread to process that request and continue hearing for more requests
        int newSocketFd = accept(socketFileDescriptor, (struct sockaddr *)&host_addr, (socklen_t *)&addr_len);
        if(newSocketFd < 0){
            perror("Webserver (accept)");
            continue;
        }
        printf("Connection accepted\n");

        int i;
        int error;
        for(i = 0; i < THREAD_ARRAY_SIZE; i++){
            int slot_found = 0;

            pthread_mutex_lock(&counter_mutex);
            if(!request_threads_counter[i]){
                request_threads_counter[i] = 1;
                slot_found = 1;
            }
            pthread_mutex_unlock(&counter_mutex);

            if(slot_found){
                thread_args_t *thread_args = malloc(sizeof(thread_args_t));
                if(!thread_args){
                    perror("malloc failed for the thread (create thread)\n");
                    pthread_mutex_lock(&counter_mutex);
                    request_threads_counter[i] = 0;
                    pthread_mutex_unlock(&counter_mutex);
                    close(newSocketFd);
                    continue;
                }

                thread_args->socket_fd = newSocketFd;
                thread_args->thread_index = i;

                error = pthread_create(&(request_threads[i]), NULL, request_routine, (void *)thread_args);
                if(error != 0){
                    printf("Thread could not be created for array index: %d\n", i);
                    free(thread_args);
                    pthread_mutex_lock(&counter_mutex);
                    request_threads_counter[i] = 0;
                    pthread_mutex_unlock(&counter_mutex);
                    close(newSocketFd);
                } else {
                    printf("Thread created for array index: %d\n", i);
                }

                break;
            }

            if(i == THREAD_ARRAY_SIZE - 1){
                fprintf(stderr, "Server is busy, no available threads to process your request (create thread)\n");
            }
        }
    }

    return 0;
}

void *request_routine(void *thread_args){
    thread_args_t *args = (thread_args_t *)thread_args;

    int newSocketFd = args->socket_fd;
    int  thread_index = args->thread_index;

    free(thread_args);

    char buffer[BUFFER_SIZE];

    int valRead = read(newSocketFd, buffer, BUFFER_SIZE);
    if(valRead < 0){
        perror("Webserver (read)");
        return NULL;
    }
    printf("Webserver read from!\n");

    // Handle the request
    http_request request;
    handle_request(buffer, &request);

    // Handle the method
    char response[2048];
    char* response_value = manage_request(&request);

    // Handle the response
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
        snprintf(response_content, sizeof(response_content), "%s", response_value);
        if(handle_response(response, sizeof(response), response_content) == -1) {
            return_500_response(response, sizeof(response));
        }
    }

    int valWrite = write(newSocketFd, response, strlen(response));
    if(valWrite < 1){
        perror("Webserver (write)");
        return NULL;
    }
    printf("Webserver written to!\n");

    close(newSocketFd);
    printf("Request finished on array index: %d\n\n", thread_index);

    pthread_mutex_lock(&counter_mutex);
    request_threads_counter[thread_index] = 0;
    pthread_mutex_unlock(&counter_mutex);

    return NULL;
}

void handle_server_shutdown(int signal){
    printf("\nCaught signal for server shutdown. Persisting data to disk\n");
    save_data_to_disk(filename);
    exit(0);
}