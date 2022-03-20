#include "event.h"
#include "common.h"
#include <sys/epoll.h>
Fdlist::Fdlist(){
    pos = 0;
    memset(fds, 0, sizeof(int) * MAX_FDS_NUM);
    pthread_mutex_init(&fd_list_lock, NULL);
}

Fdlist::~Fdlist(){
    pthread_mutex_destroy(&fd_list_lock);
}

void Fdlist::Put_fds(int *_fds, int num){
    int i;
    pthread_mutex_lock(&fd_list_lock);
    for (i = 0;i < num;i++){
        if (pos < MAX_FDS_NUM)
            fds[pos ++] = _fds[i];
        else
            break;
    }
    pthread_mutex_unlock(&fd_list_lock);
    // close fd?
//    for (i = 0;i < num;i++)
//        close(fds[i]);
}

int Fdlist::Get_fds(int *_fds, int num){
    int i;
    pthread_mutex_lock(&fd_list_lock);
    for (i = 0;i < num;i++){
        if (pos < 1)
            break;
        _fds[i] = fds[--pos];
    }
    pthread_mutex_unlock(&fd_list_lock);
    return i;
}

Events::Events(const short port){
    epoll_fd = epoll_create(256);
    thread_pool = new Thread_pool(10, 100, 100);
    server_event = new Server_event(port, epoll_fd);
    fdlist = new Fdlist();
}

Events::~Events(){
    close(epoll_fd);
    delete thread_pool;
    delete server_event;
    delete fdlist;
}

int Events::Get_epoll_fd(){
    return epoll_fd;
}

void Events::Process_events(){
    int nfds, i;
    int ret;
    while(1){
        nfds = epoll_wait(epoll_fd, events, MAX_EVENT_NUM, 500);
        for (i = 0; i < nfds; i++){
            int fds[UNIT];
            Event *event = static_cast<Event *>(events[i].data.ptr);
            ret = event->Do_socket_process(); 
            // server
            if (ret){
                event = new Client_event(ret, epoll_fd); 
                Add_event(event);
            } else {
                Destroy_event(event);
                event->~Event(); 
            }
        }
    }
}

void Events::Add_event(Event *event){
    struct epoll_event ev; 
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = event->fd;
    ev.events = EPOLLIN;
    ev.data.ptr = static_cast<void *>(event);
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev);
}

void* Events::T_process_event(Events *_events){
    Events *events = _events;

}

void Events::Destroy_event(Event *event){
    struct epoll_event ev; 
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = event->fd;
    ev.events = EPOLLIN;
    ev.data.ptr = static_cast<void *>(event);
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev);
}

Event::Event(){
}

Event::~Event(){
}

int Event::Do_socket_process(){
}

Server_event::Server_event(const short port, int _epoll_fd){
    socket = new Server_Socket_link(port);
    fd= socket->Get_sock();
    epoll_fd = _epoll_fd;
}

Server_event::~Server_event(){
    delete socket;
}

int Server_event::Do_socket_process(){
    return socket->Accept();
}

Client_event::Client_event(int _fd, int _epoll_fd){
    fd = _fd;
    socket = new Client_Socket_link(_fd);
    epoll_fd = _epoll_fd;
}

Client_event::~Client_event(){
    delete socket;
}

int Client_event::Do_socket_process(){
    socket->Read();
    socket->Http_analyse();
    socket->Write();
    return 0;
}