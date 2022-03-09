#ifndef SOCKET_LINK_H_
#define SOCKET_LINK_H_
#include <arpa/inet.h>
#include <sys/socket.h>
//todo simple instance
class Socket_link{
private:
    enum link_state {CONNECTIONG, CONNECTED, FINISHED};
    enum {buf_length = 1024};
    int message_length;
    char buf[buf_length];
    short port;
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_adr;
    struct sockaddr_in clnt_adr;
    link_state state;
public:
    Socket_link(const short port);
    ~Socket_link();
    int Accept();
    void Read();
    void Http_analyse();
    void Write();
};
#endif 