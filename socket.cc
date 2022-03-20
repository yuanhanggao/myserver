#include <unistd.h>
#include "socket.h"
#include "common.h"
#include "http.h"
Socket_link::Socket_link(){
    memset(&adr, 0, sizeof(adr));
    state = CONNECTIONG; 
}

Socket_link::~Socket_link(){
    close(sock);
}

Client_Socket_link::Client_Socket_link(const int client_sock){
    memset(buf, 0, buf_length);
    sock = client_sock;
    state = CONNECTED;
}

Client_Socket_link::~Client_Socket_link(){
}

void Client_Socket_link::Read(){
    if (state != CONNECTED)
        warn("not connected!");
    message_length = read(sock, buf, buf_length);
    if (message_length  < 0)
        error_handle("read failed!");
    if (message_length == 0){
        state = FINISHED;
        warn("finished!");
    }
}

void Client_Socket_link::Http_analyse(){
    char *tmp_buf;
    Http Socket_http(buf, message_length);
    Socket_http.Analyse_request();
    tmp_buf = Socket_http.Build_response(message_length);
    snprintf(buf, message_length, "%s",tmp_buf);
}

void Client_Socket_link::Write(){
    if (state == CONNECTED)
        write(sock, buf, message_length);
}

int Client_Socket_link::Get_sock(){
    return sock;
}

Server_Socket_link::Server_Socket_link(const short port){
    sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(port);
    if (bind(sock, (struct sockaddr*)&adr, sizeof(adr)) == -1)
        error_die("bind() error");
    if (listen(sock, 20) < 0){
        error_die("listen() error");
    }
}

Server_Socket_link::~Server_Socket_link(){
}

int Server_Socket_link::Accept(){
    socklen_t clnt_adr_size = sizeof(adr);
    int clnt_sock = accept(sock, (struct sockaddr*)&adr, &clnt_adr_size);
    printf("Connection Request: %s port: %d\n", 
        inet_ntoa(adr.sin_addr), ntohs(adr.sin_port));
    state = CONNECTED;
    return(clnt_sock);
}

int Server_Socket_link::Get_sock(){
    return sock;
}