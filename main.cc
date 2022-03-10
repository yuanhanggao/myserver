#include "socket.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv){
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    Server_Socket_link Server_Socket(atoi(argv[1]));
    while(1){
        int client_sock;
        client_sock = Server_Socket.Accept();
        Client_Socket_link Client_Sock(client_sock);
        Client_Sock.Read();
        Client_Sock.Http_analyse();
        Client_Sock.Write();
    }
    return(0);
}
