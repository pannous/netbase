#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <string.h>
#include <vector>
#include <string>

#include "webserver.hpp"

#include "netbase.hpp"
#include "util.hpp"
#include "export.hpp"
#include "import.hpp"
#include "tests.hpp"
#include "query.hpp"
#include "init.hpp"
#include "relations.hpp"

#define USE_READLINE
// compile with -lreadline !
#ifdef USE_READLINE
#include <readline/history.h>
#include <readline/readline.h>
#endif

using namespace std;
// static struct termios stored_settings;

void showHelp() {
	ps("available commands");
	ps("help :h or ?");
	ps("load :l [force]\tfetch the graph from disk or mem");
	//	ps("load_files :lf");
	ps("import :i [<file>]");
	ps("export :e (all to csv)");
	//    ps("print :p");
	ps("delete :d <node|statement|id|name>");
	ps("save :s or :w");
	//	ps("save and exit :x");
	ps("server");
	ps("quit :q");
	ps("clear :cl");
	ps("limit <n>");
	ps("Type words, ids, or queries:");
	//    ps("all animals that have feathers");
	ps("all animals with feathers");
	ps("select * from dogs where terrier");
	ps("all city with countrycode ZW");
	ps("Population of Gehren");
	ps("opposite of bad");
	//	ps("update city set type=town where population<10000");
}

bool file_read_done=false;

void getline(char *buf) {
	if (buf == 0) return; // end
	int MAXLENGTH=1000;
	const char* PROMPT="netbase> ";
#ifdef RL_READLINE_VERSION // USE_READLINE
	if (!file_read_done) file_read_done=1 + read_history(0);
	char *tmp=readline(PROMPT);
	if (tmp == 0 || strlen(tmp) == 0) {
		return;
	}
	tmp=fixQuotesAndTrim(tmp);
	if (strncmp(tmp, buf, MAXLENGTH) && strlen(tmp) > 0) add_history(tmp); // only add new content
	strncpy(buf, tmp, MAXLENGTH);
	buf[MAXLENGTH - 1]='\0';
	write_history(0);
	//	append_history(1,0);
	//            free(tmp);
#else
	std::cout << PROMPT;
	std::cin.get(buf, MAXLENGTH);
	std::cin.ignore(); // delete CR
#endif
}

//bool parse(string* data) {
static char* lastCommand;

Node *parseProperty(const char *data) {
	char *thing=(char *) malloc(1000);
	char *property=(char *) malloc(1000);
	if (contains(data, " of ")) sscanf(data, "%s of %s", property, thing);
	if (contains(data, " by ")) sscanf(data, "%s by %s", property, thing);
	else {
		//			sscanf(data,"%s.%s",thing,property);
		char** splat=splitStringC(data, '.');
		thing=splat[0];
		property=splat[1];
	}
	if (!property) {
		char** splat=splitStringC(data, ' ');
		thing=splat[0];
		property=splat[2];
	}
	pf("does %s have a %s?\n", thing, property);
	Node* found=has(getThe(thing), getAbstract(property));
	if (found == 0) found=has(getAbstract(thing), getAbstract(property));
	if (checkNode(found)) {
		show(found);
		pf("ANSWER: %s\n", found->name);
	}
	return found;
}

NodeVector parseProperties(const char *data) {
	char *thing=(char *) malloc(1000);
	char *property=(char *) malloc(1000);
	if (contains(data, " of ")) sscanf(data, "%s of %s", property, thing);
	if (contains(data, " by ")) sscanf(data, "%s by %s", property, thing);
	else {
		//			sscanf(data,"%s.%s",thing,property);
		char** splat=splitStringC(data, '.');
		thing=splat[0];
		property=splat[1];
	}
	if (!property) {
		char** splat=splitStringC(data, ' ');
		thing=splat[0];
		property=splat[2];
	}
	if (property[strlen(property) - 1] == 's') property[strlen(property) - 1]=0;

	pf("does %s have %s?\n", thing, property);
	NodeVector all=findProperties(thing, property);
	showNodes(all);
	return all;
}

NodeVector nodeVectorWrap(Node* n) {
	NodeVector r;
	r.push_back(n);
	return r;
}

NodeVector OK;


NodeVector parse(const char* data) {
	if (eq(data, null)) return OK;
	if (!isprint(data[0])) // ??
		return OK;
	if (eq(data, "")) {
		return OK;
	}
	clearAlgorithmHash(true); //  maybe messed up
	data=fixQuotesAndTrim(modifyConstChar(data));
	//	std::remove(arg.begin(), arg.end(), ' ');
	vector<char*> args=splitString(data, " "); // WITH 0==cmd!!!

	//		scanf ( "%s", data );
	if (eq(data, "exit")) return OK;
	if (eq(data, "help") || eq(data, "?")) {
		showHelp();
		//        printf("type exit or word");
		return OK;
	}

	if (eq(data, "more")) {
		resultLimit=resultLimit * 2;
		if (lastCommand) return parse(lastCommand);
		else return OK;
	}

	if (eq(data, ":x")) {
		save();
		exit(1);
	}
	lastCommand=clone(data);
	if (eq(data, ":w")) save();
	if (eq(data, ":q")) exit(1);

	if (eq(data, "q")) return OK;
	if (eq(data, "x")) return OK;
	if (eq(data, "export")) {
		export_csv();
		return OK;
	}
	if (eq(data, ":cl")) {
		clearMemory();
		return OK;
	}
	if (eq(data, "clear")) {
		clearMemory();
		return OK;
	}
	if (eq(data, "./clear-shared-memory.sh")) {
		clearMemory();
		return OK;
	}
	if (eq(data, ":e")) {
		export_csv();
		return OK;
	}
	if (eq(data, "quit")) return OK;
	if (eq(data, "quiet")) quiet=!quiet;

	if (startsWith(data, ":if")) {
		autoIds=false;
		//			if (endsWith(data, "!"))deleteWord("acceptant");
		//			if (!hasWord("acceptant"))
		importFreebase();
		return OK;
	}
	if (startsWith(data, ":iw") || startsWith(data, ":wi")) {
		autoIds=false;
		if (endsWith(data, "!")) deleteWord("acceptant");
		if (!hasWord("acceptant")) importWordnet();
		return OK;
	}
	if (startsWith(data, ":iy") || startsWith(data, ":yi")) {
		autoIds=false;
		if (endsWith(data, "!")) deleteWord("yagoGeoEntity");
		importAllYago();
		return OK;
	}
	if (startsWith(data, ":i") || startsWith(data, "import")) {
		autoIds=false;
		string arg=next_word(string(data));
		if (arg.length() > 2) import(arg.c_str());
		else importAll();
		return OK;
	}
	autoIds=true;

	if (contains(data, "limit")) {
		char* newdata=(char*) malloc(1000);
		sscanf(data, "%s limit %d", newdata, &resultLimit);
//		if(!)
//		sscanf(data, "limit %d %s", &resultLimit, newdata);
		strcpy((char*) data, newdata);
		p(resultLimit);
	}
	if (eq(data, "load") || eq(data, ":l")) {
		load(false);
		return OK;
	}
	if (eq(data, ":ca")) collectAbstracts();
	if (eq(data, "load_files") || eq(data, ":lf") || eq(data, ":l!") || eq(data, "load!")) {
		load(true);
		return OK;
	}
	if (eq(data, "save")) {
		save();
		return OK;
	}
	if (eq(data, ":ha")) {
		Context* c=currentContext();
		c->nodeCount-=1000; //hack!
		//		maxNodes += 1000;
		return OK;
	}
	if (eq(data, ":s")) {
		save();
		return OK;
	}
	if (eq(data, ":c")) {
		showContext(currentContext());
		return OK;
	}
	if (eq(data, ":t") || eq(data, "test") || eq(data, "test ") || eq(data, "tests")) {
		exitOnFailure=false;
		tests();
		return OK;
	}
	if (eq(data, "debug") || eq(data, "debug on") || eq(data, "debug 1")) {
		debug=true;
		return OK;
	}
	if (eq(data, "debug off") || eq(data, "no debug") || eq(data, "debug 0")) {
		debug=true;
		return OK;
	}
	if (startsWith(data, ":d ") || startsWith(data, "delete ") || startsWith(data, "del ") || startsWith(data, "remove ")) {
		string d=next_word(data);
		const char* what=d.data();
		printf("deleting %s\n", what);
		deleteWord(what);
	}

	if (startsWith(data, "path") || startsWith(data, ":p")) {
		Node* from=getAbstract(args.at(1));
		Node* to=getAbstract(args.at(2));
		return shortestPath(from, to);
	}

	if (args.size() > 1 && startsWith(data, "has")) {
		Node* from=getAbstract(args.at(1));
		Node* to=getAbstract(args.at(2));
		return memberPath(from, to);
	}
	if (args.size() > 1 && startsWith(data, "is")) {
		Node* from=getAbstract(args.at(1));
		Node* to=getAbstract(args.at(2));
		return parentPath(from, to);
	}
	if (startsWith(data, "select ")) return query(data);
	if (startsWith(data, "all ")) {
     return showNodes(query(data));
	}
	if (startsWith(data, "these ")) return query(data);
	if (contains(data, "that ")) return query(data);
	if (contains(data, "who")) // who loves jule
		return query(data);

	if (eq(args[0], "the") || eq(args[0], "my")) {
		N da=getThe(data + 4, More);
		show(da);
		return nodeVectorWrap(da);
	}

	if (startsWith(data, "label ")||startsWith(data, "rename ")) {
		N n=getThe(args[1]);
		setLabel(n, next_word(next_word(data)).data());
		return nodeVectorWrap(n);
	}
	if (startsWith(data, "an ")) return query(data);
	if (startsWith(data, "a ")) return query(data);
	if (startsWith(data, "any ")) return query(data);

	//        if(startsWith(data,"is ")){  check_statement(data);return OK;}
	//        if(startsWith(data,"does ")){  check_statement(data);return OK;}
	if (contains(data, " in ")) return query(data);

	//	if (args.size() > 2 && eq(args[1], "of")) {
	//		clearAlgorithmHash();
	//		Node* property = getThe(args[0]);
	//		Node* propertyA = getAbstract(args[0]);
	//		Node* node = getThe(args[2]);
	//		Node* nodeA = getAbstract(args[2]);
	//		Node* found;
	//		if (found == 0)found = has(nodeA, propertyA);
	//		if (found == 0)found = has(Any, propertyA, nodeA);
	////		if (found == 0)found = has(nodeA, propertyA, Any, true, true, true, true);
	//		if (checkNode(found)) {
	//			show(found);
	//			pf("ANSWER: %s\n", found->name);
	//		}
	//		return nodeVectorWrap(found);
	//	}

//	if (startsWith(q, "m/")) {
	if (startsWith(data, "<m.") || startsWith(data, "<g.")) {
		p("<m.0c21rgr> needs showered memory or boost ");
		Node* n=getFreebaseEntity((char*) data);
		show(n);    //<g.11vjx36lj>
		return OK;
	}
	if ((args.size() > 2 && eq(args[1], "of")) || contains(data, " of ") || contains(data, " by ") || (contains(data, "."))) { // && !contains(data, " "))) {
		return parseProperties(data); // ownerpath
	}
	if (eq(data, "server") || eq(data, "daemon") || eq(data, "demon")) {
		printf("starting server\n");
		start_server();
		return OK;
	}

	if (contains(data, " with ")) return query(data);
	if (contains(data, " where ")) return query(data);
	if (contains(data, " from ")) return query(data);
	if (contains(data, " whose ")) return query(data);
	if (contains(data, " which ")) return query(data);
	if (contains(data, " without ")) return query(data);
	if (contains(data, "ing ")) return query(data);

	if (contains(data, " that ")) return query(data);
	if (contains(data, "who ")) return query(data);

	if (args.size() >= 3 && eq(args[1], "to")) {
		Node* from=getAbstract(args.at(0));
		Node* to=getAbstract(args.at(2));
		return shortestPath(from, to);
		//		NodeVector all = memberPath(from, to);
		//		if(all==EMPTY)parentPath(from,to);
		//		if(all==EMPTY)shortestPath(from,to);
	}

	if (data[0] == '!' || (args.size() >= 3 && eq(args[1], "is"))) return nodeVectorWrap(reify(learn(data)));

	data=replace((char*) data, ' ', '_');

	int i=atoi(data);
	if (startsWith(data, "$")) showStatement(getStatement(atoi(data + 1)));
	if (endsWith(data, "$")) showStatement(getStatement(i));

	Node* a=dissectWord(get(data), true);
	show(a);
	if (i == 0) showNodes(instanceFilter(a), true);
	//        findWord(currentContext()->id, data);
//    if(isAbstract(a))
	if (i == 0){
		NV all= instanceFilter(a);
		if(all.size()>0)
			return all;
	}
	return nodeVectorWrap(a);
//

	//    return nodeVectorWrap(get(data));
	//	return OK;
}

void console() {
	quiet=false;
	printf("\nNetbase C++ Version z.a\n");
	char* data=(char*) malloc(1000);
#ifdef signal
	setjmp(try_context); //recovery point
#endif
	while (true) {
		//		clearAlgorithmHash();
		getline(data);
		parse(data);
	}
}
