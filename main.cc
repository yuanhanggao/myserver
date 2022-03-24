#include "socket.h"
#include "event.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>

int main(int argc, char **argv){
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

#if 0    
    // socket sucess!
    Server_Socket_link Server_Socket(atoi(argv[1]), false);
    while(1){
        int client_sock;
        client_sock = Server_Socket.Accept();
        Client_Socket_link Client_Sock(client_sock, false);
        Client_Sock.Read();
        Client_Sock.Http_analyse();
        Client_Sock.Write();
    }
#endif

#if 0
    // test events add_events and destory_events success
    int epoll_fd = epoll_create(256);
    struct epoll_event activeEvs[100];
    Event *server_event = new Server_event(atoi(argv[1]), false, 0);
    Events::Add_event(server_event, epoll_fd);

    while(1){
        int n = 0, i, ret;
        n = epoll_wait(epoll_fd, activeEvs, 100, -1);
        for (i = 0; i < n; i++){
            Event *event = static_cast<Event *>(activeEvs[i].data.ptr);
            ret = event->Do_socket_process();
            if (ret){
                Event *client_event = new Client_event(ret, false, 0);
                Events::Add_event(client_event, epoll_fd);
            } else {
                Events::Destroy_event(event, epoll_fd);
                event->~Event(); 
            }
        }
    }

#endif 

//#if 0
    // events test success! 
    Events events(atoi(argv[1]));
    events.Process_events();

//#endif 
    return 0;
}
