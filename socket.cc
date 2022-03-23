#include <unistd.h>
#include <fcntl.h>
#include "socket.h"
#include "common.h"
#include "http.h"

Socket_link::Socket_link(){
    memset(&adr, 0, sizeof(adr));
    state = CONNECTIONG; 
}

Socket_link::~Socket_link(){
    printf("close fd\n");
    close(sock);
}

int Socket_link::Set_block(int mode){
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0)
        return errno;
    if (mode)
        return fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    return fcntl(sock, F_SETFL, flags | ~O_NONBLOCK);
}

void Client_Socket_link::_create(const int client_sock){
    memset(buf, 0, buf_length);
    sock = client_sock;
    state = CONNECTED;
}

Client_Socket_link::Client_Socket_link(const int client_sock){
    _create(client_sock);
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

Server_Socket_link::Server_Socket_link(const short port, bool _is_block){
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

    Set_block(_is_block); 
}

Server_Socket_link::~Server_Socket_link(){
}

int Server_Socket_link::Accept(){
    socklen_t clnt_adr_size = sizeof(adr);
    int clnt_sock = accept(sock, (struct sockaddr*)&adr, &clnt_adr_size);
    printf("Connection Request: %s port: %d\n", 
        inet_ntoa(adr.sin_addr), ntohs(adr.sin_port));
    state = CONNECTED;
    return clnt_sock;
}

int Server_Socket_link::Get_sock(){
    return sock;
}