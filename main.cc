#include "socket.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv){
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    Socket_link Socket(atoi(argv[1]));
    Socket.Accept();
    while(1){
        Socket.Read();
        Socket.Http_analyse();
        Socket.Write();
    }
    return(0);
}