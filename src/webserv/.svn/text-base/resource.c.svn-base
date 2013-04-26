/*

  RESOURCE.C
  ==========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  Implementation of functions for returning a resource.

*/


#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>

#include "resource.h"
#include "reqhead.h"
#include "helper.h"


/*  Change this string to change the root directory that
    the server will use, i.e. /index.html will translate
    here to /home/httpd/html/index.html                   */

static char server_root[1000] = "/Users/me/";


/*  Returns a resource  */

int Return_Resource(int conn, int resource, struct ReqInfo * reqinfo) {

    char c;
    int  i;

    while ( (i = read(resource, &c, 1)) ) {
	if ( i < 0 )
	    Error_Quit("Error reading from file.");
	if ( write(conn, &c, 1) < 1 )
	    Error_Quit("Error sending file.");
    }

    return 0;
}


/*  Tries to open a resource. The calling function can use
    the return value to check for success, and then examine
    errno to determine the cause of failure if neceesary.    */

int Check_Resource(struct ReqInfo * reqinfo) {

    /*  Resource name can contain urlencoded
	data, so clean it up just in case.    */

    CleanURL(reqinfo->resource);


    /*  Concatenate resource name to server root, and try to open  */

    strcat(server_root, reqinfo->resource);
    return open(server_root, O_RDONLY);
}


/*  Returns an error message  */

int Return_Error_Msg(int conn, struct ReqInfo * reqinfo) {

    char buffer[100];

    sprintf(buffer, "<HTML>\n<HEAD>\n<TITLE>Server Error %d</TITLE>\n"
	            "</HEAD>\n\n", reqinfo->status);
    Writeline(conn, buffer, strlen(buffer));

    sprintf(buffer, "<BODY>\n<H1>Server Error %d</H1>\n", reqinfo->status);
    Writeline(conn, buffer, strlen(buffer));

    sprintf(buffer, "<P>The request could not be completed.</P>\n"
	            "</BODY>\n</HTML>\n");
    Writeline(conn, buffer, strlen(buffer));

    return 0;

}
