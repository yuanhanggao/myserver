#ifndef SOCKET_LINK_H_
#define SOCKET_LINK_H_
#include <arpa/inet.h>
#include <sys/socket.h>

class Socket_link{
public:
    enum link_state {CONNECTIONG, CONNECTED, FINISHED};
    int sock;
    struct sockaddr_in adr;
    link_state state;
    bool is_block;

    Socket_link();
    ~Socket_link();
    int Set_block(int mode); 
};

class Client_Socket_link: private Socket_link{
private:
    enum {buf_length = 1024};
    int message_length;
    char buf[buf_length];
    void _create(const int client_sock);
public:
    Client_Socket_link(const int client_sock);
    Client_Socket_link(const int client_sock, bool _is_block);
    ~Client_Socket_link();
    int Get_sock();
    void Read();
    void Http_analyse();
    void Write();
};

class Server_Socket_link: private Socket_link{
private:
    int port;
    void _create(const short port);
public:
    Server_Socket_link(const short port);
    Server_Socket_link(const short port, bool _is_block);
    ~Server_Socket_link();
    int Get_sock();
    int Accept();
};
#endif 
