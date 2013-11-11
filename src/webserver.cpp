#pragma once
/*

  HELPER.C
  ========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  Implementation of helper functions for simple web server.
  The Readline() and Writeline() functions are shamelessly
  ripped from "UNIX Network Programming" by W Richard Stevens.

 */


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "webserver.hpp"
#include "console.hpp" // parse request
#include "util.hpp"
#include "query.hpp"
//#include "helper.h"

/*  Service an HTTP request  */

#define SERVER_PORT  (3333)

enum result_format {
	xml, json, txt,csv,html
};

enum result_verbosity {
	shorter, normal, longer,verbose
};


void fixLabel(Node* n){
	if(n->name[0]=='"')n->name=n->name+1;
	if(n->name[strlen(n->name)-1]=='"'&&n->name[strlen(n->name)-2]!='"')
		n->name[strlen(n->name)-1]=0;
}

void fixLabels(Statement* s){
	fixLabel(s->Subject());
	fixLabel(s->Predicate());
	fixLabel(s->Object());
}

// WORKS FINE, but not when debugging
int Service_Request(int conn) {

	struct ReqInfo reqinfo;
	InitReqInfo(&reqinfo);
	enum result_format format = txt;
	enum result_verbosity verbosity = normal;

	/*  Get HTTP request  */
	if (Get_Request(conn, &reqinfo) < 0)
		return -1;

	if (reqinfo.type == FULL)
		Output_HTTP_Headers(conn, &reqinfo);

	// file system:
	//		Serve_Resource(ReqInfo  reqinfo,int conn)

	CleanURL(reqinfo.resource);

	init(); // for each forked process!
	char* q = substr(reqinfo.resource, 1, -1);
	int len=strlen(q);
	if (eq(q, "favicon.ico"))return 0;
	if (endsWith(q, ".json")) {
			format = json;
			q[len-5]=0;
		}

	if (endsWith(q, ".xml")) {
			format = xml;
			q[len-4]=0;
		}

	if (endsWith(q, ".csv")) {
			format = csv;
			q[len-4]=0;
		}

	if (endsWith(q, ".txt")) {
			format = txt;
			q[len-4]=0;
		}

	if (endsWith(q, ".html")) {
		format = html;
		q[len-5]=0;
	}
	if (startsWith(q, ".js")) {
		q[len-3]=0;
		Writeline(conn, "var results=");
		format = json;
	}
	if (startsWith(q, "html/")) {
			format = html;
			verbosity=verbose;
			q = q + 5;
		}
	if (startsWith(q, "text/")) {
		format = txt;
		q = q + 5;
	}
	if (startsWith(q, "txt/")) {
		format = txt;
		q = q + 4;
	}
	if (startsWith(q, "xml/")) {
		format = xml;
		q = q + 4;
	}
	if (startsWith(q, "csv/")) {
		format = csv;
		q = q + 4;
	}
	if (startsWith(q, "js/")) {
		q = q + 3;
		if(format!=json)
		Writeline(conn, "var results=");
		format = json;
	}
	else if (startsWith(q, "json/")) {
		format = json;
		q = q + 5;
	}
	if (startsWith(q, "short/")) {
		verbosity = shorter;
		q = q + 6;
	}
	if (startsWith(q, "long/")) {
		verbosity = longer;
		q = q + 5;
	}
	if (startsWith(q, "verbose/")) {
		verbosity = verbose;
		q = q + 8;
	}
//	if (startsWith(q, "m/")) {
//			q = q + 2;
//
//		}

    
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!
	//
    NodeVector all = parse(q); // <<<<<<<< NETaddSBASE!
    //
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!
	
    const char* html_block="<html><body><div id='results'></div><script>var results={'results':[\n";

    if((int)all.size()==0)Writeline("0");
	//	Writeline(conn,q);
	char buff[10000];
	if (format == xml && (startsWith(q,"select")||contains(q," where "))){Writeline(conn,query2(q));return 0;}
	if (format == xml)Writeline(conn, "<results>\n");
	if (format == json)Writeline(conn, "{'results':[\n");
	if (format == html)Writeline(conn,html_block);
	const char* statement_format_xml = "   <statement id='%d' subject=\"%s\" predicate=\"%s\" object=\"%s\" sid='%d' pid='%d' oid='%d'/>\n";
	const char* statement_format_text = "   $%d %s %s %s %d->%d->%d\n";
	const char* statement_format_json = "      { 'id':%d, 'subject':\"%s\", 'predicate':\"%s\", 'object':\"%s\", 'sid':%d, 'pid':%d, 'oid':%d},\n";
	const char* statement_format_csv = "%d\t%s\t%s\t%s\t%d\t%d\t%d\n";
	const char* statement_format;
	if (format == xml)statement_format = statement_format_xml;
	if (format == html)statement_format = statement_format_json;
	if (format == json)statement_format = statement_format_json;
	if (format == txt)statement_format = statement_format_text;
	if (format == csv)statement_format = statement_format_csv;
    
   	const char* entity_format;
	const char* entity_format_txt = "%s (%d)\n";
	const char* entity_format_xml = "<entity name=\"%s\" id='%d'>\n";
	const char* entity_format_json = "   {'name':\"%s\", 'id':%d";
   	const char* entity_format_csv = "%s\t%d\n";
    if(all.size()==1)entity_format_csv = "";//statements!
	if (format == xml)entity_format = entity_format_xml;
	if (format == html)entity_format = entity_format_json;
	if (format == json)entity_format = entity_format_json;
	if (format == txt)entity_format = entity_format_txt;
	if (format == csv)entity_format = entity_format_csv;
	for (int i = 0; i < all.size(); i++) {
		Node* node = (Node*) all[i];
		sprintf(buff, entity_format, node->name, node->id);
		Writeline(conn, buff);
		Statement* s = 0;
		if (format==csv|| verbosity == verbose || verbosity == longer || ( all.size() == 1 && !verbosity == shorter)) {
			if (format == json||format == html)Writeline(conn, ",'statements':[\n");
			int count=0;
			while ((s = nextStatement(node, s))&&count++<resultLimit) {
				fixLabels(s);
                if(format==csv&&all.size()>1)break;
				if (!checkStatement(s))continue;
				if(verbosity!=verbose && (s->Predicate()==Instance||s->Predicate()==Type))continue;
				sprintf(buff, statement_format, s->id(), s->Subject()->name, s->Predicate()->name, s->Object()->name, s->Subject()->id, s->Predicate()->id, s->Object()->id);
				Writeline(conn, buff);
			}
			if (format == json||format == html)Writeline(conn, "]");
		}
		if (format == json||format == html)Writeline(conn, "},\n");
		if (format == xml)Writeline(conn, "</entity>\n");
		//		string img=getImage(node->name);
		//		if(img!="")Writeline(conn,"<img src='"+img+"'/>");
	}
	const char* html_end="]};</script><script src='http://pannous.net/netbase.js'></script></body></html>";
	if (format == json)Writeline(conn, "]}\n");
	if (format == html)Writeline(conn, html_end);
	if (format == xml)Writeline(conn, "</results>\n");

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!
	FreeReqInfo(&reqinfo);
	return 0;
}

/*  Prints an error message and quits  */

void Error_Quit(char const * msg) {
	fprintf(stderr, "WEBSERV: %s\n", msg);
	exit(EXIT_FAILURE);
}

/*  Read a line from a socket  */

ssize_t Readline(int sockd, void *vptr, size_t maxlen) {
	ssize_t n, rc;
	char c, *buffer;

	buffer = (char*) vptr;

	for (n = 1; n < maxlen; n++) {

		if ((rc = read(sockd, &c, 1)) == 1) {
			*buffer++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			if (n == 1)
				return 0;
			else
				break;
		} else {
			if (errno == EINTR)
				continue;
			Error_Quit("Error in Readline()");
		}
	}

	*buffer = 0;
	return n;
}

void Writeline(const char* s) {
	Writeline(0, s, 0);
}

/*  Write a line to a socket  */
ssize_t Writeline(int sockd, string s) {
	return Writeline(sockd, s.data(), s.length());
}
int lastSockd = 0;

ssize_t Writeline(int sockd, const char *vptr, size_t n) {
	size_t nleft;
	ssize_t nwritten;
	const char *buffer;

	if (sockd == 0)sockd = lastSockd; //not thread safe!
	lastSockd = sockd;

	buffer = vptr;
	if (n == 0 || n == -1)
		n = strlen(buffer);
	//	printf("%d:%s\n",n,buffer);
	nleft = n;

	while (nleft > 0) {
		if ((nwritten = write(sockd, buffer, nleft)) <= 0) {
			if (errno == EINTR)
				nwritten = 0;
			else
				Error_Quit("Error in Writeline()");
		}
		nleft -= nwritten;
		buffer += nwritten;
	}

	return n;
}


/*  Removes trailing whitespace from a string  */
#include "string.h"

int Trim(char * buffer) {
	long n = strlen(buffer) - 1;

	while (!isalnum(buffer[n]) && n >= 0)
		buffer[n--] = '\0';

	return 0;
}

/*  Converts a string to upper-case  */

int StrUpper(char * buffer) {
	while (*buffer) {
		*buffer = toupper(*buffer);
		++buffer;
	}
	return 0;
}

/*  Cleans up url-encoded string  */

void CleanURL(char * buffer) {
	char asciinum[3] = {0};
	int i = 0, c;

	while (buffer[i]) {
		if (buffer[i] == '+')
			buffer[i] = ' ';
		else if (buffer[i] == '%') {
			asciinum[0] = buffer[i + 1];
			asciinum[1] = buffer[i + 2];
			buffer[i] = strtol(asciinum, NULL, 16);
			c = i + 1;
			do {
				buffer[c] = buffer[c + 2];
			} while (buffer[2 + (c++)]);
		}
		++i;
	}
}
/*

  REQHEAD.C
  =========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  Implementation of functions to manipulate HTTP request headers.

 */


#include <sys/time.h>             /*  For select()  */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include "reqhead.h"
//#include "servreq.h"
//#include "helper.h"

/*  Parses a string and updates a request
	information structure if necessary.    */

int Parse_HTTP_Header(char * buffer, struct ReqInfo * reqinfo) {

	static int first_header = 1;
	char *temp;
	char *endptr;
	size_t len;


	if (first_header == 1) {

		/*  If first_header is 0, this is the first line of
			the HTTP request, so this should be the request line.  */


		/*  Get the request method, which is case-sensitive. This
			version of the server only supports the GET and HEAD
			request methods.                                        */

		if (!strncmp(buffer, "GET ", 4)) {
			reqinfo->method = GET;
			buffer += 4;
		} else if (!strncmp(buffer, "HEAD ", 5)) {
			reqinfo->method = HEAD;
			buffer += 5;
		} else {
			reqinfo->method = UNSUPPORTED;
//			reqinfo->status = 501;
//			return -1;
		}

		/*  Skip to start of resource  */
		while (*buffer && isspace(*buffer))
			buffer++;


		/*  Calculate string length of resource...  */
		endptr = strchr(buffer, ' ');
		if (endptr == NULL)
			len = strlen(buffer);
		else
			len = endptr - buffer;
		if (len == 0) {
//			reqinfo->status = 400;
//			return -1;
		}

		/*  ...and store it in the request information structure.  */

		reqinfo->resource = (char*) calloc(len + 1, sizeof (char));
		strncpy(reqinfo->resource, buffer, len);


		/*  Test to see if we have any HTTP version information.
			If there isn't, this is a simple HTTP request, and we
			should not try to read any more headers. For simplicity,
			we don't bother checking the validity of the HTTP version
			information supplied - we just assume that if it is
			supplied, then it's a full request.                        */

		if (contains(buffer, "HTTP/")||contains(buffer, "http/"))
			reqinfo->type = FULL;
		else
			reqinfo->type = SIMPLE;

		first_header = 0;
		return 0;
	}


	/*  If we get here, we have further headers aside from the
	request line to parse, so this is a "full" HTTP request.  */

	/*  HTTP field names are case-insensitive, so make an
	upper-case copy of the field name to aid comparison.
	We need to make a copy of the header up until the colon.
	If there is no colon, we return a status code of 400
	(bad request) and terminate the connection. Note that
	HTTP/1.0 allows (but discourages) headers to span multiple
	lines if the following lines start with a space or a
	tab. For simplicity, we do not allow this here.              */

	endptr = strchr(buffer, ':');
	if (endptr == NULL) {
//		reqinfo->status = 400;
		return -1;//ok
	}
	temp = (char*) calloc((endptr - buffer) + 1, sizeof (char));
	strncpy(temp, buffer, (endptr - buffer));
	StrUpper(temp);


	/*  Increment buffer so that it now points to the value.
	If there is no value, just return.                    */

	buffer = endptr + 1;
	while (*buffer && isspace(*buffer))
		++buffer;
	if (*buffer == '\0')
		return 0;
    

	/*  Now update the request information structure with the
	appropriate field value. This version only supports the
	"Referer:" and "User-Agent:" headers, ignoring all others.  */

	if (!strcmp(temp, "USER-AGENT")) {
		reqinfo->useragent = (char*) malloc(strlen(buffer) + 1);
		strcpy(reqinfo->useragent, buffer);
	} else if (!strcmp(temp, "REFERER")) {
		reqinfo->referer = (char*) malloc(strlen(buffer) + 1);
		strcpy(reqinfo->referer, buffer);
	}

	free(temp);
	return 0;
}

/*  Gets request headers. A CRLF terminates a HTTP header line,
	but if one is never sent we would wait forever. Therefore,
	we use select() to set a maximum length of time we will
	wait for the next complete header. If we timeout before
	this is received, we terminate the connection.               */

int Get_Request(int conn, struct ReqInfo * reqinfo) {

	char buffer[MAX_REQ_LINE] = {0};
	int rval;
	fd_set fds;
	struct timeval tv;


	/*  Set timeout to 5 seconds  */

	tv.tv_sec = 5;
	tv.tv_usec = 0;


	/*  Loop through request headers. If we have a simple request,
	then we will loop only once. Otherwise, we will loop until
	we receive a blank line which signifies the end of the headers,
	or until select() times out, whichever is sooner.                */

	do {

		/*  Reset file descriptor set  */

		FD_ZERO(&fds);
		FD_SET(conn, &fds);


		/*  Wait until the timeout to see if input is ready  */

		rval = select(conn + 1, &fds, NULL, NULL, &tv);


		/*  Take appropriate action based on return from select()  */

		if (rval < 0) {
			Error_Quit("Error calling select() in get_request()");
		} else if (rval == 0) {

			p(" input not ready after timeout  ");
			return -1;

		} else {

			/*  We have an input line waiting, so retrieve it  */

			Readline(conn, buffer, MAX_REQ_LINE - 1);
			//	    Trim(buffer);

			if (buffer[0] == '\0')
				break;

			if (Parse_HTTP_Header(buffer, reqinfo))
				break;
		}
	} while (reqinfo->type != SIMPLE);

	return 0;
}

/*  Initialises a request information structure  */

void InitReqInfo(struct ReqInfo * reqinfo) {
	reqinfo->useragent = NULL;
	reqinfo->referer = NULL;
	reqinfo->resource = NULL;
	reqinfo->method = UNSUPPORTED;
	reqinfo->status = 200;
}

/*  Frees memory allocated for a request information structure  */

void FreeReqInfo(struct ReqInfo * reqinfo) {
	if (reqinfo->useragent)
		free(reqinfo->useragent);
	if (reqinfo->referer)
		free(reqinfo->referer);
	if (reqinfo->resource)
		free(reqinfo->resource);
}
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

//#include "resource.h"
//#include "reqhead.h"
//#include "helper.h"


/*  Change this string to change the root directory that
	the server will use, i.e. /index.html will translate
	here to /home/httpd/html/index.html                   */

static char server_root[1000] = "/Users/me/";

/*  Returns a resource  */

int Return_Resource(int conn, int resource, struct ReqInfo * reqinfo) {
	char c;
	size_t i;

	while ((i = read(resource, &c, 1))) {
//		if (i < 0)
//			Error_Quit("Error reading from file.");
		if (write(conn, &c, 1) < 1)
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
/*

  RESPHEAD.C
  ==========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  Implementation of HTTP reponse header functions.

 */

#include <unistd.h>
#include <stdio.h>

//#include "resphead.h"
//#include "helper.h"

/*  Outputs HTTP response headers  */

int Output_HTTP_Headers(int conn, struct ReqInfo * reqinfo) {
	char buffer[100];
	sprintf(buffer, "HTTP/1.0 %d OK\r\n", reqinfo->status);
	Writeline(conn, buffer, strlen(buffer));
	Writeline(conn, "Server: Netbase \r\n", 24);
	Writeline(conn, "Content-Type: text/html\r\n", 25);
	Writeline(conn, "\r\n", 2);
	return 0;
}



#include <stdio.h>
#include <errno.h>

void Serve_Resource(ReqInfo reqinfo, int conn) {
	int resource = 0;
	/*  Check whether resource exists, whether we have permission
	to access it, and update status code accordingly.          */

	if (reqinfo.status == 200)
		if ((resource = Check_Resource(&reqinfo)) < 0) {
			if (errno == EACCES)
				reqinfo.status = 401;
			else
				reqinfo.status = 404;
		}

	/*  Output HTTP response headers if we have a full request  */

	if (reqinfo.type == FULL)
		Output_HTTP_Headers(conn, &reqinfo);


	/*  Service the HTTP request  */

	//	if ( Return_Resource(conn, resource, &reqinfo) )
	//	    Error_Quit("Something wrong returning resource.");
	//    }
	//    else
	//	Return_Error_Msg(conn, &reqinfo);

	if (resource > 0)
		if (close(resource) < 0)
			Error_Quit("Error closing resource.");
	FreeReqInfo(&reqinfo);
}



/*

  WEBSERV.C
  =========
  (c) Copyright Paul Griffiths 1999
  Email: mail@paulgriffiths.net

  A simple web server

 */


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <sys/wait.h>         /*  for waitpid()             */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include <stdio.h>
#include <stdlib.h>

//#include "helper.h"
//#include "servreq.h"

/*  main() funcion  */

void start_server() {
	printf("STARTING SERVER!\n localhost:%d\n", SERVER_PORT);
	flush();
	int listener, conn;
	pid_t pid;
//    socklen_t
	struct sockaddr_in servaddr;


	/*  Create socket  */
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		Error_Quit("Couldn't create listening socket.");


	/*  Populate socket address structure  */
	memset(&servaddr, 0, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);


	/*  Assign socket address to socket  */
//	__bind<int&,sockaddr *,unsigned long> x=
    bind(listener, (struct sockaddr *) &servaddr, sizeof (servaddr));
    if(listener<0)
		Error_Quit("Couldn't bind listening socket.");


	/*  Make socket a listening socket  */
    
//    	if (listen(listener, BACKLOG) < 0)
	if (listen(listener, LISTENQ) < 0)
		Error_Quit("Call to listen failed.");


	printf("listening on %d port %d\n", INADDR_ANY, SERVER_PORT);
    p(" [doesn't work with xcode, use ./compile.sh ]");

	/*  Loop infinitely to accept and service connections  */
	while (1) {
		/*  Wait for connection  */
		if ((conn = accept(listener, NULL, NULL)) < 0)
			Error_Quit("Error calling accept()! debugging not supported, are you debugging?");
        else p("conn = accept OK");
		// WORKS FINE, but not when debugging

		/*  Fork child process to service connection  */
        pid = fork();
		if (pid == 0) {

			/*  This is now the forked child process, so
			close listening socket and service request   */
			if (close(listener) < 0)
				Error_Quit("Error closing listening socket in child.");

			Service_Request(conn);

			/*  Close connected socket and exit forked process */
			if (close(conn) < 0)
				Error_Quit("Error closing connection socket.");
			exit(EXIT_SUCCESS);
		}else{
            p("not forked yet");
//            Service_Request(conn);// whatever
        }


		/*  If we get here, we are still in the parent process,
			so close the connected socket, clean up child processes,
			and go back to accept a new connection.                   */

		waitpid(-1, NULL, WNOHANG);

		if (close(conn) < 0)
			Error_Quit("Error closing connection socket in parent.");

	}
	Error_Quit("FORK web server failed");
	return; // EXIT_FAILURE;    /*  We shouldn't get here  */
}


/*
int main(int argc, char *argv[]) {
	start_server();
}
 */

