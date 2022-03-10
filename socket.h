#ifndef SOCKET_LINK_H_
#define SOCKET_LINK_H_
#include <arpa/inet.h>
#include <sys/socket.h>
//todo simple instance
class Socket_link{
public:
    enum link_state {CONNECTIONG, CONNECTED, FINISHED};
    int sock;
    struct sockaddr_in adr;
    link_state state;
    
    Socket_link();
    ~Socket_link();
};

class Client_Socket_link: private Socket_link{
private:
    enum {buf_length = 1024};
    int message_length;
    char buf[buf_length];
public:
    Client_Socket_link(const int client_sock);
    ~Client_Socket_link();
    void Read();
    void Http_analyse();
    void Write();
};

class Server_Socket_link: private Socket_link{
    int port;	
public:
    Server_Socket_link(const short port);
    ~Server_Socket_link();
    int Accept();
};
#endif 
