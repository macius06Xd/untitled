#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "List.h"

typedef struct client_desc
{
    int socket;
    int pipe_enter;
    int pipe_end;
} client_desc;
int compareSockets(void* l, void * r){
    client_desc L = *(client_desc *) l;
    client_desc R = *(client_desc *) r;

    if(L.socket==R.socket){
        return 0;
    }
    else{
        return 1;
    }
}

List* users;

int activeUsers = 0;

int sendMessage(void* user, int i, va_list args){
    client_desc sender = va_arg(args,client_desc);
    char* message = va_arg(args, char*);
    client_desc userdesc =  *(client_desc *)user;
    fprintf(stderr, "Writing to pipe\n");
    write(userdesc.pipe_enter,message,2000);

    return 0;
}

void dispatch_messages(client_desc sender, char message [2040])
{

    list_apply(users,sendMessage,2,sender,message);
}


void *connection_handler(void *desc) {

    /* Get the socket descriptor */
    client_desc descirptors = *(client_desc*)desc;
    int sock = descirptors.socket;
    fprintf(stderr,"socket: %d",sock);
    int pipe_start = descirptors.pipe_enter;
    int pipe_end = descirptors.pipe_end;
    int read_size;
    char *message , client_message[2000] , response[2040];
    char username [30];
    fd_set readfds;
    int maxfd;
    int ret;
    maxfd = (sock > pipe_end) ? sock : pipe_end;
    message = "Please provide me your username in the first message\n";
    send(sock , message , strlen(message), 0);
    read_size = recv(sock , client_message , 2000 , 0);
    fprintf(stderr,"ClientMessage %s \n", client_message);
    client_message[read_size] = '\0';
    strcpy(username,client_message);

    /* Send some messages to the client */
    message = "Greetings!";
    send(sock , message , strlen(message), 0);

    do {
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        FD_SET(pipe_end, &readfds);
        ret = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (FD_ISSET(sock, &readfds)) {
            read_size = recv(sock, response, 2000, 0);
            fprintf(stderr, "ClientMessage %s \n", response);
            client_message[read_size] = '\0';
            strcat(response, username);
            strcat(response, ": ");
            strcat(response, client_message);
            /* Send the message back to client */
            dispatch_messages(descirptors, response);
        }
        else if (FD_ISSET(pipe_end, &readfds)) {
            char redirect [2000];
            read_size = read(pipe_end, redirect, sizeof(redirect));
            fprintf(stderr, "redirectedMessage %s \n", redirect);
            send( sock, redirect, strlen(redirect), 0);
        }

        /* Clear the message buffer */
        memset(client_message, 0, 2000);
        memset(response, 0, 2040);
    } while(read_size > 0); /* Wait for empty line */

    fprintf(stderr, "Client disconnected\n");
    remove_from_list(users, (void *) &sock);
    close(sock);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    int listenfd = 0;
    struct sockaddr_in serv_addr;
    users = create_list(NULL, compareSockets, sizeof(client_desc));
    pthread_t thread_id;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    for (;;) {
        int *  connfd  = malloc(sizeof (int));
        *connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        int * fd = malloc(sizeof(int)*2);
        pipe(fd);
        client_desc*  desc =  malloc(sizeof(client_desc));
        desc->socket = *connfd;
        desc->pipe_enter = fd[1];
        desc->pipe_end = fd[0];
        fprintf(stderr , "user %d\n",*connfd);
        list_add(users, (void *)fd);
        activeUsers++;
        fprintf(stderr, "Connection accepted\n");
        pthread_create(&thread_id, NULL, connection_handler , (void *)desc);
    }

    return 0;
}
