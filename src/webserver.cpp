//#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <sys/wait.h>         /*  for waitpid()             */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>             /*  For select()  */
#include <unistd.h>
#include <fcntl.h>

#include "webserver.hpp"
#include "console.hpp" // parse request
#include "util.hpp"
#include "init.hpp"
#include "query.hpp"
//#include "helper.h"

/*  Service an HTTP request  */

#define SERVER_PORT  (80)
static char server_root[1000] = "/Users/me/";

int listener, conn,closing=0;
pid_t pid;
//    socklen_t
struct sockaddr_in servaddr;

enum result_format {
	xml, json, txt,csv,html
};

enum result_verbosity {
	shorter, normal, longer,verbose,alle
}verbosity;


void fixLabel(Node* n){
    if(!checkNode(n))return;
    if(n->name==0)return;// HOW? checkNames=false :(
	if(n->name[0]=='"')n->name=n->name+1;

	if(n->name[strlen(n->name)-1]=='"'&&n->name[strlen(n->name)-2]!='"')
		n->name[strlen(n->name)-1]=0;
	if(n->name[strlen(n->name)-1]=='\\')
		n->name[strlen(n->name)-1]=0;
	replaceChar(n->name,'"',' ');
	replaceChar(n->name,'\'',' ');
}

/// true = filter
vector<char*>excluded;
vector<char*>included;
vector<int>excludedIds;
vector<int>includedIds;
bool showExcludes=false;
int warnings=0;
//static char* excluded=0;
//static char* excluded2=0;
//static char* excluded3=0;
bool checkHideStatement(Statement* s){
	if(s->predicate==23025403)return true;// 	Topic equivalent webpage
    if(s->subject==0||s->predicate==0||s->object==0){warnings++;return true;}
	char* predicateName=s->Predicate()->name;
	char* objectName=s->Object()->name;
	char* subjectName=s->Subject()->name;
    if(subjectName==0||predicateName==0||objectName==0){warnings++;return true;}
    
    if(showExcludes){
        if(eq(subjectName,"exclude",1)||eq(predicateName,"exclude",1)||eq(objectName,"exclude",1))return false;
        if(eq(subjectName,"include",1)||eq(predicateName,"include",1)||eq(objectName,"include",1))return false;
        return true;
    }
    
    if(eq(predicateName,"exclude")){excluded.push_back(objectName);return true;}
    if(eq(predicateName,"include")){included.push_back(objectName);return true;}
    if(predicateName[0]=='<')predicateName++;
	if(eq(predicateName,"Key"))return true;
   	if(eq(predicateName,"expected type"))return true;
   	if(eq(predicateName,"Range"))return true;
    if(eq(predicateName,"usage domain"))return true;
    if(eq(predicateName,"schema"))return true;
    if(startsWith(predicateName,"http"))return true;
    
    if(predicateName[2]=='-'||predicateName[2]=='_'||predicateName[2]==0)
    	return true;// zh-ch, id ...
    if(objectName[0]=='/'||objectName[1]=='/')return true;// ?
    
    
    for(int i=0;i<excluded.size();i++){
        char* exclude=excluded.at(i);
        if(contains(subjectName,exclude,1)||contains(predicateName,exclude,1)||contains(objectName,exclude,1))return true;
        if(eq(itoa(s->subject),exclude)||eq(itoa(s->predicate),exclude)||eq(itoa(s->object),exclude))return true;
    }
    bool ok=included.size()==0;// no filter
    for(int i=0;i<included.size();i++){
        char* include=included.at(i);
        if(eq(predicateName,"Bundesland"))
            p(s);
        if(eq(itoa(s->subject),include)||eq(itoa(s->predicate),include)||eq(itoa(s->object),include))ok=true;
        if(contains(subjectName,include,1)||contains(predicateName,include,1)||contains(objectName,include,1))
            ok=true;
    }
    
    //    if(contains(predicateName,excluded,1)||contains(objectName,excluded,1)||contains(subjectName,excluded,1))return true;
    //    if(contains(predicateName,excluded2,1)||contains(objectName,excluded2,1)||contains(subjectName,excluded2,1))return true;
    //    if(contains(predicateName,excluded3,1)||contains(objectName,excluded3,1)||contains(subjectName,excluded3,1))return true;
   return !ok;
//    return false;
}

void fixLabels(Statement* s){
	fixLabel(s->Subject());
	fixLabel(s->Predicate());
	fixLabel(s->Object());
}

void getIncludes(Node* n){
	if(verbosity==shorter||verbosity==alle)return;
	if(n->id<1000)return;
	if(eq("Release track",n->name))return;
	if(eq("Recording",n->name))return;
	if(eq("Document",n->name))return;
	if(eq("Cataloged instance",n->name))return;
    pf("getIncludes %d >>%s<<\n",n->id,n->name);
    Statement *s=0;
	int maxLookups=50;
    while((s=nextStatement(n,s))){
		if(maxLookups--<0)break;
//        p(s);
        if(eq(s->Predicate()->name,"exclude")){
            excluded.push_back(s->Object()->name);
            excludedIds.push_back(s->Object()->id);
        }
        if(eq(s->Predicate()->name,"include")){
            included.push_back(s->Object()->name);
            includedIds.push_back(s->Object()->id);
        }
    }
    
}

void loadView(Node* n){
    getIncludes(n);
    N parent= getType(n);
    if(parent)
        getIncludes(parent);
    if(parent&&parent->kind!=Abstract->kind)
        getIncludes(getAbstract(parent->name));
}

void loadView(char* q){
    N ex=get("excluded");// globally
    if(ex &&   verbosity != alle )getIncludes(ex);
    ex=getAbstract(getAbstract(q)->name);// todo AND TYPE city
    if(ex && verbosity != alle )getIncludes(ex);
    
   char* exclude=q;
    while(exclude&&contains(exclude," -")){
        exclude=strstr(exclude," -");
        if(exclude[2]!=' '){// not 2009 - 2010 etc
            exclude[0]=0;
            exclude+=2;
            if(verbosity != alle)
            excluded.push_back(exclude);
        }else exclude=0;
    }
    char* include=q;
    while(include&&contains(include," +")){
        include=strstr(exclude," +");
        include[0]=0;
        include+=2;
        if(verbosity != alle)
        included.push_back(include);
    }
    
}

/* CENTRAL METHOD to parse and render html request*/
int handle(cchar* q0,int conn){
    char* q=editable(q0);
    while(q[0]=='/')q++;
	enum result_format format = txt;
	enum result_verbosity verbosity = normal;
	int len=(int)strlen(q);
	if (eq(q, "favicon.ico"))return 0;
    if(contains(q,"robots.txt")){
        Writeline(conn,"User-agent: *\n");
        Writeline("Disallow: /\n");
        return 0;
    }
    
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
		Writeline(conn, "parseResults(");
		format = json;
	}
	if (startsWith(q, "html/")) {
        format = html;
        if(!contains(q,".")&&!contains(q,":"))
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
            Writeline(conn, "parseResults(");
		format = json;
	}
	else if (startsWith(q, "json/")) {
		format = json;
		q = q + 5;
	}
	if (startsWith(q, "long/")) {
		verbosity = longer;
		q = q + 5;
	}
	if (startsWith(q, "verbose/")) {
		verbosity = verbose;
		q = q + 8;
	}
	if (startsWith(q, "short/")) {
		verbosity = shorter;
		q = q + 6;
	}
	if (startsWith(q, "excludes/")||startsWith(q, "includes/")||startsWith(q, "excluded/")||startsWith(q, "included/")||startsWith(q, "showview/")) {
        showExcludes=true;
        verbosity=longer;
		q = q + 9;
	}
    else showExcludes=false;
    excluded.clear();
    included.clear();
    
    if(contains(q,"statement count")){Writeline(conn,itoa(currentContext()->statementCount).data());return 0;}
    if(contains(q,"node count")){Writeline(conn,itoa(currentContext()->nodeCount).data());return 0;}
    
    
	if (startsWith(q, "all/")) {
        cut_to(q," +");
        cut_to(q," -");
		q = q + 4;
		showExcludes=false;
		verbosity = alle;
    }
    loadView(q);
    
    if(contains(q,"exclude")||contains(q,"include")){
        verbosity=normal;
        showExcludes=true;
    }
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!
	//
    NodeVector all = parse(q); // <<<<<<<< HANDLE QUERY WITH NETBASE!
    //
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!
	
    int size=(int)all.size();
    if(showExcludes){
        for (int i = 0; i < size; i++) {
        // todo : own routine!!!
        Node* node = (Node*) all[i];
        if(!contains(all,getAbstract(node->name)))
            all.push_back(getAbstract(node->name));
        N parent= getType(node);
        if(parent){
            if(!contains(all,parent))all.push_back(parent);
            N abs= getAbstract(parent->name);
            if(parent&&!contains(all,abs))all.push_back(abs);
        }
        }
        show(excluded);
    }
    
    const char* html_block="<html><META HTTP-EQUIV='CONTENT-TYPE' CONTENT='text/html; charset=UTF-8'><body><div id='results'></div><script>var results={'results':[\n";
    
    //    if((int)all.size()==0)Writeline("0");
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
	const char* entity_format_txt = "%s #%d statements:%d\n";
	const char* entity_format_xml = "<entity name=\"%s\" id='%d' statementCount='%d'>\n";
	const char* entity_format_json = "   {'name':\"%s\", 'id':%d, 'statementCount':%d";
   	const char* entity_format_csv = "%s\t%d\t%d\n";
    if(all.size()==1)entity_format_csv = "";//statements!
	if (format == xml)entity_format = entity_format_xml;
	if (format == html)entity_format = entity_format_json;
	if (format == json)entity_format = entity_format_json;
	if (format == txt)entity_format = entity_format_txt;
	if (format == csv)entity_format = entity_format_csv;
	Node* last=0;
    warnings=0;
    
    char* entity=0;
    if(startsWith(q,"all")){
        entity=(char*)cut_to(q," ");
        entity=keep_to(entity,"limit");
    }
    
	for (int i = 0; i < all.size(); i++) {
		Node* node = (Node*) all[i];
		if(last==node)continue;
		last=node;
        if(verbosity ==normal && entity&& eq(entity,node->name))continue;
        
		sprintf(buff, entity_format, node->name, node->id,node->statementCount);
		Writeline(conn, buff);
        if(verbosity != alle)
            loadView(node);
        if(verbosity==verbose||verbosity==shorter)// lol // just name
            Writeline(conn, ", 'kind':"+itoa(node->kind));
		Statement* s = 0;
		if (format==csv|| verbosity == verbose || verbosity == longer|| verbosity == alle ||showExcludes || ( all.size() == 1 && !verbosity == shorter)) {
            if((format == json||format == html)&&!showExcludes&&node->statementCount>1 && getImage(node)!="")
                Writeline(", 'image':'"+getImage(node,150,/*thumb*/true)+"'");
            //            Writeline(",image:'"+getImage(node->name)+"'");
			if (format == json||format == html)Writeline(conn, ", 'statements':[\n");
			int count=0;
			while ((s = nextStatement(node, s))&&count++<resultLimit) {
                if(format==csv&&all.size()>1)break;// entities vs statements
                p(s);
				if (!checkStatement(s))continue;
				if(verbosity!=alle&&checkHideStatement(s)){warnings++;continue;}
				fixLabels(s);
				if(!(verbosity==verbose||verbosity==alle) && (s->Predicate()==Instance||s->Predicate()==Type))continue;
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
    if(contains(q0,"js/"))Writeline(conn, ");");// jsonp
	if (format == html)Writeline(conn, html_end);
	if (format == xml)Writeline(conn, "</results>\n");
    pf("Warnings/excluded: %d\n",warnings);
    return 0;// 0K
}


// WORKS FINE, but not when debugging
int Service_Request(int conn) {
    
	struct ReqInfo reqinfo;
	InitReqInfo(&reqinfo);
    
	/*  Get HTTP request  */
	if (Get_Request(conn, &reqinfo) < 0)
		return -1;
    
	if (reqinfo.type == FULL)
		Output_HTTP_Headers(conn, &reqinfo);
    
	// file system:	//		Serve_Resource(ReqInfo  reqinfo,int conn)
    
	CleanURL(reqinfo.resource);
    
	init(); // for each forked process!
    if(strlen(reqinfo.resource)>1000)return 0;
	char* q = substr(reqinfo.resource, 1, -1);
    
    int ok=handle(q,conn); // <<<<<<<<<<<<<
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!
	FreeReqInfo(&reqinfo);
	return ok;
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

int lastSockd = -1;
void Writeline(const char* s) {
	Writeline(lastSockd, s, 0);
}
void Writeline(string s) {
	Writeline(lastSockd, s.data(), 0);
}

/*  Write a line to a socket  */
ssize_t Writeline(int sockd, string s) {
	return Writeline(sockd, s.data(), s.length());
}
ssize_t Writeline(int sockd, const char *vptr, size_t n) {
	size_t nleft;
	ssize_t nwritten;
	const char *buffer;
    if(sockd==-1){// debug
        p(vptr);
        return 0;
    }
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

int Trim(char * buffer) {
	long n = strlen(buffer) - 1;
    
	while (!isalnum(buffer[n]) && n >= 0)
		buffer[n--] = '\0';
    
	return 0;
}

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

/*  Parses a string and updates a request  information structure if necessary.    */
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

void InitReqInfo(struct ReqInfo * reqinfo) {
	reqinfo->useragent = NULL;
	reqinfo->referer = NULL;
	reqinfo->resource = NULL;
	reqinfo->method = UNSUPPORTED;
	reqinfo->status = 200;
}

void FreeReqInfo(struct ReqInfo * reqinfo) {
	if (reqinfo->useragent)
		free(reqinfo->useragent);
	if (reqinfo->referer)
		free(reqinfo->referer);
	if (reqinfo->resource)
		free(reqinfo->resource);
}

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

int Output_HTTP_Headers(int conn, struct ReqInfo * reqinfo) {
	char buffer[100];
	sprintf(buffer, "HTTP/1.0 %d OK\r\n", reqinfo->status);
	Writeline(conn, buffer, strlen(buffer));
	Writeline(conn, "Server: Netbase \r\n", 24);
	Writeline(conn, "Content-Type: text/html\r\n", 25);
	Writeline(conn, "\r\n", 2);
	return 0;
}

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

void start_server() {
	printf("STARTING SERVER!\n localhost:%d\n", SERVER_PORT);
	flush();
    
    
	/*  Create socket  */
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		Error_Quit("Couldn't create listening socket.");
    
	int flag = 1;// allow you to bind a local port that is in TIME_WAIT.
    //	This is very useful to ensure you don't have to wait 4 minutes after killing a server before restarting it.
	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    
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
		conn = accept(listener, NULL, NULL);
		if (conn  < 0)
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
            //            p("not forked yet");
            //            Service_Request(conn);// whatever
        }
        
        
		/*  If we get here, we are still in the parent process,
         so close the connected socket, clean up child processes,
         and go back to accept a new connection.
         */
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

