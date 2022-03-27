#ifndef COMMON_H_
#define COMMON_H_
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <sys/file.h>
void error_die(const char* message);
void error_handle(const char* message);
void warn(const char* message);
int single_instance(void);
#endif 
