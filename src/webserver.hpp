#pragma once


#include <string>
/*

  HELPER.H
  ========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  Interface to helper functions for simple webserver.

*/
void start_server();

#ifndef PG_HELPER_H
#define PG_HELPER_H


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

#define LISTENQ          (1024)


#endif  /*  PG_HELPER_H  */
/*

  REQHEAD.H
  =========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  Interface to functions for manipulating HTTP request headers.

*/


#ifndef PG_REQHEAD_H
#define PG_REQHEAD_H


/*  User-defined data types  */

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


/*  Global macros/variables  */

#define MAX_REQ_LINE         (1024)


/*  Function prototypes  */

int  Parse_HTTP_Header(char * buffer, struct ReqInfo * reqinfo);
int  Get_Request      (int conn, struct ReqInfo * reqinfo);
void InitReqInfo      (struct ReqInfo * reqinfo);
void FreeReqInfo      (struct ReqInfo * reqinfo);

#endif  /*  PG_REQHEAD_H  */
/*

  RESOURCE.H
  ==========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  Interface to functions for returning a resource.

*/


#ifndef PG_RESOURCE_H
#define PG_RESOURCE_H


//#include "reqhead.h"         /*  for struct ReqInfo  */


/*  Function prototypes  */

int Return_Resource (int conn, int resource, struct ReqInfo * reqinfo);
int Check_Resource  (struct ReqInfo * reqinfo);
int Return_Error_Msg(int conn, struct ReqInfo * reqinfo);


#endif  /*  PG_RESOURCE_H  */
/*

  RESPHEAD.H
  ==========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  Interface to HTTP response header functions

*/


#ifndef PG_RESPHEAD_H
#define PG_RESPHEAD_H


//#include "reqhead.h"          /*  for struct ReqInfo  */


/*  Function prototypes  */

int Output_HTTP_Headers(int conn, struct ReqInfo * reqinfo);


#endif  /*  PG_RESPHEAD_H  */
/*

  SERVREQ.H
  =========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  Interface to function to server connections.

*/


#ifndef PG_SERVREQ_H
#define PG_SERVREQ_H


/*  Function prototypes  */

int Service_Request(int conn);
int handle(char* q,int conn=-1);

#endif  /*  PG_SERVREQ_H  */










