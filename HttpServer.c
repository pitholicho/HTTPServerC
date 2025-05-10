#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
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
    while(1){
        int client_sock = accept(socket_server, (struct sockaddr*)&client_addr, &client_size);
    }

    return 0;
}