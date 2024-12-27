#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>


#define PORT 8080
#define BUFFER_SIZE 4096

void handle_client(SOCKET client_socket)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    // Step 1: Receive the request
    recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    printf("Received request:\n%s\n", buffer);

    // Step 2: Parse the request line
    char method[16], path[256], protocol[16];
    sscanf(buffer, "%s %s %s", method, path, protocol);
    printf("Method: %s, Path: %s, Protocol: %s\n", method, path, protocol);

    // Step 3: Extract headers
    char *headers_start = strstr(buffer, "\r\n") + 2; // Start after request line
    char *headers_end = strstr(buffer, "\r\n\r\n");   // End of headers
    size_t headers_length = headers_end - headers_start;

    char headers[BUFFER_SIZE];
    strncpy(headers, headers_start, headers_length);
    headers[headers_length] = '\0';
    printf("Headers:\n%s\n", headers);

    // Step 4: Respond dynamically based on path
    if (strcmp(path, "/hello") == 0)
    {
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Hello, World!";
        send(client_socket, response, strlen(response), 0);
    }
    else if (strcmp(path, "/time") == 0)
    {
        char response[BUFFER_SIZE];
        time_t now = time(NULL);
        snprintf(response, sizeof(response),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/plain\r\n"
                 "\r\n"
                 "Current time: %s",
                 ctime(&now));
        send(client_socket, response, strlen(response), 0);
    }
    else
    {
        const char *response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Resource not found.";
        send(client_socket, response, strlen(response), 0);
    }

    // Step 5: Close the client socket
    closesocket(client_socket);
}

int main()
{
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    WSAStartup(MAKEWORD(2, 2), &wsa);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 3);

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket != INVALID_SOCKET)
        {
            handle_client(client_socket);
        }
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
