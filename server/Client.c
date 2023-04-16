#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#pragma comment(lib,"ws2_32.lib")

void *send_messages(void *arg);
void *receive_messages(void *arg);

int main(int argc, char *argv[])
{
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char message[2000], server_reply[2000];
    int read_size;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("WSAStartup failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        return 1;
    }

    // Server address
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(5000);

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connection error");
        return 1;
    }



    pthread_t tid[2];

    // Start send_messages thread
    if (pthread_create(&tid[0], NULL, send_messages, &sock)) {
        printf("Error creating send_messages thread\n");
        return 1;
    }

    // Start receive_messages thread
    if (pthread_create(&tid[1], NULL, receive_messages, &sock)) {
        printf("Error creating receive_messages thread\n");
        return 1;
    }

    // Wait for threads to finish
    for (int i = 0; i < 2; i++) {
        pthread_join(tid[i], NULL);
    }

    // Close socket
    closesocket(sock);

    return 0;
}

void *send_messages(void *arg) {
    int sock = *(int *)arg;
    char message[2000];

    while (1) {
        //printf("Enter message: ");
        fgets(message, 2000, stdin);
        // Send message to server
        send(sock, message, strlen(message), 0);
    }

    return NULL;
}

void *receive_messages(void *arg) {
    int sock = *(int *)arg;
    char server_reply[2000];
    int read_size;

    while (1) {
        // Receive message from server
        read_size = recv(sock, server_reply, 2000, 0);
        server_reply[read_size] = '\0';
        printf("%s", server_reply);
        if (read_size <= 2) {
            break;
        }
    }

    return NULL;
}