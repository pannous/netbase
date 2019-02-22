//#pragma once
// All structured data from the main and property namespace is available under the Creative Commons CC0 License;

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>    /* socket definitions    */
#include <sys/types.h>    /* socket types       */
#include <sys/wait.h>     /* for waitpid()       */
#include <arpa/inet.h>    /* inet (3) funtions     */
#include <unistd.h>      /* misc. UNIX functions   */

#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>       /* For select() */
#include <unistd.h>
#include <fcntl.h>

#include "webserver.hpp"
#include "console.hpp" // parse request
#include "util.hpp"
#include "init.hpp"
#include "query.hpp"
#include "relations.hpp" // Node* Entity;
//#include "helper.h"

/* Service an HTTP request */
bool LIVE = true; // lower limits, higher security
int SERVER_PORT = 8080;
int MAX_QUERY_LENGTH = 1000;
//static char server_root[1000] = "/Users/me/";
//static char server_root[1000] = "./";
// explicit whitelist for files that can be served:
static char index_html[100] = "index.html";
static char netbase_js[100] = "netbase.js";
static char netbase_css[100] = "netbase.css";
static char favicon_ico[100] = "favicon.ico";
int resultLimit = 200; // != lookuplimit reset with every fork !!

//extern map<int,int> startPositions;
//extern map<int,int> endPositions;

pid_t pid;
int listener, conn, closing = 0;
struct sockaddr_in servaddr;
//  socklen_t

vector<char *> excluded;
vector<char *> included;
vector<int> excludedIds;
vector<int> includedIds;
bool showExcludes = false;
bool useView=false;// filter excludes and includes

int warnings = 0;

enum result_format {
	xml, js, json, txt, csv, html
};

enum result_verbosity {
	abstract, shorter, normal, longer, verbose, alle
} verbosity;


void fixLabels(Statement *s);

void getIncludes(Node *n);

void loadView(Node *n);

void loadView(char *q);

void fixLabel(Node *n);

bool checkSanity(char *q, int len);

bool checkHideStatement(Statement *s);

char *fixName(char *name);

deque<Statement *> sortStatements(Node *node, bool use_topic);

/* CENTRAL METHOD to parse and render html request*/
int handle(cchar *q0, int conn) {
//	autoIds = false;
//	Writeline("{'message':'Please use dev01 until going live'}"); return
	autoIds = true; // 2.7.18 : for http://sqb:8080/html/550866 !?!
	newQuery();
	int len = (int) strlen(q0);
	if (LIVE)lookupLimit = 10000;
	newQuery();
	char *q = editable(q0);
	if (!checkSanity(q, len)) {//	if(len>MAX_QUERY_LENGTH){ ...
		p("checkSanity :command OR len>10000");
		return 0;// SAFETY!
	}
	while (q[0] == '/')
		q++;

	enum result_format format = html;//txt; html DANGER WITH ROBOTS
	enum result_verbosity verbosity = alle;//normal;
	if (NO_TOPICS)p("NO_TOPICS!");
	if (contains(q, "robots.txt")) {
		Writeline(conn, "User-agent: *\n");
		Writeline("Disallow: /\n");
		return 0;
	}

	char *jsonp = strstr(q, "jsonp");// ?jsonp=fun
	if (jsonp) {
		jsonp[-1] = 0;
		jsonp += 6;
		format = json;
	} else jsonp = (char *) "parseResults";


	if (endsWith(q, "?")) {
		q[len - 1] = 0;
	}

	if (endsWith(q, ".json")) {
		format = json;
		q[len - 5] = 0;
	}

	if (endsWith(q, ".xml")) {
		format = xml;
		q[len - 4] = 0;
	}

	if (endsWith(q, ".csv") or endsWith(q, ".tsv")) {
		format = csv;
		q[len - 4] = 0;
	}

	if (endsWith(q, ".txt")) {
		format = txt;
		q[len - 4] = 0;
	}

	if (endsWith(q, ".html")) {
		format = html;
		q[len - 5] = 0;
	}
	if (startsWith(q, ".js")) {
		q[len - 3] = 0;
		Writeline(conn, jsonp);
		Writeline(conn, "(");
		format = js;
	}
	// todo : dedup!!
	if (startsWith(q, "all/")) {
		cut_to(q, " +");
		cut_to(q, " -");
		q = q + 4;
		showExcludes = false;
		verbosity = alle;
	}
	if (startsWith(q, "long/")) {
		verbosity = longer;
		q = q + 5;
	}
	if (startsWith(q, "full/")) {
		verbosity = verbose;
		q = q + 5;
	}
	if (startsWith(q, "abstract/")) {
		verbosity = abstract;
		q = q + 9;
	}
	if (startsWith(q, "verbose/")) {
		verbosity = verbose;
		q = q + 8;
	}
	if (startsWith(q, "short/")) {
		verbosity = shorter;
		q = q + 6;
	}

	if (startsWith(q, "html/")) {
		format = html;
		if (!contains(q, ".") and !contains(q, ":"))
			verbosity = verbose;
		q = q + 5;
	}
	if (startsWith(q, "plain/")) {
		format = txt;
		q = q + 6;
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
	if (startsWith(q, "csv/") or startsWith(q, "tsv/")) {
		format = csv;
		q = q + 4;
	}
	if (startsWith(q, "json/")) {
		format = json;
		q = q + 5;
	}
	if (startsWith(q, "js/")) {
		q = q + 3;
		Writeline(conn, jsonp);
		Writeline(conn, "(");
		format = js;
	}
	if (startsWith(q, "long/")) {
		verbosity = longer;
		q = q + 5;
	}
	if (startsWith(q, "verbose/")) {
		verbosity = verbose;
		q = q + 8;
	}
	if (startsWith(q, "abstract/")) {
		verbosity = abstract;
		q = q + 9;
	}
	if (startsWith(q, "short/")) {
		verbosity = shorter;
		q = q + 6;
	}
	if (startsWith(q, "the/")) {
		q[3] = ' ';
	}
	if (startsWith(q, "excludes/") or startsWith(q, "includes/") or startsWith(q, "excluded/") or
	    startsWith(q, "included/") or startsWith(q, "showview/")) {
		showExcludes = true;
		verbosity = longer;
		q = q + 9;
	} else showExcludes = false;
	excluded.clear();
	included.clear();

	if (contains(q, "statement count")) {
		Writeline(conn, itoa((int) context->statementCount).data());
		return 0;
	}
	if (contains(q, "node count")) {
		Writeline(conn, itoa(context->nodeCount).data());
		return 0;
	}
	if (contains(q, " limit ")) {
		char *limit = (char *) strstr(q, "limit");
		sscanf(limit, "limit %d", &resultLimit);
		pf("LIMIT SET TO %d\n", resultLimit);// quiet bug
		lookupLimit = resultLimit * 10;//todo
		if (limit > q) *(limit - 1) = 0;
		*limit = 0;
		//		if(len<2)return OK;
	}

	if (startsWith(q, "all/")) {
		cut_to(q, " +");
		cut_to(q, " -");
		q = q + 4;
		showExcludes = false;
		verbosity = alle;
	}
	bool get_norm=false;
	if (startsWith(q,"norm/")){
		get_norm=true;
		q[4]=' ';
		q=q+5;
	}
//	bool get_topic=false;
	bool get_topic = true;
	bool seo = false;
	if (startsWith(q, "seo/")) {
		q[3] = ' ';
		q = q + 4;
		seo = true;
	}
//	bool sort=false;
	bool entities= false;
	if (startsWith(q, "ee/") or startsWith(q, "ee ")) {
		q[2] = ' ';
		q = q + 3;// KEEP for parse command!
		get_topic = true;
		entities = true;
		verbosity = shorter;
	}
	if (startsWith(q, "entities/")) {
		q[8] = ' ';
		q = q + 9;// KEEP for parse command!
		get_topic = true;
		entities = true;
		verbosity=longer;
	}

	bool properties=false;
	if (startsWith(q,"qa/")){
		q[2]=' ';
		q=q+3;
		get_norm=true;
		get_topic = true;
		entities = true;
		properties= true;
		if(format!=html)
		verbosity = shorter;
	}
	if (startsWith(q, "?query=")) {
		q[6] = ' ';
		q = q + 7;
	}
	if (startsWith(q, "?q=")) {
		q[2] = ' ';
		q = q + 3;
	}
	if (startsWith(q, "query=")) {
		q[5] = ' ';
		q = q + 6;
	}
	if (startsWith(q, "q=")) {
		q[1] = ' ';
		q = q + 2;
	}

	if (hasWord(q) and useView) loadView(q);

	if (contains(q, "exclude") or contains(q, "include")) {
		verbosity = normal;
		showExcludes = true;
	}
	bool safeMode = true;
	if (startsWith(q, "query/")) { // AND ...
		q = q + 6;
//		safeMode = false;// RLLY?
	}
	if (startsWith(q, "synonym/")) { // AND ...
		q = q + 8;
		safeMode = false;// RLLY?
	}
	if (startsWith(q, ":learn "))safeMode=false;// RLLY?
	if (startsWith(q, ":lable "))safeMode=false;// RLLY?
	if (startsWith(q, ":delete "))safeMode=false;// RLLY?

	p(q);
	if (q[0] == ':')appendFile("logs/commands.log", q);
	else appendFile("logs/query.log", q);
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!
	NodeVector all;
	if(!safeMode) // <<<<<<<< HANDLE QUERY WITH NETBASE!
		all = parse(q, safeMode, false);
	else{
		if(properties)
			all= findAnswers(q);
		else if(entities)
		 all= findEntites(q);
		else if (isNumber(q))
			all = wrap(get(q));
		else if(seo)
			all = wrap(getSeo(q));
		else
			all = wrap(hasWord(q));;
	}
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!

	// all entities
	if(all.size()==1 and all[0]->kind==_abstract){ ///  and verbosity!=shorter
//				all.push_back(all[0]);
//		Node *the = all[0];
//	if(all[0]->statementCount==1)// instance:
      if(format == html){
		N the=getThe(all[0],0,false); // BUUUG DONT
		if(the) all[0] = the;
      }
//		Node *the = getThe("Samsung");
//				Node *the = getThe(all[0]);
//		all.push_back(the);// segfault WHY?
//		mergeVectors(&all,all_instances(all[0]));
	}

	if (contains(q, " limit ")) { strstr(q, " limit ")[0] = 0; }
	int size = (int) all.size();
	int count = (int) all.size();
	if (showExcludes) {
		for (int i = 0; i < size; i++) {
			// todo : own routine!!!
			Node *node = (Node *) all[i];
			if (!contains(all, getAbstract(node->name)))
				all.push_back(getAbstract(node->name));
			N parent = getType(node);
			if (parent) {
				if (!contains(all, parent))all.push_back(parent);
				N abs = getAbstract(parent->name);
				if (parent and !contains(all, abs))all.push_back(abs);
			}
		}
		show(excluded);
	}

	const char *html_block = "<!DOCTYPE html><html><head><META HTTP-EQUIV='CONTENT-TYPE' CONTENT='text/html; charset=UTF-8'/></head>\n"\
                            "<link rel='stylesheet' href='netbase.css'>\n"\
                            "<body class='netbase'><div id='netbase_results'></div>\n<script>var results=";
	//  if((int)all.size()==0)Writeline("0");
	//	Writeline(conn,q);
	char buff[10000];

	bool use_json = format == json or format == js or format == html;
	if (format == xml and (startsWith(q, "select") or contains(q, " where "))) {
		Writeline(conn, query2(q));
		return 0;
	}
	if (format == xml)Writeline(conn, "<results>\n");
	if (format == html)Writeline(conn, html_block);
//	if (use_json)Writeline(conn, "{\"results\":[\n");
	if (use_json)
		Writeline(conn, "{\"query\":\"" + string(fixName(q)) + "\", \"count\":\"" + itoa(count) + "\",\"results\":[\n");
	const char *statement_format_xml = "  <statement id='%d' subject=\"%s\" predicate=\"%s\" object=\"%s\" sid='%d' pid='%d' oid='%d'/>\n";
	const char *statement_format_text = "  $%d %s %s %s %d->%d->%d\n";
	const char *statement_format_json = "   { \"id\":%d, \"subject\":\"%s\", \"predicate\":\"%s\", \"object\":\"%s\", \"sid\":%d, \"pid\":%d, \"oid\":%d}";
//	const char* statement_format_json_long = "   { \"id\":%d, \"subject\":\"%s\", \"predicate\":\"%s\", \"object\":\"%s\", \"sid\":%d, \"pid\":%d, \"oid\":%d, \"title\":\"%s\"}";
	const char *statement_format_csv = "%d\t%s\t%s\t%s\t%d\t%d\t%d\n";
	const char *statement_format = 0;
	if (format == xml)statement_format = statement_format_xml;
	if (format == html)statement_format = statement_format_json;
	if (format == js)statement_format = statement_format_json;
	if (format == json)statement_format = statement_format_json;
	if (format == txt)statement_format = statement_format_text;
	if (format == csv)statement_format = statement_format_csv;

	const char *entity_format = 0;
	const char *entity_format_txt = "%s #%d (statements:%d) %s\n";
	const char *entity_format_xml = "<entity name=\"%s\" id='%d' statementCount='%d' description='%s'>\n";
	const char *entity_format_json = "  {\"name\":\"%s\", \"id\":%d, \"statementCount\":%d, \"description\":\"%s\"";
	const char *entity_format_csv = "%s\t%d\t%d\t%s\n";
	if (all.size() == 1)entity_format_csv = "";//statements!
	if (format == xml)entity_format = entity_format_xml;
	if (format == txt)entity_format = entity_format_txt;
	if (format == csv)entity_format = entity_format_csv;
	if (use_json) entity_format = entity_format_json;
	Node *last = 0;
	warnings = 0;
	char *entity = 0;
	if (startsWith(q, "all")) {
		entity = (char *) cut_to(q, " ");
		entity = keep_to(entity, "limit");
	}

	sortNodes(all);

	int good = 0;
	for (int i = 0; i < count and (i < resultLimit); i++) {
		Node *node = (Node *) all[i];
		if (!checkNode(node, -1, false, true))continue;
		if (node->id == 0)continue;
//		if(node->kind==_internal)continue;
		if (last == node)continue;
		if (eq(node->name, "◊"))continue;
		last = node;
		if (verbosity == normal and entity and eq(entity, node->name))continue;
		Node *abstracto = getAbstract(node);
		char *text = fixName(getText(node));
//		if(use_json and get_topic){
//			if(empty(text))continue;//! no description = no entity? BAD for amazon etc
//			if(isAbstract(node))continue;
//			N t=getTopic(node);
//		}
		good++;
		if (use_json)if (good > 1)Writeline(conn, "},\n");

		cchar *name = fixName(node->name);

		N kind=get(node->kind);
		if(kind and checkNode(kind) and node->kind>0){
			name = concat(concat(name, " "),kind->name);// ...
//			if(eq(kind->name,"Gram")) name = concat(name, "g");// ...
//			if(eq(kind->name,"Fach")) name = concat(name, " Fach");// ... zoom
//			if(eq(kind->name,"megapixel")) name = concat(name, " Megapixel");// ...
		}

		sprintf(buff, entity_format, name, node->id, node->statementCount, text);
		Writeline(conn, buff);
//    if(verbosity != alle and !get_topic)
//			loadView(node);
		bool got_topic = false;
		if (use_json and get_topic) {
			char *id=0;// side effect : set found_attribute and normed
			N key= findKey(node);
			if(checkNode(key))id = key->name;
			N ty = getType(node);
			N c = getClass(node,true,ty);
			N t = NO_TOPICS ? c : getTopic(node);
//			if (ty == Internal)continue;
			if (!checkNode(c, -1, false, true)) c = null;
			if (c and eq(c->name,node->name)) c = null;
			if (!checkNode(t, -1, false, true)) t = null;
			if (t and eq(t->name,node->name)) t = null;
			if (!checkNode(ty, -1, false, true))ty = null;
			if (ty and eq(ty->name,node->name)) ty = null;
			if(id && found_attribute==1)ty = Attribute;
			if(id && found_attribute==0)ty = getThe("attributevalue");// hack
//			if( c == Attribute)ty = Attribute; // why? SIM-Karten-Slot attributevalue hack
			if (c and c != Entity and c != _Node and  c!=node) {
				Writeline(conn, ",\n\t \"class_id\":" + itoa(c->id));
				Writeline(conn, ", \"class\":\"" + string(fixName(c->name)) + "\"");
			}
			if (ty and c != ty and ty!=node ) {
				Writeline(conn, ",\n\t \"type_id\":" + itoa(ty->id));
				Writeline(conn, ", \"type\":\"" + string(fixName(ty->name)) + "\"");
			}
			if (t and t != Entity and t!=node and ty != t and c != t ) {
				got_topic = true;
				Writeline(conn, ",\n\t \"topic_id\":" + itoa(t->id));
				Writeline(conn, ", \"topic\":\"" + string(fixName(t->name)) + "\"");
			}
			if (node->name and !empty(node->name)) {
				string seo = generateSEOUrl(node->name);
				if(!eq(seo.data(),node->name,1,1))
					Writeline(conn, ", \"seo\":\"" + seo + "\"");
			}
			if(get_norm){
				N n=0; //only
				if(ty!=Attribute and ty!=getThe("attributevalue"))
					n=normEntity(node, false);
				if(normed)n=normed;
				if(n){
					Writeline(conn, ",\n\t \"normed_id\":" + itoa(n->id));
					Writeline(conn, ", \"normed\":\"" + string(fixName(n->name)) + "\"");
				}
				if(id)replace(id, ' ', '_');// hack back
				if(id) Writeline(conn, ", \"key\":\"" + string(id) + "\"");// uid? nah: blau
				int start = startPositions[node->id];
				if(start<0)start = startPositions[abstracto->id];
				if(start>0){
					int end = endPositions[node->id];
					if(end<0)end=endPositions[abstracto->id];
					char *match = substr(q, start - 1, end);
					if(!eq(node->name,match,1,1))
						Writeline(conn, ", \"match\":\"" + string(match) + "\"");
				}
			}
		}
		if (use_json)
			Writeline(conn, ", \"kind\":" + itoa(node->kind));
		if (use_json and verbosity == alle and node->kind != _abstract and i == 0)
			Writeline(conn, ", \"abstract_id\":" + itoa(getAbstract(node->name)->id));
		if (use_json and verbosity == alle and node->kind == _abstract and i == 0 and checkNode(getThe(node,0,0)))
			Writeline(conn, ", \"main_id\":" + itoa(getThe(node)->id));
		bool show_images = verbosity == alle or format == html;
		if ((use_json and show_images) and verbosity != shorter and !showExcludes and node->statementCount > 1) {
			string img = getImage(node, 150,/*thumb*/true);
			if (img != "") {
				img = replace_all(replace_all(img, "'", "%27"), "\"", "%22");
				Writeline(", \"image\":\"" + img + "\"");
			}
		}
//		if((use_json) and getText(node)[0]!=0)
//			Writeline(", \"description\":\""+string(getText(node))+"\"");
		Statement *s = 0;
		if (format == csv or verbosity == normal or verbosity == verbose or verbosity == longer or verbosity == alle or
		    showExcludes or (all.size() == 1 and !(verbosity == shorter))) {
			int count = 0;
			//      Writeline(",image:\""+getImage(node->name)+"\"");
			if (use_json)Writeline(conn, ",\n\t \"statements\":[\n");

			
			deque<Statement *> statements= sortStatements(node, get_topic and !got_topic);// sort
			

			int good = 0;
			auto statementCount = statements.size();
//			for (int j = 0; j < statementCount and j <= resultLimit; j++) {
//				s = statements.at(j); use sortedAndFiltered
			while ((s = nextStatement(node, s)) and count++<resultLimit) { // unfiltered
				if (format == csv and all.size() > 1)break;// entities vs statements
				if(debug)p(s);
				if (verbosity != alle and checkHideStatement(s)) {
					warnings++;
					continue;
				}
				fixLabels(s);
				if (!(verbosity == verbose or verbosity == alle or verbosity == abstract) and
				    (s->Predicate() == Instance or s->Predicate() == Type))
					continue;
				if (verbosity == abstract and s->Predicate() != Instance and s->Predicate() != Type)continue;
				if (!checkNode(s->object, false, true))continue;
				if (use_json and good > 0)Writeline(conn, ",\n");
				char *objectName = s->Object()->name;
				if (s->Predicate() == Instance) {
					N type = getClass(s->Object());// findProperty(s->Object(),Type->name,0,50);
					if (checkNode(type, -1, false, true) and type->id != _entity and !contains(objectName, type->name))
						objectName=(char*)(concat(concat(objectName, ": "),type->name));
//						objectName = (char *) BUG!!!?? (string(objectName) + " (" + type->name + ")").data();
				}
				if (!objectName) {
					p("PROBLEM WITH");
					p(s);
//					objectName="???";
					continue;// hebrew?
				}

				if (objectName[strlen(objectName) - 1] == '\n')objectName[strlen(objectName) - 1] = 0;
				char *subject = fixName(s->Subject()->name);
				char *predicate = fixName(s->Predicate()->name);
				char *object = fixName(objectName);
//				if(verbosity==alle){
//					char* title=fixName(getStatementTitle(s,node)); ???
//					sprintf(buff, statement_format_json_long, s->id(),subject, predicate, object, s->Subject()->id, s->Predicate()->id, s->Object()->id,title);
				sprintf(buff, statement_format, s->id(), subject, predicate, object, s->Subject()->id,
				        s->Predicate()->id, s->Object()->id);

				Writeline(conn, buff);
				good++;
			}
			if (use_json)Writeline(conn, "]");
		}
		if (format == xml)Writeline(conn, "</entity>\n");
	}



	if (use_json or format == html or format == js)Writeline(conn, good > 0 ? "}\n]}" : "]}");
	if (format == xml)Writeline(conn, "</results>\n");
	if (format == js)Writeline(conn, ")");// jsonp
	const char *html_end = ";\n</script>\n<script src='netbase.js'></script></body></html>\n";
	if (format == html)Writeline(conn, html_end);
	//		sprintf(buff,	"<script src='/js/%s'></script>",q0);
	//		Writeline(conn, buff);
	//	}
//	pf("Warnings/excluded: %d\n", warnings);
	return 0;// 0K
}

char *fixName(char *name) {
	if (!name)return (char *) "";
	int len = (int) strlen(name);

	while (--len >= 0) {
		if (name[len] == '_')name[len] = ' ';// json-save!
		if (name[len] == 9)name[len] = ' ';// json-save!
		if (name[len] == '"')name[len] = '\'';// json-save!
		if (name[len] == '`')name[len] = '\'';// json-save!
	}
	if (name[0] == '\'' or name[0] == '`')return name + 1;
	if (startsWith(name, "entities"))
		name += 9;
	if (startsWith(name, "ee "))
		name += 3;
	return name;
}

// whats this??
char *getStatementTitle(Statement *s, Node *n) {
	if (!checkStatement(s))return (char *) "";
	if (s->Object() == n)return getText(s->Subject());
//	if(s->Subject()==n)
	return getText(s->Object());// default
}


deque<Statement *> sortStatements(Node *node, bool use_topic) {
	newQuery();
	deque<Statement *> statements;
	int count=0;
	Statement *s=0;
	while ((s = nextStatement(node, s)) and count++ < lookupLimit) {// resultLimit
		if (!checkStatement(s)) {
			p("!checkStatement(s)");
			bad();
			show(s);
			break;
		}
//				if (!checkStatement(s))continue;// DANGER!
//				if(!got_topic and ( s->predicate==_Type or s->predicate==_SuperClass)){
//					addStatementToNode(node, s->id(), true);// next time
//				}
		//
		if (use_topic and verbosity != verbose and verbosity != alle and verbosity != normal and
		    (s->predicate > 100 or s->predicate < -100))
			continue;// only important stuff here!
		// filter statements

//				if(eq(s->Predicate()->name,"Offizielle Website") and !contains(s->Object()->name,"www"))
//					continue;

		if (s->predicate == _derives or s->predicate == _derived) {// cognet
			statements.push_front(s);
			p("_derives!");
		} else if (s->subject == node->id and s->predicate != 4)//_instance
			statements.push_front(s);
		else statements.push_back(s);
	}
//			if(get_topic and verbosity!=shorter){
//				NV topics=getTopics(node);
//				N s=topics[0];
//				for (int j = 0; j < topics.size() and j<=resultLimit; j++) {
//					N s=topics[j];
//					Temporary statement (node,topic,s)
//					statements.push_front(s);
//				}
//			}
}


bool checkSanity(char *q, int len) {
	bool bad = false;
	if (len > MAX_QUERY_LENGTH) {
		q[MAX_QUERY_LENGTH] = 0;
//		p(checkSanity);
//		bad=true;
	}
	if (q[0] == ':' or q[0] == '!')bad = true;
	if(startsWith(q,":lable"))bad=false;
	for (int i = 0; i < len and i < MAX_QUERY_LENGTH; i++) {
		if (q[i] > 127)bad = true;// no illegal chars!
	}
	if (bad)
		appendFile("netbase.warnings", q);
	return !bad;
}

void removeSpecialChars(char *line) {
	int j;
	char last = 0;
	for (int i = 0; line[i] != '\0'; ++i) {
		char c = line[i];
		while (!((c >= 'a' and c <= 'z') or (c == ' ' and last != ' ') or (c >= 'A' and c <= 'Z') or c == '\0')) {
			for (j = i; line[j] != '\0'; ++j)
				line[j] = line[j + 1];
			line[j] = '\0';
			c = line[i];
		}
		last = c;
	}
}

void fixLabel(Node *n) {
	if (!checkNode(n))return;
	if (n->name == 0)return;// HOW? checkNames=false :(
	if (n->name[0] == '"')n->name = n->name + 1;

	if (n->name[strlen(n->name) - 1] == '"' and n->name[strlen(n->name) - 2] != '"')
		n->name[strlen(n->name) - 1] = 0;
	if (n->name[strlen(n->name) - 1] == '\\')
		n->name[strlen(n->name) - 1] = 0;
	replaceChar(n->name, '"', ' ');
	replaceChar(n->name, '\'', ' ');
	replaceChar(n->name, '\\', ' ');
	replaceChar(n->name, '_', ' ');
	// todo: "'","%27" etc
	//#include <curl/curl.h>
	//char *curl_easy_escape( CURL * curl , char * url , int length );
}

bool checkHideStatement(Statement *s) {
	if (s->predicate == 23025403)return true;// 	Topic equivalent webpage
//	if(eq(s->Predicate()->name,"Geographische Koordinaten"))continue;
	if (s->subject == 0 or s->predicate == 0 or s->object == 0) {
		warnings++;
		return true;
	}
	char *predicateName = s->Predicate()->name;
	char *objectName = s->Object()->name;
	char *subjectName = s->Subject()->name;
	if (subjectName == 0 or predicateName == 0 or objectName == 0) {
		warnings++;
		return true;
	}

	if (showExcludes) {
		if (eq(subjectName, "exclude", 1) or eq(predicateName, "exclude", 1) or eq(objectName, "exclude", 1))
			return false;
		if (eq(subjectName, "include", 1) or eq(predicateName, "include", 1) or eq(objectName, "include", 1))
			return false;
		return true;
	}

	if (eq(predicateName, "exclude")) {
		excluded.push_back(objectName);
		return true;
	}
	if (eq(predicateName, "include")) {
		included.push_back(objectName);
		return true;
	}
	if (predicateName[0] == '<')predicateName++;
	if (eq(predicateName, "Key"))return true;
	if (eq(predicateName, "expected type"))return true;
	if (eq(predicateName, "Range"))return true;
	if (eq(predicateName, "usage domain"))return true;
	if (eq(predicateName, "schema"))return true;
	if (startsWith(predicateName, "http"))return true;

	if (predicateName[2] == '-' or predicateName[2] == '_' or predicateName[2] == 0)
		return true;// zh-ch, id ...
	if (objectName[0] == '/' or objectName[1] == '/')return true;// ?


	for (int i = 0; i < excluded.size(); i++) {
		char *exclude = excluded.at(i);
		if (contains(subjectName, exclude, 1) or contains(predicateName, exclude, 1) or
		    contains(objectName, exclude, 1))
			return true;
		if (eq(itoa(s->subject), exclude) or eq(itoa(s->predicate), exclude) or eq(itoa(s->object), exclude))
			return true;
	}
	bool ok = included.size() == 0;// no filter
	for (int i = 0; i < included.size(); i++) {
		char *include = included.at(i);
//		if(eq(predicateName,"Bundesland"))
//			p(s);
		if (eq(itoa(s->subject), include) or eq(itoa(s->predicate), include) or eq(itoa(s->object), include))ok = true;
		if (contains(subjectName, include, 1) or contains(predicateName, include, 1) or
		    contains(objectName, include, 1))
			ok = true;
	}

	//  if(contains(predicateName,excluded,1) or contains(objectName,excluded,1) or contains(subjectName,excluded,1))return true;
	//  if(contains(predicateName,excluded2,1) or contains(objectName,excluded2,1) or contains(subjectName,excluded2,1))return true;
	//  if(contains(predicateName,excluded3,1) or contains(objectName,excluded3,1) or contains(subjectName,excluded3,1))return true;
	return !ok;
	//  return false;
}

void fixLabels(Statement *s) {
	fixLabel(s->Subject());
	fixLabel(s->Predicate());
	fixLabel(s->Object());
}

void getIncludes(Node *n) {
	if (verbosity == shorter or verbosity == alle)return;
	if (n->id < 0)return;
	pf("getIncludes %d >>%s<<\n", n->id, n->name);
	Statement *s = 0;
	int lookups = 0;
	while ((s = nextStatement(n, s)) and ++lookups < 50) {
		//    p(s);
		if (eq(s->Predicate()->name, "exclude")) {
			excluded.push_back(s->Object()->name);
			excludedIds.push_back(s->Object()->id);
		}
		if (eq(s->Predicate()->name, "include")) {
			included.push_back(s->Object()->name);
			includedIds.push_back(s->Object()->id);
		}
	}
}

void loadView(Node *n) {
	getIncludes(n);
	N parent = getType(n);
	if (parent)
		getIncludes(parent);
	if (parent and parent->kind != Abstract->kind)
		getIncludes(getAbstract(parent->name));
}

void loadView(char *q) {
	N ex = get("excluded");// globally
	if (ex and verbosity != alle)getIncludes(ex);
	ex = getAbstract(q);// todo AND TYPE city
	if (ex and verbosity != alle)getIncludes(ex);

	char *exclude = q;
	while (exclude and contains(exclude, " -")) {
		exclude = strstr(exclude, " -");
		if (exclude[2] != ' ') {// not 2009 - 2010 etc
			exclude[0] = 0;
			exclude += 2;
			if (verbosity != alle)
				excluded.push_back(exclude);
		} else exclude = 0;
	}
	char *include = q;
	while (include and contains(include, " +")) {
		include = strstr(exclude, " +");
		include[0] = 0;
		include += 2;
		if (verbosity != alle)
			included.push_back(include);
	}

}


// WORKS FINE, but not when debugging
int Service_Request(int conn) {
	int ok = 0;
	struct ReqInfo reqinfo;
	InitReqInfo(&reqinfo);
	/* Get HTTP request */
	if (Get_Request(conn, &reqinfo) < 0)
		return -1;
	else if (reqinfo.type == FULL)
		Output_HTTP_Headers(conn, &reqinfo);

	CleanURL(reqinfo.resource);
	initSharedMemory(); // for each forked process!
	if (strlen(reqinfo.resource) > 1000)return 0;// safety
	char *q = substr(reqinfo.resource, 1, -1);
	// ::::::::::::::::::::::::::::::
	if (strlen(q) == 0 or q[0] == '?' or q[strlen(q) - 1] == '/'
	    or contains(q, netbase_js) or contains(q, netbase_css) or contains(q, favicon_ico) or contains(q, index_html))
		Serve_Resource(reqinfo, conn);
	else
		handle(q, conn); // <<<<<<< CENTRAL CALL
	// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	FreeReqInfo(&reqinfo);
	return ok;
}


/* Prints an error message and quits */
void Error_Quit(char const *msg) {
	fprintf(stderr, "WEBSERV: %s\n", msg);
	exit(EXIT_FAILURE);
}

/* Read a line from a socket */
ssize_t Readline(int sockd, void *vptr, size_t maxlen) {
	ssize_t n, rc;
	char c, *buffer;

	buffer = (char *) vptr;

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

void Writeline(const char *s) {
	Writeline(lastSockd, s, 0);
}

void Writeline(string s) {
	Writeline(lastSockd, s.data(), 0);
}

/* Write a line to a socket */
ssize_t Writeline(int sockd, string s) {
	return Writeline(sockd, s.data(), s.length());
}

ssize_t Writeline(int sockd, const char *vptr, size_t n) {
	size_t nleft;
	ssize_t nwritten;
	const char *buffer;
	if (sockd == -1) {// debug
		p(vptr);
		return 0;
	}
	if (sockd == 0)sockd = lastSockd; //not thread safe!
	lastSockd = sockd;

	buffer = vptr;
	if (n == 0 or n == -1)
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

int Trim(char *buffer) {
	long n = strlen(buffer) - 1;

	while (!isalnum(buffer[n]) and n >= 0)
		buffer[n--] = '\0';

	return 0;
}

int StrUpper(char *buffer) {
	while (*buffer) {
		*buffer = toupper(*buffer);
		++buffer;
	}
	return 0;
}

/* Cleans up url-encoded string */
void CleanURL(char *buffer) {
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

/* Parses a string and updates a request information structure if necessary.  */
int Parse_HTTP_Header(char *buffer, struct ReqInfo *reqinfo) {

	static int first_header = 1;
	char *temp;
	char *endptr;
	size_t len;
	if (first_header == 1) {
		/* If first_header is 0, this is the first line of
     the HTTP request, so this should be the request line. */
		/* Get the request method, which is case-sensitive. This
     version of the server only supports the GET and HEAD
     request methods.                    */
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
		/* Skip to start of resource */
		while (*buffer and isspace(*buffer))
			buffer++;
		/* Calculate string length of resource... */
		endptr = strchr(buffer, ' ');
		if (endptr == NULL)
			len = strlen(buffer);
		else
			len = endptr - buffer;
		if (len == 0) {
			//			reqinfo->status = 400;
			//			return -1;
		}
		/* ...and store it in the request information structure. */
		reqinfo->resource = (char *) calloc(len + 1, sizeof(char));
		strncpy(reqinfo->resource, buffer, len);
		/* Test to see if we have any HTTP version information.
     If there isn't, this is a simple HTTP request, and we
     should not try to read any more headers. For simplicity,
     we don't bother checking the validity of the HTTP version
     information supplied - we just assume that if it is
     supplied, then it's a full request.            */
		if (contains(buffer, "HTTP/") or contains(buffer, "http/"))
			reqinfo->type = FULL;
		else
			reqinfo->type = SIMPLE;
		first_header = 0;
		return 0;
	}
	/* If we get here, we have further headers aside from the
   request line to parse, so this is a "full" HTTP request. */
	/* HTTP field names are case-insensitive, so make an
   upper-case copy of the field name to aid comparison.
   We need to make a copy of the header up until the colon.
   If there is no colon, we return a status code of 400
   (bad request) and terminate the connection. Note that
   HTTP/1.0 allows (but discourages) headers to span multiple
   lines if the following lines start with a space or a
   tab. For simplicity, we do not allow this here.       */
	endptr = strchr(buffer, ':');
	if (endptr == NULL) {
		//		reqinfo->status = 400;
		return -1;//ok
	}
	temp = (char *) calloc((endptr - buffer) + 1, sizeof(char));
	strncpy(temp, buffer, (endptr - buffer));
	StrUpper(temp);
	/* Increment buffer so that it now points to the value.
   If there is no value, just return.          */
	buffer = endptr + 1;
	while (*buffer and isspace(*buffer))
		++buffer;
	if (*buffer == '\0')
		return 0;
	/* Now update the request information structure with the
   appropriate field value. This version only supports the
   "Referer:" and "User-Agent:" headers, ignoring all others. */
	if (!strcmp(temp, "USER-AGENT")) {
		reqinfo->useragent = (char *) malloc((strlen(buffer) + 1 )* sizeof(char));
		strcpy(reqinfo->useragent, buffer);
	} else if (!strcmp(temp, "REFERER")) {
		reqinfo->referer = (char *) malloc((strlen(buffer) + 1 )* sizeof(char));
		strcpy(reqinfo->referer, buffer);
	}
	free(temp);
	return 0;
}

/* Gets request headers. A CRLF terminates a HTTP header line,
 but if one is never sent we would wait forever. Therefore,
 we use select() to set a maximum length of time we will
 wait for the next complete header. If we timeout before
 this is received, we terminate the connection.        */
int Get_Request(int conn, struct ReqInfo *reqinfo) {
	char buffer[MAX_REQ_LINE] = {0};
	int rval;
	fd_set fds;
	struct timeval tv;
	/* Set timeout to 5 seconds */
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	/* Loop through request headers. If we have a simple request,
   then we will loop only once. Otherwise, we will loop until
   we receive a blank line which signifies the end of the headers,
   or until select() times out, whichever is sooner.        */
	do {
		/* Reset file descriptor set */
		FD_ZERO(&fds);
		FD_SET(conn, &fds);
		/* Wait until the timeout to see if input is ready */
		rval = select(conn + 1, &fds, NULL, NULL, &tv);
		/* Take appropriate action based on return from select() */
		if (rval < 0) {
			Error_Quit("Error calling select() in get_request()");
		} else if (rval == 0) {
//			p(" input not ready after timeout "); seems non-severe? happened after 2017 why??
			return -1;
		} else {
			/* We have an input line waiting, so retrieve it */
			Readline(conn, buffer, MAX_REQ_LINE - 1);
			//	  Trim(buffer);
			if (buffer[0] == '\0')
				break;
			if (Parse_HTTP_Header(buffer, reqinfo))
				break;
		}
	} while (reqinfo->type != SIMPLE);
	return 0;
}

void InitReqInfo(struct ReqInfo *reqinfo) {
	reqinfo->useragent = NULL;
	reqinfo->referer = NULL;
	reqinfo->resource = NULL;
	reqinfo->method = UNSUPPORTED;
	reqinfo->status = 200;
	reqinfo->type = (Req_Type) FULL;// SIMPLE, FULL
}

void FreeReqInfo(struct ReqInfo *reqinfo) {
	if (reqinfo->useragent)
		free(reqinfo->useragent);
	if (reqinfo->referer)
		free(reqinfo->referer);
	if (reqinfo->resource)
		free(reqinfo->resource);
}

int Return_Resource(int conn, int resource, struct ReqInfo *reqinfo) {
	char c;
	size_t i;
	while ((i = read(resource, &c, 1))) {
		if (i < 0)
			Error_Quit("Error reading from file.");
		if (write(conn, &c, 1) < 1)
			Error_Quit("Error sending file.");
	}
	return 0;
}

/* Tries to open a resource. The calling function can use
 the return value to check for success, and then examine
 errno to determine the cause of failure if neceesary.  */
int Check_Resource(struct ReqInfo *reqinfo) {
	/* Resource name can contain urlencoded
   data, so clean it up just in case.  */
	CleanURL(reqinfo->resource);
	/* Concatenate resource name to server root, and try to open */
//	strcat(server_root, reqinfo->resource);// DONT allow arbitrary files
	if (contains(reqinfo->resource, "netbase.js"))
		return open(netbase_js, O_RDONLY);
	if (contains(reqinfo->resource, "netbase.css"))
		return open(netbase_css, O_RDONLY);
	if (contains(reqinfo->resource, "favicon.ico"))
		return open(favicon_ico, O_RDONLY);

//	return open(string("./")+reqinfo->resource, O_RDONLY);
	else
		return open(index_html, O_RDONLY);
}

int Return_Error_Msg(int conn, struct ReqInfo *reqinfo) {
	char buffer[100];
	sprintf(buffer, "<HTML>\n<HEAD>\n<TITLE>Server Error %d</TITLE>\n</HEAD>\n\n", reqinfo->status);
	Writeline(conn, buffer, strlen(buffer));
	sprintf(buffer, "<BODY>\n<H1>Server Error %d</H1>\n", reqinfo->status);
	Writeline(conn, buffer, strlen(buffer));
	sprintf(buffer, "<P>The request could not be completed.</P>\n"
			"</BODY>\n</HTML>\n");
	Writeline(conn, buffer, strlen(buffer));
	return 0;
}

int Output_HTTP_Headers(int conn, struct ReqInfo *reqinfo) {
	char buffer[100];
	sprintf(buffer, "HTTP/1.1 %d OK\r\n", reqinfo->status);
	Writeline(conn, buffer, strlen(buffer));
	if (contains(reqinfo->resource, "text/") or contains(reqinfo->resource, "txt/") or
	    contains(reqinfo->resource, "plain/"))
		Writeline(conn, "Content-Type: text/plain; charset=utf-8\r\n");
	else if (contains(reqinfo->resource, "json/") or contains(reqinfo->resource, "learn") or
	         contains(reqinfo->resource, "delete")) {
		Writeline(conn, "Content-Type: application/json; charset=utf-8\r\n");
		Writeline(conn, "Access-Control-Allow-Origin: *\r\n");// http://quasiris.com
	} else if (contains(reqinfo->resource, "csv/"))
		Writeline(conn, "Content-Type: text/plain; charset=utf-8\r\n");
	else if (contains(reqinfo->resource, "tsv/"))
		Writeline(conn, "Content-Type: text/plain; charset=utf-8\r\n");
	else if (contains(reqinfo->resource, "xml/"))
		Writeline(conn, "Content-Type: text/plain; charset=utf-8\r\n");// till entities are fixed
	else if (contains(reqinfo->resource, ".css"))
		Writeline(conn, "Content-Type: text/css; charset=utf-8\r\n");
	else if (endsWith(reqinfo->resource, ".ico"))
		Writeline(conn, "Content-Type: image/x-icon\r\n");
//		Writeline(conn, "Content-Type: application/xml; charset=utf-8\r\n");
	else
		Writeline(conn, "Content-Type: text/html; charset=utf-8\r\n");
//	Writeline(conn, "Cache-Control:max-age=10000, public\r\n");
	Writeline(conn, "Connection: close\r\n");
	Writeline(conn, "Server: Netbase\r\n");
	Writeline(conn, "\r\n", 2);
//	Writeline(conn, "\r\n", 2);
	return 0;
}

void Serve_Resource(ReqInfo reqinfo, int conn) {
	int resource = 0;
//	p("Serve_Resource!!\n");
	/* Check whether resource exists, whether we have permission
   to access it, and update status code accordingly.     */
	if (reqinfo.status == 200)
		if ((resource = Check_Resource(&reqinfo)) < 0) {
			if (errno == EACCES)
				reqinfo.status = 401;
			else
				reqinfo.status = 404;
		}
	/* Output HTTP response headers if we have a full request */
//	if (reqinfo.type == FULL) done
//		Output_HTTP_Headers(conn, &reqinfo);
	/* Service the HTTP request */
	if (Return_Resource(conn, resource, &reqinfo))
		Error_Quit("Something wrong returning resource.");
//	  else
//		Return_Error_Msg(conn, &reqinfo);
//  
	if (resource > 0)
		if (close(resource) < 0)
			Error_Quit("Error closing resource.");
//	FreeReqInfo(&reqinfo);
}

void start_server(int port = SERVER_PORT) {
	printf("STARTING SERVER!\n http://localhost:%d\n", port);
	if (port < 1024)p("sudo netbase if port < 1024!");
	flush();
	/* Create socket */
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		Error_Quit("Couldn't create listening socket.");

	int flag = 1;// allow you to bind a local port that is in TIME_WAIT.
	//	This is very useful to ensure you don't have to wait 4 minutes after killing a server before restarting it.
	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

	/* Populate socket address structure */
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	/* Assign socket address to socket */
	//	__bind<int&,sockaddr *,unsigned long> x=
	bind(listener, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if (listener < 0)
		Error_Quit("Couldn't bind listening socket.");

	/* Make socket a listening socket */
	//  	if (listen(listener, BACKLOG) < 0)
	if (listen(listener, LISTENQ) < 0)
		Error_Quit("Call to listen failed.");

	printf("listening on %d port %d\n", INADDR_ANY, port);
	p(" [debugging server doesn't work with xcode, use ./compile.sh ]");

	/* Loop infinitely to accept and service connections */
	while (1) {
		/* Wait for connection */
		// NOT with XCODE -> WEBSERV
		conn = accept(listener, NULL, NULL);
		if (conn < 0)
			Error_Quit("Error calling accept()! debugging not supported, are you debugging?");
		else p("connection accept OK");
		// WORKS FINE, but not when debugging
		/* Fork child process to service connection */
		pid = fork();
		if (pid == 0) {
			/* This is now the forked child process, so
       close listening socket and service request  */
			if (close(listener) < 0)
				Error_Quit("Error closing listening socket in child.");
			Service_Request(conn);
			/* Close connected socket and exit forked process */
			if (close(conn) < 0)
				Error_Quit("Error closing connection socket.");
			exit(EXIT_SUCCESS);
		} else {
//      p("not forked yet"); // !!?!
//      Service_Request(conn);// whatever
		}
		/* If we get here, we are still in the parent process,
     so close the connected socket, clean up child processes,
     and go back to accept a new connection.
     */
		waitpid(-1, NULL, WNOHANG);

		if (close(conn) < 0)
			Error_Quit("Error closing connection socket in parent.");

	}
	Error_Quit("FORK web server failed");
	return; // EXIT_FAILURE;  /* We shouldn't get here */
}


/*
 int main(int argc, char *argv[]) {
 start_server();
 }
 */

