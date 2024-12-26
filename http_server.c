#include <stdio.h>    // Standard Input/Output for printf()
#include <stdlib.h>   // Standard library for general utilities like memory allocation
#include <string.h>   // For string operations like memset() and strlen()
#include <winsock2.h> // Windows Sockets API for network programming

#pragma comment(lib, "ws2_32.lib") // Links the Winsock library during compilation

#define PORT 8080        // The port number where the server will listen for incoming connections
#define BUFFER_SIZE 1024 // Size of the buffer used to store incoming data from clients

int main()
{
    WSADATA wsa;                                 // Structure to hold Winsock data for initialization
    SOCKET server_socket, client_socket;         // Sockets for server and client
    struct sockaddr_in server_addr, client_addr; // Structures to store address information for server and client
    int addr_len = sizeof(client_addr);          // Size of the client address structure, required for accept()
    char buffer[BUFFER_SIZE];                    // Buffer to store client requests

    // Step 1: Initialize Winsock
    // Initialize the Winsock library, checking for success
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Step 2: Create a socket
    printf("Creating socket...\n");

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Step 3: Prepare the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Step 4: Bind the socket to the specified IP and port
    printf("Binding socket...\n");
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Step 5: Start listening for incoming connections
    printf("Listening on port %d...\n", PORT);
    if (listen(server_socket, 3) == SOCKET_ERROR)
    {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Step 6: Keep accepting connections in an infinite loop
    while (1)
    
    {
        printf("Waiting for incoming connections...\n");

        // Accept a new client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == INVALID_SOCKET)
        {
            printf("Accept failed. Error Code: %d\n", WSAGetLastError());
            continue; // Skip this iteration and continue accepting connections
        }

        printf("Connection accepted.\n");

        // Step 7: Handle the client request
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        printf("Received request:\n%s\n", buffer);

        // Step 8: Send an HTTP response
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Hello, World!";
        send(client_socket, response, strlen(response), 0);
        printf("Response sent.\n");

        // Step 9: Close the client socket after sending the response
        closesocket(client_socket);
    }

    // Step 10: Cleanup Winsock (This will never be reached due to the infinite loop)
    WSACleanup();
    printf("Server shutdown.\n");

    return 0;
}
