#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

void main(void){

    int err;
    int sock_desc;

    printf("UDP Socket client\r\n");

    sock_desc = socket(AF_INET,SOCK_DGRAM,0);
    if(sock_desc < 0){
        printf("socket() failed: %d\r\n",errno);
        return;
    }
    printf("socket() succeeded: %d\r\n",sock_desc);

    struct sockaddr_in server;
    server.sin_addr.s_addr = INADDR_ANY; // localhost
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    const char * send_msg = "Hello World! UDP Packet\r\n";
    err = sendto(sock_desc,send_msg,strlen(send_msg),MSG_CONFIRM,(struct sockaddr *)&server, sizeof(server) );
    if(err < 0){
        printf("sendto() failed: %d\r\n",errno);
        return;
    }
    printf("sendto() %d bytes. msg: %s\r\n",err,send_msg);

    char recv_msg[2048] = {0};
    int sizeofserver = 0;
    printf("receiving response...\r\n");
    err = recvfrom(sock_desc,recv_msg,sizeof(recv_msg),MSG_WAITALL,(struct sockaddr *)&server,&sizeofserver);
    if(err < 0){
        printf("recvfrom() failed: %d\r\n",errno);
        return;
    }
    printf("recvfrom() returned %d. Message: %s\r\n",err,recv_msg);

    err = close(sock_desc);
    if(err < 0){
        printf("close() failed: %d\r\n",errno);
        return;
    }
    printf("socket %d closed\r\n",sock_desc);

    return;
}