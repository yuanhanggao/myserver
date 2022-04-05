#ifndef CONF_H
#define CONF_H
#include "common.h"

typedef struct threadpool_conf_s{
	int min_thread_num = 10;  
	int max_thread_num = 100;
	int max_task_queue_size = 10;
}Threadpool_conf;

static const struct option long_options[] = {
	{"port",  required_argument, NULL, 'p'},
	{"block", no_argument,       NULL, 'b'},
	{"edge",  no_argument,       NULL, 'e'},
	{"conf",  required_argument, NULL, 'c'},
	{"ssl",   no_argument,       NULL, 's'},
	{"help",  no_argument,       NULL, 'h'},
	{0,       0,                 0,     0 }
};

class Conf{
private:	
	// configure setted bit flag 
	enum {
		PORT_SETTED = 1 << 0,
		BLOCK_SETTED = 1 << 1,
		EPOLL_SETTED = 1 << 2,
		CONF_SETTED = 1 << 3,
		SSL_SETTED = 1 << 4,
	};

	int conf_flag = 0;
	// command line parse 
	const char *opt_str = (char *)"p:becs:h";
	// network
	int port = 8888;
	bool is_block = false;
	bool is_epoll_ET = false;
	bool is_ssl = false;
	// threadpool 
	Threadpool_conf threadpool_conf;
	// validators
	bool Port_validator(const char *port);
	bool Threadpool_validator(int _min_thread_num,
							  int _max_thread_num,
							  int _max_task_queue_size);
	// parse the configure file
	void Conf_file_parse(const char *conf_file);
	// check whether it's setted	
	void Set_flag(int set_flag);
	bool Is_flag_setted(int set_flag);
public:
	Conf(int _argc, char **_argv);
	~Conf();
	int Get_port();
	bool Is_block();
	bool Is_epoll_ET();
	bool Is_ssl();
	Threadpool_conf &Get_threadpool_conf();
	void Usage();
};

#endif 
