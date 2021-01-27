#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

static int testvar = 0;

void *new_socket_thread_fn(void *socket){

    int err = -1;
    int client_sock_desc = *(int *)socket;
    
    /* Debug only to test watchpoints here */
    if(client_sock_desc > 7){
        testvar++;
    }

    const char * send_msg = "Continuous TCP server (echo). Write a message:\r\n";

    err = send(client_sock_desc,send_msg,strlen(send_msg),0);
    if(err < 0){
        printf("send() failed: %d\r\n",errno);
        goto socket_close;
    }
        printf("send() to socket: %d succeeded\r\n",client_sock_desc);

    /* Echo the contents of the message received */
    do{
        char recv_msg[2048] = {0};
        err = recv(client_sock_desc,recv_msg,sizeof(recv_msg),0);
        if(err < 0){
            printf("recv() failed: %d\r\n",errno);
            break;
        }
        printf("Received message for socket %d: %s\r\n",client_sock_desc,recv_msg);
        err = send(client_sock_desc, recv_msg, strlen(recv_msg),0);
        if(err < 0){
            printf("send() failed: %d\r\n",errno);
            break;
        }
        printf("send() echo to socket: %d succeeded\r\n",client_sock_desc);
    } while(err > 0);

    socket_close:
    err = close(client_sock_desc);
    if(err < 0){
        printf("close() of socket %d failed: %d\r\n",client_sock_desc, errno);
    }
    else{
        printf("socket %d closed\r\n",client_sock_desc);
    }    
    pthread_exit(0);
}

void main(void){

    int err;

    printf("TCP Socket loopback demo - Server\r\n");

    int sock_desc = -1;

    sock_desc = socket(AF_INET,SOCK_STREAM,0);
    if(sock_desc < 0){
        printf("socket() creation failed: %d\r\n",errno);
        return;
    }
    printf("socket() created successfully. Descriptor: %d\r\n",sock_desc);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);
    server.sin_addr.s_addr = INADDR_ANY;

    err = bind(sock_desc,(struct sockaddr *)&server, sizeof(server));
    if(err < 0){
        printf("socket %d failed to bind: %d\r\n",sock_desc, errno);
        return;
    }
    printf("socket %d bound successfully\r\n",sock_desc);

    err = listen(sock_desc,3);
    if(err < 0){
        printf("listen() failed: %d\r\n",errno);
        return;
    }
    printf("listening to incoming connections...\r\n");

    struct sockaddr_in client;
    int c = sizeof(struct sockaddr_in);
    int client_sock_desc;

    do{
        client_sock_desc = accept(sock_desc,(struct sockaddr *)&client,(socklen_t *)&c);
        if(client_sock_desc < 0){
            printf("accept() failed: %d\r\n",errno);
            return;
        }
        printf("Connection accepted. Descriptor: %d\r\n",client_sock_desc);

        char *client_ip = inet_ntoa(client.sin_addr);
        int client_port = ntohs(client.sin_port);

        printf("Client IP Address: %s, Port: %d\r\n",client_ip,client_port);

        int *new_sock = malloc(1);
        *new_sock = client_sock_desc;

        pthread_t new_socket_thread;
        err = pthread_create(&new_socket_thread,NULL,new_socket_thread_fn,(void *)new_sock);
        if(err != 0){
            printf("pthread_create() failed: %d \r\n",err);   
            return;
        }
        printf("pthread_create() successfully for socket %d\r\n",*new_sock);

    } while (client_sock_desc > 0);

    return;
}