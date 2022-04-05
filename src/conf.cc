#include "conf.h"
#include "INIReader.h"

// the default path of myserver configure file path
const char *MYSERVER_CONF_PATH = (char *)"etc/myserver.conf";

Conf::Conf(int _argc, char **_argv) {
	int opt, option_index = 0;
	while ((opt = getopt_long(_argc, _argv, opt_str,
				  long_options, &option_index)) != -1){
		switch (opt){
			case 'p':
				if (Port_validator(optarg)) {
					port = atoi(optarg);	
					Set_flag(PORT_SETTED);
				}
				else{
					printf("invalid option -'%c %s!\r\n", opt, optarg);
					printf("Try './myserver -h' for more information.\r\n");
					exit(-1);
				}	
				break;
			case 'b':
				is_block = true;	
				Set_flag(BLOCK_SETTED);
				break;
			case 'e':
				is_epoll_ET = true;
				Set_flag(EPOLL_SETTED);
				break;
			case 'c':
				Conf_file_parse(optarg);
				Set_flag(CONF_SETTED);
				break;
			case 's':
				is_ssl = true;
				Set_flag(SSL_SETTED);
				break;
			case 'h':
				Usage();
				exit(-1);
				break;
			default:
				printf("invaliod option -- '%c'\r\n", opt);
				printf("Try './myserver -h' for more information.\r\n");
				exit(-1);
				break;
		}
	}
	// parse the default configure file
	if (!Is_flag_setted(CONF_SETTED))
		Conf_file_parse(MYSERVER_CONF_PATH);
}

void Conf::Conf_file_parse(const char *path) {
	if (path == NULL){
		dzlog_warn("conf path is NULL");
		exit(-1);
	}	
	INIReader reader(path);
	dzlog_info("configure!");

	if (reader.ParseError() < 0) {
		dzlog_error("load conf file failed!");
		printf("load conf file failed!\n");
		exit(-1);
	}
	// parse port
	if (!Is_flag_setted(PORT_SETTED)) {
		const char *_port = reader.Get("network", "port", "123").data();
		if (Port_validator(_port)){
			port = atoi(_port);
		}	
		else 
			dzlog_warn("the port: %s is invalid! "
				       "and use default port = 8888!", _port);
	}
	// parse is_block
	if (!Is_flag_setted(BLOCK_SETTED)) 
		is_block = reader.GetBoolean("network", "is_block", false);
	// parse epoll_mode
	if (!Is_flag_setted(EPOLL_SETTED)) 
		is_epoll_ET = reader.GetBoolean("network", "is_edge_tiggered", false);
	// pasre ssl mode
	if (!Is_flag_setted(SSL_SETTED))
		is_ssl = reader.GetBoolean("network", "is_ssl", false);
	// parse threadpoll config
	int _min_thread_num, _max_thread_num, _max_task_queue_size;
	_min_thread_num = reader.GetInteger("threadpool", "min_thread_num", 10);
	_max_thread_num = reader.GetInteger("threadpool", "max_thread_num", 100);
	_max_task_queue_size = reader.GetInteger("threadpool", "max_task_queue_size", 10);
	// check the params of port
	if (Threadpool_validator(_min_thread_num, _max_thread_num, _max_task_queue_size)) {
		threadpool_conf.min_thread_num = _min_thread_num;
		threadpool_conf.max_thread_num = _max_thread_num;
		threadpool_conf.max_task_queue_size = _max_task_queue_size;
	} else 
		dzlog_warn("invalid threadpool conf! use default params!");
}

Conf::~Conf() {
}

bool Conf::Port_validator(const char *optarg) {
	if (!optarg)
		return false;
	int _port = atoi(optarg);
	if (_port < 1 or _port > 65535)
		return false;
	return true;
}

bool Conf::Threadpool_validator(int _min_thread_num,
								int _max_thread_num,
								int _max_task_queue_size){
		if (_min_thread_num > _max_thread_num) {
			dzlog_warn("min_thread_num is bigger than man_thread_num");
			return false;
		}
		else 
			return true;
}

void Conf::Usage(void) {
	printf("usage: myserver [arguments]\r\n");
	printf("[arguments]:\r\n");
	// -p or --port
	printf("\t-p or --port \tRun on the given port num, ");
	printf("and the value is between 1~65535\r\n");
	// -e or --edge
	printf("\t-e or --edge \tUse edge trigger mode in epoll\r\n");
	// -b or --block
	printf("\t-b or --block \tUse block mode in io\r\n");
	// -c or --conf
	printf("\t-c or --conf \tUse another conf file, ");
	printf("and the value must be an invalid path\r\n");
	// -s or --ssl
	printf("\t-s or --ssl \tHttps instead of http\r\n");
	// -h or --help
	printf("\t-h or --help \tPrint Help(this message) and exit\r\n");
}

void Conf::Set_flag(int set_flag) {
	conf_flag |= set_flag;	
}

bool Conf::Is_flag_setted(int set_flag) {
	return conf_flag&set_flag;
}

int Conf::Get_port() {
	return port;
}

bool Conf::Is_block() {
	return is_block;
}

bool Conf::Is_epoll_ET() {
	return is_epoll_ET;
}

bool Conf::Is_ssl() {
	return is_ssl;
}

Threadpool_conf &Conf::Get_threadpool_conf() {
	return threadpool_conf;
}
