#include <string.h>
#include "http.h"

char* Http::SERVER_STRING = (char *)"Server: jdbhttpd/0.1.0\r\n";
Http::Http(const char *message, int length){
    analysed_failed = false;
    state = INIT;
    if (length >= BUF_LENGTH){
        printf("http init failed!"
               " the length of input resquest is %d\n", length);
        response_code = UNIMPLEMENTED;
        analysed_failed = true;
        return;
    }

    memset(request, 0, BUF_LENGTH);
    memset(response, 0, BUF_LENGTH);
    memset(url, 0, URL_MAX_LENGTH);
    memset(request_line, 0, REQUEST_LINE_MAX_LENGTH);
    memset(header, 0, HEADER_MAX_LENGTH);
    memset(body, 0, BODY_MAX_LENGTH);

    int rc = snprintf(request, length + 1, "%s", message);
    if (rc <= 0){
        error_handle("http init failed! snprintf failed!");
        response_code = UNIMPLEMENTED;
        analysed_failed = true;
        return;
    }
}

Http::~Http(){

}


bool Http::Analyse_request(){
    pos = 0;
    if (analysed_failed){
        warn("Due to the failure of http's init, stop analysing");
    }
    if (get_request_line()&&get_header_line()&&get_body_line()){
        state = FINISH;
    	response_code = SUCCESS;
        printf("Http analyse success!\n");
        return true;
    } 
    printf("Http analyse failed!\n");
    response_code = UNIMPLEMENTED;
    analysed_failed = true;
    return false;
}

bool Http::get_method(){
    state = METHOD;
    int i = 0;
    int request_line_len = strlen(request_line);
    char tmp_method[METHOD_MAX_LENGTH] = {0};
    while((request_line[i] != ' ') ){
        if (i >= request_line_len){
            printf("Analyse method failed, "
                   "due to out of request_line's boundary!\n");
            return false;    
        }
         
        tmp_method[i] = request_line[i];
        i++;
    }
    tmp_method[i] = '\0';
    request_line_pos = i;
    if (0 == strncmp(tmp_method, "GET", strlen("GET")))
        method = GET;
    else if (0 == strncmp(tmp_method, "POST", strlen("POST")))
        method = POST;
    else if (0 == strncmp(tmp_method, "DELETE", strlen("DELETE")))
        method = DELETE;
    else if (0 == strncmp(tmp_method, "PUT", strlen("PUT")))
        method = PUT;
    else{
        printf("unkown http request method\n");
        return false;
    }
    printf("the method is %d\n", method);
    return true;
}

bool Http::get_url(){
    state = URL;
    int i = 0;
    int request_line_len = strlen(request_line);
    while (request_line[request_line_pos] == ' '){
        if (request_line_pos >= request_line_len){
            printf("Analyse url failed, "
                   "due to out of request_line's boundary!\n");
            return false;
        }
        request_line_pos++;
    }

    while (request_line[request_line_pos] != ' '){
        if (request_line_pos >= request_line_len){
            printf("Analyse url failed, "
                   "due to out of request_line's boundary!\n");
            return false;
        }
        if (i >= URL_MAX_LENGTH){
            printf("Analyse url failed, "
                   "due to out of url_max_line's boundary!\n");
            return false;
        }
        url[i] = request_line[request_line_pos];
        request_line_pos++;
        i++;
    }
}

bool Http::get_request_line(){
    request_line_pos = 0;
    state = REQUEST_LINE;
    int i = 0;
    do{
        if (i >= REQUEST_LINE_MAX_LENGTH){
            printf("failed when analyse request line, "
                    "the request line is too long!\n");
            return false;
        }

        if (pos >= BUF_LENGTH){
            printf("failed when analyse request line, "
                    "out of request buf's boundary!\n");
            return false;
        }

        if (request[pos] == '\n' ){
            if (get_method() && get_url()){
                pos++;   // pass the \n
                break;
            }
            return false;
        }

        if (request[pos] != '\r')
            request_line[i] = request[pos];
        i++, pos++;
    }while(1);
    return true;
}

bool Http::get_header_line(){
    state = HEADER;
    int i = 0;
    char tmp_header[HEADER_MAX_LENGTH] = {0};
    while (1){
        if (pos >= BUF_LENGTH){
            printf("failed when analyse header line, "
                    "out of request buf's boundary!\n");
            return false;
        }
        if (i >= HEADER_MAX_LENGTH){
            printf("failed when analyse header line, "
                    "out of header line's boundary!\n");
            return false;
        }
        tmp_header[i] = request[pos];
        if (request[pos] == '\n'){
            if (i > 2){
                strncat(header, tmp_header, i+1);
                i = 0;
                memset(tmp_header, 0, HEADER_MAX_LENGTH); 
                continue;
            }
            else 
                break;
        }
        i++;
        pos++;
    }
    return true;
}

bool Http::get_body_line(){
    state = BODY;
    int i = 0;
    while(1) {
        if (pos >= BUF_LENGTH){
            printf("failed when analyse header line, "
                    "out of request buf's boundary!\n");
            return false;
        }
        if (i >= BODY_MAX_LENGTH){
            printf("failed when analyse body line, "
                    "out of header line's boundary!\n");
            return false;
        }
        if (request[pos] == '\n')
            break;
        else if (request[pos] == '\r')
            body[i] = '\0';
        else 
            body[i] = request[pos];
        pos++;
        i++;
    }
    return true;
}

char *Http::Build_response(int &length){
    if (state == INIT){
        return NULL;
    }
    switch (response_code)
    {
    case SUCCESS:
        fill_success_response(); 
        break;
    case NOTFOUND:
        fill_notfound_response(); 
        break;
    default:
        fill_unimplemented_response();
        break;
    }
    length = strlen(response);
    return response;
}

void Http::fill_success_response(){
    if (strlen(response) != 0)
        memset(response, 0, BUF_LENGTH);
    strcat(response, "HTTP/1.0 200 Method Not Implemented\r\n");
    strcat(response, SERVER_STRING);
    strcat(response, "Content-Type: text/html\r\n");
    strcat(response, "\r\n");
    strcat(response, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    strcat(response, "</TITLE></HEAD>\r\n");
    strcat(response, "<BODY><P>HTTP request success.\r\n");
    strcat(response, "</BODY></HTML>\r\n");
}

void Http::fill_notfound_response(){
    if (strlen(response) != 0)
        memset(response, 0, BUF_LENGTH);
    strcat(response, "HTTP/1.0 404 NOT FOUND\r\n");
    strcat(response, SERVER_STRING);
    strcat(response, "Content-Type: text/html\r\n");
    strcat(response, "\r\n");
    strcat(response, "<HTML><TITLE>Not Found</TITLE>\r\n");
    strcat(response, "<BODY><P>The server could not fulfill\r\n");
    strcat(response, "your request because the resource specified\r\n");
    strcat(response, "is unavailable or nonexistent.\r\n");
    strcat(response, "</BODY></HTML>\r\n");
}

void Http::fill_unimplemented_response(){
    if (strlen(response) != 0)
        memset(response, 0, BUF_LENGTH);
    strcat(response, "HTTP/1.0 501 Method Not Implemented\r\n");
    strcat(response, SERVER_STRING);
    strcat(response, "Content-Type: text/html\r\n");
    strcat(response, "\r\n");
    strcat(response, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    strcat(response, "</TITLE></HEAD>\r\n");
    strcat(response, "<BODY><P>HTTP request method not supported.\r\n");
    strcat(response, "</BODY></HTML>\r\n");
}
