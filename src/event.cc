#include "event.h"
#include "common.h"
#include <sys/epoll.h>


Event::Event(){
}

Event::~Event(){
    dzlog_debug("delete the event!\n");
}

int Event::Do_socket_process(){
	return 0;
}

Server_event::Server_event(const short port, bool is_nonblock, int server_epoll_fd){
    socket = new Server_Socket_link(port, is_nonblock);
    fd= socket->Get_sock();
    epoll_fd = server_epoll_fd;
}

Server_event::~Server_event(){
    dzlog_debug("delete the server_event!");
    delete socket;
}

int Server_event::Do_socket_process(){
    return socket->Accept();
}

Client_event::Client_event(int _fd, bool is_nonblock, int client_epoll_fd){
    fd = _fd;
    socket = new Client_Socket_link(_fd, is_nonblock);
    epoll_fd = client_epoll_fd;
}

Client_event::~Client_event(){
    dzlog_debug("delete the client_event!");
    delete socket;
}

int Client_event::Do_socket_process(){
    socket->Read();
    socket->Http_analyse();
    socket->Write();
    return 0;
}

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
    server_epoll_fd = epoll_create(256);
    thread_pool = new Thread_pool(10, 100, 100);
    server_event = new Server_event(port, true, server_epoll_fd);
    Add_event(static_cast<Event *>(server_event), server_epoll_fd);
    fdlist = new Fdlist();
}

Events::~Events(){
    Destroy_event(server_event, server_epoll_fd);
    close(server_epoll_fd);
    delete thread_pool;
    delete server_event;
    delete fdlist;
}

int Events::Get_server_epoll_fd(){
    return server_epoll_fd;
}

void Events::Process_events(){
    int nfds, i;
    int ret;
    struct epoll_event events[MAX_EVENT_NUM];
    while(1){
        nfds = epoll_wait(server_epoll_fd, events, MAX_EVENT_NUM, -1);
        for (i = 0; i < nfds; i++){
            int fds[UNIT];
            int j = 0;
            Event *event = static_cast<Event *>(events[i].data.ptr);
            while ((fds[j] = event->Do_socket_process()) > 0) {
                j++;
                if (j == UNIT) {
                    dzlog_debug("put %d fd!", j);
                    fdlist->Put_fds(fds, j);
                    j = 0;
                }
            }

            if (j != 0){
                dzlog_debug("put %d fd!", j);
                fdlist->Put_fds(fds, j);
            }

            thread_pool->Add_task(Thread_process_event,
                                  static_cast<void *>(this));
        }
    }
}

void Events::Add_event(Event *event, int epoll_fd){
    struct epoll_event ev; 
    ev.events = EPOLLIN;
    ev.data.fd = event->fd;
    ev.data.ptr = static_cast<void *>(event);
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event->fd, &ev);
}


void Events::Destroy_event(Event *event, int epoll_fd){
    struct epoll_event ev; 
    ev.events = EPOLLIN;
    ev.data.fd = event->fd;
    ev.data.ptr = static_cast<void *>(event);
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event->fd, &ev);
}

void* Events::Thread_process_event(void *_events){

    Events *events = static_cast<Events *>(_events);
    int client_epoll_fd = epoll_create(256);
    dzlog_error("client_epoll_fd is %d, the errno is %d\n", client_epoll_fd, errno);
    if (!client_epoll_fd) {
        dzlog_error("create client_epoll_fd failed!");    
		exit(-1);
    }
     
    int i, num;
    int fds[UNIT];
    num = events->fdlist->Get_fds(fds, UNIT);
    dzlog_debug("after get_fds, the num is %d\n", num);
    for (i = 0; i < num; i++){
        Event *client_event = new Client_event(fds[i], false, client_epoll_fd);
        Add_event(client_event, client_epoll_fd);
    }

    struct epoll_event client_events[MAX_EVENT_NUM];
    num = epoll_wait(client_epoll_fd, client_events, MAX_EVENT_NUM, -1);
    for (i = 0; i < num; i++){
        if (client_events[i].events & (EPOLLIN|EPOLLOUT)){
            Event* event = 
                static_cast<Client_event *> (client_events[i].data.ptr);
                event->Do_socket_process();
            event->~Event();
        }
        else
            dzlog_notice("unknown event = %x!\n", client_events[i].events);
    }
    close(client_epoll_fd);
	return NULL;
}
