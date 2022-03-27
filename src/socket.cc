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
    dzlog_error("close fd");
    close(sock);
}

int Socket_link::Set_nonblock(bool mode){
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

Client_Socket_link::Client_Socket_link(const int client_sock, bool _is_nonblock){
    _create(client_sock);
    Set_nonblock(_is_nonblock);
}

Client_Socket_link::~Client_Socket_link(){
}

void Client_Socket_link::Read(){
    if (state != CONNECTED)
        dzlog_warn("not connected!");
    message_length = read(sock, buf, buf_length);
    if (message_length  < 0)
        dzlog_error("read failed!");
    if (message_length == 0){
        state = FINISHED;
        dzlog_warn("finished!");
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

void Server_Socket_link::_create(const short port){
    sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(port);
    if (bind(sock, (struct sockaddr*)&adr, sizeof(adr)) == -1){
        dzlog_error("bind() error!");
		exit(-1);
	}
    if (listen(sock, 20) < 0){
        dzlog_error("listen() error!");
		exit(-1);
    }
}

Server_Socket_link::Server_Socket_link(const short port, bool _is_nonblock){
    _create(port);
    Set_nonblock(_is_nonblock); 
}

Server_Socket_link::~Server_Socket_link(){
}

int Server_Socket_link::Accept(){
    socklen_t clnt_adr_size = sizeof(adr);
    int clnt_sock = accept(sock, (struct sockaddr*)&adr, &clnt_adr_size);
    dzlog_info("Connection Request: %s port: %d", 
           inet_ntoa(adr.sin_addr), ntohs(adr.sin_port));
    state = CONNECTED;
    return clnt_sock;
}

int Server_Socket_link::Get_sock(){
    return sock;
}
