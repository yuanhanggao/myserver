#ifndef HTTP_H
#define HTTP_H
#include "common.h"
class Http{
private:
    enum http_state {INIT, REQUEST_LINE, METHOD, URL, HEADER, BODY, FINISH};
    enum {BUF_LENGTH = 1024, METHOD_MAX_LENGTH = 10, 
          URL_MAX_LENGTH = 128, REQUEST_LINE_MAX_LENGTH = 256, 
          HEADER_MAX_LENGTH = 512, BODY_MAX_LENGTH = 512};
    static char *SERVER_STRING;
    enum http_method {GET, POST, DELETE, PUT};
    http_method method;
    enum http_response_code {SUCCESS = 200, NOTFOUND =404, UNIMPLEMENTED = 501};
    http_response_code response_code;
    char request[BUF_LENGTH]; 
    char response[BUF_LENGTH];
    char url[URL_MAX_LENGTH];
    char request_line[REQUEST_LINE_MAX_LENGTH];
//    to do
    char header[HEADER_MAX_LENGTH];  
    char body[BODY_MAX_LENGTH];
    int request_line_pos;
    int pos;
    bool get_method();
    bool get_url();
    bool get_request_line();
    bool get_header_line();
    bool get_body_line();
    void fill_unimplemented_response();
    void fill_success_response();
    void fill_notfound_response();
    
public:
    bool analysed_failed;
    http_state state;
    Http(const char *message, int length);
    ~Http();
    bool Analyse_request();
    char *Build_response(int &length);
};
#endif