#include "socket.h"
#include "event.h"
#include "conf.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>

// the path of configure file
const char *ZLOG_CONF_PATH = (char *)"etc/zlog.conf";

int main(int argc, char **argv){
	// init the zlog 
	int rc = 0;
	rc = dzlog_init(ZLOG_CONF_PATH, "myserver");	
	if (rc){
		printf("init zlog failed\n");
		return -1;
	}
	// check one instance	
	dzlog_info("one instance check...!");
	if (single_instance() == -1)
		return -1;
	// read the conf 
	Conf myserver(argc, argv);
	printf("the port is %d\n", myserver.Get_port());
	printf("the is_block is %d\n", myserver.Is_block());
	printf("the is_epoll_ET is %d\n", myserver.Is_epoll_ET());
	printf("the is_ssl is %d\n", myserver.Is_ssl());
	printf("threadpool_conf is %d, %d, %d\n",
		   myserver.Get_threadpool_conf().min_thread_num,
		   myserver.Get_threadpool_conf().max_thread_num,
		   myserver.Get_threadpool_conf().max_task_queue_size);
	return 0;
	int port = 8888;
	dzlog_info("myserver run on port:%d", port);
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
    Events events(port);
    events.Process_events();

//#endif 
    return 0;
}


