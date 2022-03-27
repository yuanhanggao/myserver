#include "socket.h"
#include "event.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>

const char *ZLOG_CONF_PATH = (char *)"etc/zlog.conf";

static const struct option long_options[] = {
	{"port", required_argument, NULL, 'p'},
	{"help", no_argument,       NULL, 'h'},
	{0,      0,                 0,     0 }
};

void usage(void);
bool port_validator(char *optarg);
int main(int argc, char **argv){
	// process the commandline
	int opt, option_index = 0, port;
	char *opt_str = (char *)"p:h";
	while ((opt = getopt_long(argc, argv, opt_str,
				  long_options, &option_index)) != -1){
		switch (opt){
			case 'p':
				if (port_validator(optarg))
					port = atoi(optarg);	
				else{
					printf("invalid option -'%c %s!\r\n", opt, optarg);
					printf("Try './myserver -h' for more information.\r\n");
					return 0;
				}	
				break;
			case 'h':
				usage();
				return 0;
				break;
			default:
				printf("invaliod option -- '%c'\r\n", opt);
				printf("Try './myserver -h' for more information.\r\n");
				return 0;
				break;
		}
	}
	// init the zlog conf
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

void usage(void){
	printf("usage: myserver [arguments]\r\n");
	printf("[arguments]:\r\n");
	printf("\t-p or --port \tRun on the given port num, ");
	printf("and the value is between 1~65535\r\n");
	printf("\t-h or --help \tPrint Help(this message) and exit\r\n");
}

bool port_validator(char *optarg){
	if (!optarg)
		return false;
	int _port = atoi(optarg);
	if (_port < 1 or _port > 65535)
		return false;
	return true;
}
