#include "event.h"
#include "common.h"
#include <sys/epoll.h>
Events::Events(){
    epoll_fd = epoll_create(256);
}

Events::~Events(){
    close(epoll_fd);
}

void Events::Create_client(int sock){
    struct epoll_event ev; 
    ev.data.fd = sock;
    ev.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev);
}

void Events::Create_server(int sock){
    struct epoll_event ev;
    ev.data.fd = sock;
    ev.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev);
}