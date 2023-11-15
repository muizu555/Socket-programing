#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>

void handle_client(int client_socket) {
    char response_header[] = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html; charset=utf-8\r\n"
                            "\r\n";
    send(client_socket, response_header, sizeof(response_header) - 1, 0);

    char buffer[1024];
    FILE *html_file = fopen("muizu.html", "r"); 
    if (html_file == NULL) {
        perror("Error opening the HTML file");
    } else {
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), html_file)) > 0) {
            send(client_socket, buffer, bytes_read, 0);
        }
        fclose(html_file);
    }

    close(client_socket);
}


int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port 8080...\n");

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }
        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}
