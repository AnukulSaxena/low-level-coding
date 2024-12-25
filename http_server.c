#include <stdio.h> // Standard Input/Output for printf()
#include <stdlib.h> // Standard library for general utilities like memory allocation
#include <string.h> // For string operations like memset() and strlen()
#include <winsock2.h> // Windows Sockets API for network programming

#pragma comment(lib, "ws2_32.lib") // Links the Winsock library during compilation

#define PORT 8080 // The port number where the server will listen for incoming connections
#define BUFFER_SIZE 1024 // Size of the buffer used to store incoming data from clients

int main() {
    WSADATA wsa; // Structure to hold Winsock data for initialization
    SOCKET server_socket, client_socket; // Sockets for server and client
    struct sockaddr_in server_addr, client_addr; // Structures to store address information for server and client
    int addr_len = sizeof(client_addr); // Size of the client address structure, required for accept()
    char buffer[BUFFER_SIZE]; // Buffer to store client requests

    // Step 1: Initialize Winsock
    printf("Initializing Winsock...\n");
    // WSAStartup initializes the Winsock library. MAKEWORD(2, 2) specifies version 2.2
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        // WSAGetLastError() retrieves the error code if initialization fails
        return 1; // Exit with failure status
    }

    // Step 2: Create a socket
    printf("Creating socket...\n");
    // AF_INET specifies IPv4, SOCK_STREAM specifies TCP, and 0 lets the OS decide the protocol
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        return 1; // Exit if socket creation fails
    }

    // Step 3: Prepare the server address structure
    // sin_family specifies the address family (IPv4)
    server_addr.sin_family = AF_INET;
    // INADDR_ANY allows the server to listen on any network interface
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // htons() converts the port number from host byte order to network byte order
    server_addr.sin_port = htons(PORT);

    // Step 4: Bind the socket to the specified IP and port
    printf("Binding socket...\n");
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        return 1; // Exit if bind fails
    }

    // Step 5: Start listening for incoming connections
    printf("Listening on port %d...\n", PORT);
    // The second argument (3) specifies the maximum number of pending connections in the queue
    if (listen(server_socket, 3) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        return 1; // Exit if listen fails
    }

    // Step 6: Accept a connection from a client
    printf("Waiting for incoming connections...\n");
    // accept() blocks and waits for an incoming connection. It returns a new socket for the client
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", WSAGetLastError());
        return 1; // Exit if accept fails
    }
    printf("Connection accepted.\n");

    // Step 7: Handle the client request
    // Clear the buffer to ensure no leftover data
    memset(buffer, 0, BUFFER_SIZE);
    // recv() receives data from the client and stores it in the buffer
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    // Print the request for debugging purposes
    printf("Received request:\n%s\n", buffer);

    // Step 8: Send an HTTP response
    // The response is a minimal HTTP message with a 200 OK status and plain text content
    const char* response =
        "HTTP/1.1 200 OK\r\n"           // HTTP version and status code
        "Content-Type: text/plain\r\n"  // Specifies the type of content (plain text)
        "Content-Length: 13\r\n"        // Length of the response body
        "\r\n"                          // Empty line separates headers and body
        "Hello, World!";                // Response body
    // send() sends the response to the client
    send(client_socket, response, strlen(response), 0);
    printf("Response sent.\n");

    // Step 9: Close sockets
    // closesocket() releases the client socket
    closesocket(client_socket);
    // closesocket() releases the server socket
    closesocket(server_socket);

    // Step 10: Cleanup Winsock
    // WSACleanup() deinitializes the Winsock library
    WSACleanup();
    printf("Server shutdown.\n");

    return 0; // Exit with success status
}
