#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define NUM_THREADS 10
#define MAX_BUFFER_SIZE 4096

typedef struct
{
    char* ip;
    int port;
}client_thread_args;

void* client_thread(void* client_arg)
{
    client_thread_args* args = (client_thread_args*)client_arg;

    int socket_desc;
    struct sockaddr_in server;
    char message[] = "GET /index.html HTTP/1.1\r\n\r\n";
    char server_reply[MAX_BUFFER_SIZE];

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1)
    {
        printf("Could not create socket.");
        return NULL;
    }


    // Prepare sockaddr_in structure
    server.sin_addr.s_addr = inet_addr(args->ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(args->port);

    // Connect to the server
    if(connect(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        perror("connect failed");
        return NULL;
    }

    // Send a simple HTTP request
    if(send(socket_desc, message, strlen(message), 0) < 0)
    {
        printf("Send failed");
        return NULL;
    }


    if(recv(socket_desc, server_reply, MAX_BUFFER_SIZE, 0) < 0)
    {
        printf("Receive failed\n");
        return NULL;
    }

    printf("received :%s\n", server_reply);

    close(socket_desc);

    return NULL;
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pthread_t threads[NUM_THREADS];
    client_thread_args thread_args;

    thread_args.ip = argv[1];
    thread_args.port = atoi(argv[2]);

    for(int i = 0; i < NUM_THREADS;  i++)
    {
        pthread_create(&threads[i], NULL, client_thread, (void *)&thread_args);
    }   

    for(int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
