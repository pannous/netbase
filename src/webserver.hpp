extern int SERVER_PORT;
#pragma once


#include <string>
#include <vector>

void start_server(int port);

#include <unistd.h>          /*  for ssize_t data type  */

/*  Function prototypes  */

void    Error_Quit(char const * msg);
int     Trim      (char * buffer);
int     StrUpper  (char * buffer);
void    CleanURL  (char * buffer);
ssize_t Readline  (int sockd, void *vptr, size_t maxlen);
//ssize_t Writeline (int sockd, const void *vptr, size_t n=-1);
void Writeline(const char* s);// debug to server
void Writeline(std::string s);
ssize_t Writeline(int sockd, std::string s);
ssize_t Writeline (int sockd, const char *vptr, size_t n=-1);


/*  Global macros/variables  */

#define LISTENQ          (4096)
#define MAX_REQ_LINE         (4096)
enum Req_Method { GET, HEAD, UNSUPPORTED };
enum Req_Type   { SIMPLE, FULL };

struct ReqInfo {
    enum Req_Method method;
    enum Req_Type   type;
    char           *referer;
    char           *useragent;
    char           *resource;
    int             status;
};


int  Parse_HTTP_Header(char * buffer, struct ReqInfo * reqinfo);
int  Get_Request      (int conn, struct ReqInfo * reqinfo);
void InitReqInfo      (struct ReqInfo * reqinfo);
void FreeReqInfo      (struct ReqInfo * reqinfo);
int Return_Resource (int conn, int resource, struct ReqInfo * reqinfo);
int Check_Resource  (struct ReqInfo * reqinfo);
int Return_Error_Msg(int conn, struct ReqInfo * reqinfo);
int Output_HTTP_Headers(int conn, struct ReqInfo * reqinfo);
int Service_Request(int conn);
int handle(const char* q,int conn=-1);
void Serve_Resource(ReqInfo reqinfo, int conn);// local file
