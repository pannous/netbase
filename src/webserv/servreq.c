/*

  SERVREQ.C
  =========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  Implementation of function to service requests.

*/


#include <stdio.h>
#include <errno.h>

#include "helper.h"
#include "reqhead.h"
#include "resphead.h"
#include "resource.h"


/*  Service an HTTP request  */

int Service_Request(int conn) {

    struct ReqInfo  reqinfo;
    int             resource = 0;

    InitReqInfo(&reqinfo);


    /*  Get HTTP request  */

    if ( Get_Request(conn, &reqinfo) < 0 )
	return -1;


    /*  Check whether resource exists, whether we have permission
	to access it, and update status code accordingly.          */

    if ( reqinfo.status == 200 )
	if ( (resource = Check_Resource(&reqinfo)) < 0 ) {
	    if ( errno == EACCES )
		reqinfo.status = 401;
	    else
		reqinfo.status = 404;
	}

    /*  Output HTTP response headers if we have a full request  */

    if ( reqinfo.type == FULL )
	Output_HTTP_Headers(conn, &reqinfo);


    /*  Service the HTTP request  */

    if ( reqinfo.status == 200 ) {
		parse(reqinfo.resource);
	if ( Return_Resource(conn, resource, &reqinfo) )
	    Error_Quit("Something wrong returning resource.");
    }
    else
	Return_Error_Msg(conn, &reqinfo);

    if ( resource > 0 )
	if ( close(resource) < 0 )
	    Error_Quit("Error closing resource.");
    FreeReqInfo(&reqinfo);

    return 0;
}




