#ifndef EVENT_T_
#define EVENT_T_
#include "http.h"
class Events{
private:
    int epoll_fd;
public:
    Events();
    ~Events();
    void Process_events();
    void Create_server(int sock);
    void Create_client(int sock);
};
#endif