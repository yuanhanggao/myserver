#include "common.h" 

const char *SERVER_LOCK_FILE_PATH = (char *)"./var/myserver.lock";

#define FLAG_LOCK (O_WRONLY | O_CREAT | O_TRUNC)
#define MODE_LOCK (S_IRWXU | S_IXGRP | S_IROTH | S_IXOTH)

int single_instance(void){
	int lock_fd = open(SERVER_LOCK_FILE_PATH, FLAG_LOCK, MODE_LOCK  );
	if (lock_fd < 0) {
		dzlog_error("open lock file %s failed", SERVER_LOCK_FILE_PATH);
		return -1;
	}

	if (-1 == flock(lock_fd, LOCK_EX | LOCK_NB)) {
		dzlog_info("another instance is running, quit!");
		return -1;
	}

	return 0;
}

