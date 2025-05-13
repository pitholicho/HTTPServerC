#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int get_response(int client_socket)
{
    FILE *OK_200;
    char buffer[1024];
    size_t bytes_read;
    OK_200 = fopen("/home/kali/Desktop/C/response_200_main", "r");
    if(OK_200 == NULL){
        perror("Error to open OK_200 file");
    }
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), OK_200)))
    {
        send(client_socket, buffer, bytes_read, 0);
    }
    fclose(OK_200);
    return 0;
}

char* version_resolver(char *str)
{
    if(strcmp(str, "HTTP/1.1") == 0)
        return str;
}

char* path_resolver(char *str)
{
    if(strcmp(str, "/") == 0)
        return str;
}

char* method_resolver(char *str)
{
    if(strcmp(str, "GET") == 0)
        return str;
    if(strcmp(str, "POST") == 0)
        return str;
    if(strcmp(str, "PUT") == 0)
        return str;
    if(strcmp(str, "DELET") == 0)
        return str;
    if(strcmp(str, "HEAD") == 0)
        return str;
    if(strcmp(str, "OPTIONS") == 0)
        return str;
    if(strcmp(str, "PATCH") == 0)
        return str;
}

int request_resolver(char **parts, int size, int client_socket){
    char *cache, *method, *path, *version, *host, *port;
    int j = 0;
    for (int i = 0; i < size; i++)
    {
        if(i == 0){
            cache = strtok(parts[i], " ");
            method = method_resolver(cache);
            printf("\n\n[DEBBUG] %i - %s\n", i, cache);
            while (cache != NULL)
            {
                cache = strtok(NULL, " ");
                printf("[DEBBUG] %i - %s\n", i, cache);
                if(j == 0){
                path = path_resolver(cache);
                } else if(j == 1){
                version = version_resolver(cache);
                }
                j++;
            }
            printf("[DEBUG] %s %s %s\n\n", method, path, version);
        } else if(strcmp(method, "GET") == 0){
            cache = strtok(parts[i], "\r\n");
            if(cache != NULL){
                cache = strtok(parts[i], ":");
                while (cache != NULL)
                {
                    printf("[DEBBUG] %i - %s\n", i, cache);
                    if(strcmp(cache, "Host") == 0){
                        j = 1;
                    } else if(j == 1){
                        host = cache;
                        j++;
                    } else if(j == 2){
                        port = cache;
                        printf("[DEBUG] %s:%s\n\n", host, port);
                    }
                    cache = strtok(NULL, ":");
                }  
            }  
        } else {
            cache = strtok(parts[i], "\r\n");
            if(cache != NULL){
                cache = strtok(parts[i], ":");
                while (cache != NULL)
                {
                    printf("[DEBBUG] %i - %s\n", i, cache);
                    cache = strtok(NULL, ":");
                }  
            }  
        }
        printf("\n");
        j = 0;
    } 
    if(strcmp(method, "GET") == 0){
        get_response(client_socket);
    }
    return 0;
}

int http_request(char *get_recv, int client_socket)
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
    request_resolver(parts, i, client_socket);
    
    for (int j = 0; j < i; j++)
    {
        free(parts[j]);
    }
    free(line);
    
    return 0;
}

int http_request_recv(int client_socket)
{
    char *buffer = (char *)malloc(1024);
    int bytes_recv = recv(client_socket, buffer, 1024, 0);
    buffer[bytes_recv] = '\0';
    //printf("[DEBBUG] - %i\n\n", bytes_recv);
    if(strstr(buffer, "GET /"))
        http_request(buffer, client_socket);

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
    http_request_recv(client_sock);

    return 0;
}

int main()
{
    server_socket_open();
    
    return 0;
}
