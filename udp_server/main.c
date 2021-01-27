#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

void main(void){

    int err;
    int sock_desc;

    printf("UDP Socket server\r\n");

    sock_desc = socket(AF_INET,SOCK_DGRAM,0);
    if(sock_desc < 0){
        printf("socket() failed: %d\r\n",errno);
        return;
    }
    printf("socket() succeeded: %d\r\n",sock_desc);

    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_addr.s_addr = INADDR_ANY; // localhost
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    err = bind(sock_desc,(struct sockaddr *)&server,sizeof(server));
    if(err < 0){
        printf("bind() failed: %d\r\n",errno);
        return;
    }
    printf("bind() succeeded\r\n");

    struct sockaddr_in client;
    memset(&client,0,sizeof(client));
    int sizeofclient = sizeof(client);
    printf("Listening to incoming messages\r\n");
    do{
        char recv_msg[2048] = {0};
        err = recvfrom(sock_desc,recv_msg,sizeof(recv_msg),MSG_WAITALL,(struct sockaddr *)&client,&sizeofclient);
        if(err < 0){
            printf("recvfrom() failed: %d\r\n",errno);
            goto close_socket;
        }
        char *client_ip = inet_ntoa(client.sin_addr);
        int client_port = ntohs(client.sin_port);
        printf("recvfrom() returned %d. IP: %s, Port: %d Message: %s\r\n",err,client_ip, client_port,recv_msg);


        const char *send_msg = "Response from the UDP Server!\r\n";
        err = sendto(sock_desc,send_msg,strlen(send_msg),MSG_CONFIRM,(struct sockaddr *)&client,sizeofclient);
        if(err < 0){
            printf("sendto() failed: %d\r\n",errno);
            goto close_socket;
        }
        printf("sendto() %d bytes. msg: %s\r\n",err,send_msg);
    }while(err > 0);

    close_socket:
    err = close(sock_desc);
    if(err < 0){
        printf("close() failed: %d\r\n",errno);
        return;
    }
    printf("socket %d closed\r\n",sock_desc);

    return;
}