#include <string.h>
#include <unistd.h>
#include "socket.h"
#include "common.h"
#include "http.h"
Socket_link::Socket_link(const short port){
    state = CONNECTIONG; 
    memset(buf, 0, buf_length);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(port);
    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_die("bind() error");
    if (listen(serv_sock, 20) < 0){
        error_die("listen() error");
    }
}
Socket_link::~Socket_link(){
    close(serv_sock);
    close(clnt_sock);
}

int Socket_link::Accept(){
    socklen_t clnt_adr_size = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
    printf("Connection Request: %s port: %d\n", 
        inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
    state = CONNECTED;
    return(clnt_sock);
}

void Socket_link::Read(){
    if (state != CONNECTED)
        warn("not connected!");
    message_length = read(clnt_sock, buf, buf_length);
    if (message_length  < 0)
        error_handle("read failed!");
    if (message_length == 0){
        state = FINISHED;
        warn("finished!");
    }
}
void Socket_link::Http_analyse(){
    char *tmp_buf;
    Http Socket_http(buf, message_length);
    Socket_http.Analyse_request();
    tmp_buf = Socket_http.Build_response(message_length);
    snprintf(buf, message_length, "%s",tmp_buf);
}
void Socket_link::Write(){
    if (state == CONNECTED)
        write(clnt_sock, buf, message_length);
}