#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "List.h"

#pragma comment(lib,"ws2_32.lib")

int compareSockets(void* l, void * r){
    SOCKET L = *(SOCKET*) l;
    SOCKET R = *(SOCKET*) r;

    if(L==R){
        return 0;
    }
    else{
        return 1;
    }
}

List* users;

int activeUsers = 0;
void dispatch_messages(SOCKET sender, char message [2040])
{
    Node * iterator = users->head;
    while(iterator!=NULL){
        send(iterator , message , strlen(message), 0);
        iterator=iterator->next;
    }
}

void *
connection_handler(void *socket_desc) {

    /* Get the socket descriptor */
    SOCKET sock = *(SOCKET*)socket_desc;
    int read_size;
    char *message , client_message[2000] , response[2040];
    char username [30];

    message = "Please provide me your username\n";
    send(sock , message , strlen(message), 0);
    read_size = recv(sock , client_message , 2000 , 0);
    client_message[read_size] = '\0';
    strcpy(username,client_message);

    /* Send some messages to the client */
    message = "Greetings! I am your connection handler\n Now type something and i shall repeat what you type\n Empty line will close the connection\n";
    send(sock , message , strlen(message), 0);

    do {
        read_size = recv(sock , client_message , 2000 , 0);
        client_message[read_size] = '\0';
        strcat(response, username);
        strcat(response , ": ");
        strcat(response , client_message);
        /* Send the message back to client */
        dispatch_messages(sock,response);

        /* Clear the message buffer */
        memset(client_message, 0, 2000);
        memset(response, 0, 2040);
    } while(read_size > 2); /* Wait for empty line */

    fprintf(stderr, "Client disconnected\n");
    remove_from_list(users, sock);
    closesocket(sock);

    pthread_exit(NULL);
}

void sendMessage(void* user, int i, va_list args){
    char message[2040] = va_arg(args, char*);
    send(*(SOCKET*)user , message , strlen(message), 0);
}

int main(int argc, char *argv[]) {
    users = create_list(NULL, compareSockets, sizeof(SOCKET));
    WSADATA wsa;
    SOCKET listenfd = INVALID_SOCKET, connfd = INVALID_SOCKET;
    struct sockaddr_in serv_addr;

    pthread_t thread_id;

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("WSAStartup failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    for (;;) {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        //users[activeUsers] = connfd;
        list_add(users, connfd);
        activeUsers++;
        fprintf(stderr, "Connection accepted\n");
        pthread_create(&thread_id, NULL, connection_handler , (void *) &connfd);
    }

    WSACleanup();
    return 0;
}
