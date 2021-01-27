#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

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

    while(true){
        client_sock_desc = accept(sock_desc,(struct sockaddr *)&client,(socklen_t *)&c);
        if(client_sock_desc < 0){
            printf("accept() failed: %d\r\n",errno);
            return;
        }
        printf("Connection accepted. Descriptor: %d\r\n",client_sock_desc);

        char *client_ip = inet_ntoa(client.sin_addr);
        int client_port = ntohs(client.sin_port);

        printf("Client IP Address: %s, Port: %d\r\n",client_ip,client_port);

        const char * send_msg = "Simple TCP server. Write a message:\r\n";

        err = send(client_sock_desc,send_msg,strlen(send_msg),0);
        if(err < 0){
            printf("send() failed: %d\r\n",errno);
            return;
        }

        char recv_msg[2048] = {0};
        err = recv(client_sock_desc,recv_msg,sizeof(recv_msg),0);
        if(err < 0){
            printf("recv() failed: %d\r\n",errno);
            return;
        }
        printf("Received message: %s\r\n",recv_msg);

        err = close(client_sock_desc);
        if(err < 0){
            printf("close() of socket %d failed: %d\r\n",client_sock_desc, errno);
            return;
        }
        printf("socket %d closed\r\n",client_sock_desc);
    }

    return;
}