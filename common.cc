#include "common.h" 
void error_die(const char* message){
    perror(message);
    exit(1);
}
void error_handle(const char* message){
    perror(message);
}
void warn(const char* message){
    printf("%s\n", message);
}