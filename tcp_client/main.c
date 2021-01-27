#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
// #include <sys/types.h>
// #include <netdb.h>
#include <string.h>

void main(void){

    int err;

    printf("TCP Socket loopback demo - Cient\r\n");

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
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost

    err = connect(sock_desc,(struct sockaddr *)&server, sizeof(server) );
    if(err < 0){
        printf("connect() failed: %d\r\n", errno);
        return;
    }
    printf("socket() %d connected successfully\r\n",sock_desc);

    const char *send_msg = "Client Socket sending message!!\r\n";
    err = send(sock_desc,send_msg,strlen(send_msg),0);
    if(err < 0){
        printf("send() failed: %d\r\n", errno);
        return;
    }
    printf("sent message successfully through socket %d\r\n",sock_desc);

    char recv_msg[2048] = {0};
    err = recv(sock_desc,recv_msg,sizeof(recv_msg),0);
    if(err < 0){
        printf("recv() failed: %d\r\n", errno);
        return;
    }
    printf("received message: %s\r\n",recv_msg);

    err = close(sock_desc);
    if(err < 0){
        printf("close() of socket %d failed: %d\r\n",sock_desc, errno);
        return;
    }
    printf("socket %d closed\r\n",sock_desc);

    return;
}