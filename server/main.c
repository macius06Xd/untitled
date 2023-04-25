#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "List.h"

int compareSockets(void* l, void * r){
    int L = *(int*) l;
    int R = *(int*) r;

    if(L==R){
        return 0;
    }
    else{
        return 1;
    }
}

List* users;

int activeUsers = 0;

int sendMessage(void* user, int i, va_list args){
    int sender = va_arg(args,int);
    char* message = va_arg(args, char*);
    if(sender != *(int*)user) {
        send(*(int *) user, message, strlen(message), 0);
    }
    return 0;
}

void dispatch_messages(int sender, char message [2040])
{
    list_apply(users,sendMessage,2,sender,message);
}


void *connection_handler(void *socket_desc) {

    /* Get the socket descriptor */
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000] , response[2040];
    char username [30];

    message = "Please provide me your username in the first message\n";
    send(sock , message , strlen(message), 0);
    read_size = recv(sock , client_message , 2000 , 0);
    client_message[read_size] = '\0';
    strcpy(username,client_message);

    /* Send some messages to the client */
    message = "Greetings!";
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
    remove_from_list(users, (void *) &sock);
    close(sock);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    users = create_list(NULL, compareSockets, sizeof(connfd));
    pthread_t thread_id;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    for (;;) {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        list_add(users, (void *) &connfd);
        activeUsers++;
        fprintf(stderr, "Connection accepted\n");
        pthread_create(&thread_id, NULL, connection_handler , (void *) &connfd);
    }

    return 0;
}
