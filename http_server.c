#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "thread_pool.h"

#define MAX_BUFFER_SIZE 4096
#define MAX_CONN_QUEUE 500
#define PORT 8080

void handle_request(void *);

void handle_request(void *socket_desc)
{
    int sock = *(int *)socket_desc;
    char received[MAX_BUFFER_SIZE];
    
    memset(received, 0, MAX_BUFFER_SIZE);

    if(read(sock, received, MAX_BUFFER_SIZE) <= 0)
    {
        close(sock);
        free(socket_desc);
        return;
    }

    char filePath[256];
    if(sscanf(received, "GET %s HTTP/", filePath) != 1)
    {
        printf("Failed to parse HTTP request\n");
        close(sock);
        free(socket_desc);
        return;
    }

    char data[MAX_BUFFER_SIZE];
    FILE *file = fopen(filePath + 1, "r");
    if(!file)
    {
        char response[] = "HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 9\n\nNot Found";
        write(sock, response, sizeof(response));
        close(sock);
        free(socket_desc);
        return;
    }
    
    // Get the content of the file
    size_t n = fread(data, 1, MAX_BUFFER_SIZE, file);
    char response[MAX_BUFFER_SIZE];
    snprintf(response, MAX_BUFFER_SIZE, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %zd\n\n%s", n, data);
    write(sock, response, strnlen(response, MAX_BUFFER_SIZE));
    fclose(file);
    close(sock);
    free(socket_desc);
}

int main(int argc, char *argv[])
{
    int socket_desc, new_socket, *new_sock;
    struct sockaddr_in server, client;
    int num_threads, job_capacity;

    if(argc !=3)
    {
        printf("Usage: %s <num_threads> <job_capacity>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    num_threads = atoi(argv[1]);
    job_capacity = atoi(argv[2]);

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc < 0)
    {
        perror("socket creation failed");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed");
        close(socket_desc);
        exit(1);
    }

    if(listen(socket_desc, MAX_CONN_QUEUE) < 0)
    {
        perror("listen failed");
        close(socket_desc);
        exit(1);
    }

    // Initialize thread pool
    thread_pool_t* pool = thread_pool_create(num_threads, job_capacity);
    int addr_len = sizeof(struct sockaddr_in);

    while((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&addr_len )))
    {
        // Copy the socket desc as it will be overwritten on the next client connection.
        new_sock = malloc(sizeof(int));
        *new_sock = new_socket;
        printf("new_sock(malloc):%d new_socket:%d\n", *new_sock, new_socket);

        // Add the job to the thread pool.
        int is_added = thread_pool_add_job(pool, handle_request, new_sock);
        if (is_added == 0) {
            close(*new_sock);
            free(new_sock);
        }
    }

    thread_pool_destroy(pool);
    return 0;
}