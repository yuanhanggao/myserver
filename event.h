#ifndef EVENT_T_
#define EVENT_T_
#include "socket.h"
#include "threadpool.h"

enum {MAX_FDS_NUM = 10240, UNIT = 512};
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
    __thread static int epoll_fd;
    enum {MAX_EVENT_NUM = 100};
    struct epoll_event events[MAX_EVENT_NUM];
    Thread_pool *thread_pool;
    Event *server_event;
    Fdlist *fdlist;
    void Add_event(Event *event);
    void Destroy_event(Event *event);
    static void* T_process_event(Events *_events);
public:
    Events(const short port);
    ~Events();
    static int Get_epoll_fd();
    void Process_events();
};

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
    Server_event(const short port, int _epoll_fd);
    virtual ~Server_event();
    int Do_socket_process();
};

class Client_event: public Event{
private:
    Client_Socket_link *socket; 
public:
    Client_event(int _fd, int _epoll_fd);
    virtual ~Client_event();
    int Do_socket_process();
};

#endif