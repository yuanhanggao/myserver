#ifndef EVENT_T_
#define EVENT_T_
#include "socket.h"
#include "threadpool.h"

enum {MAX_FDS_NUM = 10240, UNIT = 512};

class Event{
public:
    int epoll_fd;
    int fd;
    Event();
    virtual ~Event();
    virtual int Do_socket_process();
};

class Server_event: public Event{
private:
    Server_Socket_link *socket;
public:
    Server_event(const short port, bool is_nonblock, int _epoll_fd);
    ~Server_event();
    int Do_socket_process();
};

class Client_event: public Event{
private:
    Client_Socket_link *socket; 
public:
    Client_event(int _fd, bool is_nonblock, int _epoll_fd);
    ~Client_event();
    int Do_socket_process();
};

class Fdlist{
private:
    int fds[MAX_FDS_NUM];
    pthread_mutex_t fd_list_lock;
    int pos;
public:
    Fdlist();
    ~Fdlist();
    int Get_fds(int *fds, int num);
    void Put_fds(int *fds, int num);
};

class Events{
private:
    int server_epoll_fd;
    enum {MAX_EVENT_NUM = 100};
    Thread_pool *thread_pool;
    Event *server_event;
    Fdlist *fdlist;
    static void* Thread_process_event(void *_events);
public:
    static void Destroy_event(Event *event, int epoll_fd);
    static void Add_event(Event *event, int epoll_fd);
    Events(const short port);
    ~Events();
    int Get_server_epoll_fd();
    void Process_events();
};

#endif