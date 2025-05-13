#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


int GET_resolver(char **parts, int size){

    for (int i = 0; i < size; i++)
    {
        printf("[DEBUG] %i - %s\n", i,parts[i]);
    } 

    return 0;
}

int http_GET(char *get_recv)
{
    int i = 0, cap = 10;
    char **parts = malloc(cap * sizeof(char *));
    char *line = strtok(get_recv, "\r\n");
    while (line != NULL)
    {
        parts[i] = strdup(line);
        printf("[DEBBUG] %i- %s\n", i ,parts[i]);
        line = strtok(NULL, "\r\n");
        i++;
        if (i >= cap)
        {
            cap += 10;
            parts = realloc(parts, cap  * sizeof(char*));
        } 
    }
    GET_resolver(parts, i);
    
    for (int j = 0; j < i; j++)
    {
        free(parts[j]);
    }
    free(line);
    
    return 0;
}

int http_request_resolver(int client_socket)
{
    char *buffer = (char *)malloc(1024);
    int bytes_recv = recv(client_socket, buffer, 1024, 0);
    buffer[bytes_recv] = '\0';
    printf("[DEBBUG] - %i\n\n", bytes_recv);
    if(strstr(buffer, "GET /"))
        http_GET(buffer);

    return 0;
}

int server_socket_open()
{   
    int opt = 1;
    int socket_server = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_server == -1){
        perror("Error to creat socket");
        exit(1);
    }
    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(bind(socket_server, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("Error in bind");
        close(socket_server);
        exit(1);
    }

    if(listen(socket_server, 5) < 0){
        perror("Eror to listen");
        close(socket_server);
        exit(1);
    }
    printf("\nServer listen in port 8080...\n");

    socklen_t client_size = sizeof(client_addr);
    int client_sock = accept(socket_server, (struct sockaddr*)&client_addr, &client_size);
    http_request_resolver(client_sock);

    return 0;
}

int main()
{
    server_socket_open();
    
    return 0;
}
