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
	ps("save :s or :w");
	ps("load :l [force]\tfetch the graph from disk or mem");
	//	ps("load_files :lf");
	ps("import :i [<file>]");
	ps("export :e (all to csv)");
	//    ps("print :p");
	ps("delete :d <node|statement|id|name>");
	//	ps("save and exit :x");
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
}

//bool parse(string* data) {
static char* lastCommand;

Node *parseProperty(const char *data) {
	char *thing = (char *) malloc(1000);
	char *property = (char *) malloc(1000);
	if (contains(data, " of "))
		sscanf(data, "%s of %s", property, thing);
	else {
		//			sscanf(data,"%s.%s",thing,property);
		char** splat = splitStringC(data, '.');
		thing = splat[0];
		property = splat[1];
	}
	if (!property) {
		char** splat = splitStringC(data, ' ');
		thing = splat[0];
		property = splat[2];
	}
	pf("does %s have a %s?\n", thing, property);
	Node* found = has(getThe(thing), getAbstract(property));
	if (found == 0)
		found = has(getAbstract(thing), getAbstract(property));
	if (checkNode(found)) {
		show(found);
		pf("YES: %s\n", found->name);
	}
	return found;
}

bool parse(const char* data) {
	if (eq(data, null)) {
		return true;
	}
	if (eq(data, "")) {
		return true;
	}
	clearAlgorithmHash(true); //  maybe messed up
	string arg = next(string(data));
	vector<char*> args = splitString(data, " "); // WITH 0==cmd!!!

	//		scanf ( "%s", data );
	if (eq(data, "exit"))return false;
	if (eq(data, "help") || eq(data, "?")) {
		showHelp();
		//        printf("type exit or word");
		return true;
	}

	if (eq(data, "more")) {
		resultLimit = resultLimit * 2;
		if (lastCommand)
			return parse(lastCommand);
		else return true;
	}

	if (eq(data, ":x")) {
		save();
		exit(1);
		return false;
	}
	lastCommand = clone(data);
	if (eq(data, ":w")) return save();
	if (eq(data, ":q")) {
		exit(1);
		return false;
	}
	if (eq(data, "q"))return false;
	if (eq(data, "x"))return false;
	if (eq(data, "export"))return export_csv();
	if (eq(data, ":cl"))return clearMemory();
	if (eq(data, "clear"))return clearMemory();
	if (eq(data, "./clear-shared-memory.sh"))return clearMemory();
	if (eq(data, ":e"))return export_csv();
	if (eq(data, "quit"))return false;
	if (eq(data, "quiet"))quiet = !quiet;

	//        if (eq(data, "collect abstracts")) {
	//            collectAbstracts2();
	//            return true;
	//        }
	if (startsWith(data, ":iw") || startsWith(data, ":wi")) {
		if (endsWith(data, "!"))deleteWord("acceptant");
		if (!hasWord("acceptant"))
			importWordnet();
		return true;
	}
	if (startsWith(data, ":iy") || startsWith(data, ":yi")) {
		if (endsWith(data, "!"))deleteWord("yagoGeoEntity");
		import("yago");
		return true;
		//		importYago()
		//		  importAllYago();
	}
	if (startsWith(data, ":i") || startsWith(data, "import")) {
		if (arg.length() > 2) // && arg<0x7fff00000000
			import(arg.c_str());
		else
			importAll();
		return true;
	}
	if (contains(data, " limit"))
		sscanf(data, "%s limit %d", &resultLimit, data);
	if (contains(data, "limit"))
		sscanf(data, "limit %d %s", &resultLimit, data);
	if (eq(data, "load") || eq(data, ":l")) {
		load(false);
		return true;
	}
	if (eq(data, ":ca"))
		collectAbstracts();
	if (eq(data, "load_files") || eq(data, ":lf") || eq(data, ":l!") || eq(data, "load!")) {
		load(true);
		return true;
	}
	if (eq(data, "save")) {
		save();
		return true;
	}
	if (eq(data, ":ha")) {
		Context* c = currentContext();
		c->nodeCount -= 1000; //hack!
		//		maxNodes += 1000;
		return true;
	}
	if (eq(data, ":s")) {
		save();
		return true;
	}
	if (eq(data, ":c")) {
		showContext(currentContext());
		return true;
	}
	if (eq(data, ":t") || eq(data, "test") || eq(data, "test ") || eq(data, "tests")) {
		exitOnFailure = false;
		tests();
		return true;
	}
	if (eq(data, "debug") || eq(data, "debug on") || eq(data, "debug 1")) {
		debug = true;
		return true;
	}
	if (eq(data, "debug off") || eq(data, "no debug") || eq(data, "debug 0")) {
		debug = true;
		return true;
	}
	if (startsWith(data, ":d ") || startsWith(data, "delete ") || startsWith(data, "del ") || startsWith(data, "remove ")) {
		const char* what = next(data).c_str();
		printf("deleting %s\n", what);
		deleteWord(what);
		return true;
	}

	if (startsWith(data, "path") || startsWith(data, ":p")) {
		Node* from = getAbstract(args.at(1));
		Node* to = getAbstract(args.at(2));
		shortestPath(from, to);
		return true;
	}

	if (args.size() > 1 && startsWith(data, "has")) {
		Node* from = getAbstract(args.at(1));
		Node* to = getAbstract(args.at(2));
		memberPath(from, to);
		return true;
	}
	if (args.size() > 1 && startsWith(data, "is")) {
		Node* from = getAbstract(args.at(1));
		Node* to = getAbstract(args.at(2));
		parentPath(from, to);
		return true;
	}
	if (startsWith(data, "select ")) {
		query(data);
		return true;
	}
	if (startsWith(data, "all ")) {
		query(data);
		return true;
	}
	if (startsWith(data, "these ")) {
		query(data);
		return true;
	}
	if (contains(data, "that ")) {
		query(data);
		return true;
	}
	if (contains(data, "who")) {// who loves jule
		query(data);
		return true;
	}

	if (args.size() == 2 && (eq(args[0], "the") || eq(args[0], "my")))
		return show(getThe(args[1]));
	if (startsWith(data, "an ")) {
		query(data);
		return true;
	}
	if (startsWith(data, "a ")) {
		query(data);
		return true;
	}
	if (startsWith(data, "any ")) {
		query(data);
		return true;
	}

	//        if(startsWith(data,"is ")){  check_statement(data);return true;}
	//        if(startsWith(data,"does ")){  check_statement(data);return true;}
	if (contains(data, " in ")) {
		query(data);
		return true;
	}

	if (args.size() > 2 && eq(args[1], "of")) {
		clearAlgorithmHash();
		Node* property = getThe(args[0]);
		Node* propertyA = getAbstract(args[0]);
		Node* node = getThe(args[2]);
		Node* nodeA = getAbstract(args[2]);
		Node* found = has(node, property);
		if (found == 0)found = has(node, property);
		if (found == 0)found = has(nodeA, property);
		if (found == 0)found = has(node, propertyA);
		if (found == 0)found = has(nodeA, propertyA);
		if (found == 0)found = has(Any, property, node);
		if (found == 0)found = has(Any, propertyA, node);
		if (found == 0)found = has(Any, property, nodeA);
		if (found == 0)found = has(Any, propertyA, nodeA);
		if (found == 0)found = has(nodeA, propertyA,Any,true,true,true,true);
		if (checkNode(found)) {
			show(found);
			pf("ANSWER: %s\n", found->name);
		}
		return true;
	}
	if (args.size() > 2 && args[1] == "of" || contains(data, " of ") || (contains(data, ".") && !contains(data, " "))) {
		parseProperty(data);// ownerpath
		return true;
	}


	if (contains(data, " with ")) {
		query(data);
		return true;
	}
	if (contains(data, " where ")) {
		query(data);
		return true;
	}
	if (contains(data, " from ")) {
		query(data);
		return true;
	}
	if (contains(data, " whose ")) {
		query(data);
		return true;
	}
	if (contains(data, " which ")) {
		query(data);
		return true;
	}
	if (contains(data, " without ")) {
		query(data);
		return true;
	}
	if (contains(data, "ing ")) {
		query(data);
		return true;
	}
	if (contains(data, " that ")) {
		query(data);
		return true;
	}
	if (contains(data, "who ")) {
		query(data);
		return true;
	}
	if (!isprint(data[0])) {// ??
		return false;
	}

	if (args.size() == 2) {
		Node* from = getAbstract(args.at(0));
		Node* to = getAbstract(args.at(1));
		shortestPath(from,to);
//		NodeVector all = memberPath(from, to);
		//		if(all==EMPTY)parentPath(from,to);
		//		if(all==EMPTY)shortestPath(from,to);
		return true;
	}
	if (args.size() > 3) {
		//		if (data[0] == '!')
		return learn(data); // SPO
		//		else data=replace(data," ","_");
	}

	//        query(string("all ")+data);// NO!
	//        query(data);// NOO!
	int i = atoi(data);
	if (i > 0) {
		//		if (i < 1000)showContext(i);
		if (endsWith(data, "s") || endsWith(data, "S") || endsWith(data, "$"))
			showStatement(getStatement(i));
		else{
			dissectWord(get(i));
			showNr(currentContext()->id, i);
		}
	} else {
		dissectWord(get(data));
		findWord(currentContext()->id, data);
	}
}

bool file_read_done = false;

void getline(char *buf) {
	int MAXLENGTH = 1000;
	const char* PROMPT = "netbase> ";
#ifdef RL_READLINE_VERSION // USE_READLINE
	if (!file_read_done)file_read_done = 1 + read_history(0);
	char *tmp = readline(PROMPT);
	if (strncmp(tmp, buf, MAXLENGTH) && strlen(tmp) > 0)
		add_history(tmp); // only add new content
	strncpy(buf, tmp, MAXLENGTH);
	buf[MAXLENGTH - 1] = '\0';
	write_history(0);
	//	append_history(1,0);
	//            free(tmp);
#else
	std::cout << PROMPT;
	std::cin.get(buf, MAXLENGTH);
	std::cin.ignore(); // delete CR
#endif
}

void console() {
	Node* n;
	int i;
	quiet = false;
	printf("\nNetbase C++ Version z.a\n");
	char* data = (char*) malloc(1000);
#ifdef signal
	setjmp(try_context); //recovery point
#endif
	while (true) {
		//		clearAlgorithmHash();
		getline(data);
		parse(data);
	}
}

char* serve(const char* data) {
	if (parse(data))return "success"; // stdout.to_s
	else return "NOPE";
}
