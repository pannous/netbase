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
#include "webserver.hpp" // int handle(char* q,int conn) TEST

// sudo  apt-get install libreadline-dev
// g++ -DNO_READLINE or compile with -lreadline  !
#ifndef NO_READLINE
#define USE_READLINE
#include <readline/history.h> // libreadline-dev
#include <readline/readline.h>
#endif

using namespace std;
NodeVector OK;
// static struct termios stored_settings;

void showHelpMessage() {
	ps("");
	ps("AVAILABLE COMMANDS:");
	ps("help :h or ?");
	ps(":load :l [force]\tfetch the graph from disk or mem");
	ps(":save :s or :w \tbinary dump");
	//	ps("load_files :lf");
	ps(":import :i [<file>|dbpedia|wordnet|images|labels|...]");
	ps(":export :e (all to csv)");
	//  ps("print :p");
	ps(":delete :d <node|statement|id|name>");
	//	ps("save and exit :x");
	ps(":server");
	ps(":quit :q");
	ps(":clear :cl");
	ps(":context :info");
	ps("limit <n>");
	ps("");
	ps("Query data: type words, ids, or queries");
	ps("Abraham Lincoln");
	ps("Q5");
	ps("subclasses of human limit 1000");
	//  ps("all animals that have feathers");
	ps(":all animals with feathers");
//	ps("select * from dogs where terrier");
	ps("all city with countrycode=ZW");
	ps("select longitude,latitude from city where Population>10000000");
	ps("Population of Gehren");
	ps("opposite of bad");
	ps(":seo loud-as");
	ps(":entities loud-as");
	ps("");
//	ps(":set Gehren.Population to 244797");
//		ps(":update Gehren.Population set current=244797");
	ps(":update Gehren set Population=244797");
	//  ps("delete from Stadt.Gemeindeart where typ=244797");
	ps(":delete xyz (entity/id/$statement)");
	ps(":learn Gehren is Ort");
	ps(":learn 1001 5 2442 (via ids)");
	//	ps("update city set type=town where population<10000");
}

bool file_read_done = false;

void getline(char *buf) {
	if (buf == 0) return; // end
	int MAXLENGTH = 10000;
	const char *PROMPT = "netbase> ";
#ifdef USE_READLINE
	if (!file_read_done) file_read_done = 1 + read_history(".netbase_history");
	char *tmp = readline(PROMPT);
	if (tmp == 0 or strlen(tmp) == 0) {
		return;
	}
//	tmp=fixQuotesAndTrim(tmp);// LATER!
	if (strncmp(tmp, buf, MAXLENGTH) and strlen(tmp) > 0) add_history(tmp); // only add new content
	strncpy(buf, tmp, MAXLENGTH);
	buf[MAXLENGTH - 1] = '\0';
	write_history(".netbase_history");
	//	append_history(1,0);
	//      free(tmp);
#else
	std::cout << PROMPT;
	std::cin.get(buf, MAXLENGTH);
	std::cin.ignore(); // delete CR
#endif
}

//bool parse(string* data) {
static char *lastCommand;

NodeSet getPredicates(Node *n) {
	NodeSet predicates;
	Statement *s = 0;
	while ((s = nextStatement(n->id, s))) {
		predicates.insert(s->Predicate());
	}
	return predicates;
}

Node *parseProperty(const char *data, bool deepSearch) {
	newQuery();
	char *thing = (char *) malloc(1000);
	char *property = (char *) malloc(1000);
	if (contains(data, " of ")) sscanf(data, "%s of %s", property, thing);
	if (contains(data, " by ")) sscanf(data, "%s by %s", property, thing);
	else {
		//			sscanf(data,"%s.%s",thing,property);
		char **splat = splitStringC(data, '.');
		thing = splat[0];
		property = splat[1];
	}
	if (!property) {
		char **splat = splitStringC(data, ' ');
		thing = splat[0];
		property = splat[2];
	}
//	if(eq(property,"predicates"))return wrap(getPredicates(getThe(thing));
	pf("does %s have a %s?\n", thing, property);
	Node *found = getProperty(getThe(thing), property);
	if (found == 0)
		found = getProperty(getThe(property),thing);// REVERSE (IF WEB??)
	if (found == 0 and deepSearch) found = has(getThe(thing), getAbstract(property));
	if (found == 0 and deepSearch) found = has(getAbstract(thing), getAbstract(property));
//	free(property);
	return found;
}

void console() {
	quiet = false;
	if (germanLabels)printf("\nDeutsch");
	printf("\nNetbase C++ Version %s\n",version);

	char *data = (char *) malloc(10000);
#ifdef signal
	setjmp(try_context); //recovery point
#endif
	while (true) {
		//		clearAlgorithmHash();
		getline(data);
		bool _autoIds = autoIds;
		NodeVector results = parse(data, false, false);// safeMode only for web access
		if (results.size() == 1)show(results[0]);
		else show(results);
		autoIds = _autoIds;
	}
}

NodeVector runScript(const char *file) {
	FILE *fp = fopen(file, "r");
	if (fp == 0) { return OK; }
	char line[1000];
	NodeVector last;
	while (fgets(line, sizeof(line), fp) != NULL) {
		if (startsWith(line, ":i"))continue;// don't import here!
		if (startsWith(line, ":s"))continue;// don't
		if (startsWith(line, ":rh"))continue;// don't loop
		last = parse(line, false, false);
	}
	fclose(fp);
	return last;
}

// CENTRAL CONSOLE INSTRUCTION PARSING
NodeVector parse(const char *data0, bool safeMode, bool info) {
	if (eq(data0, null)) return OK;
	bool forbidAutoIds= false;
	newQuery();
	context = getContext(current_context);
	if (data0[0]=='"' || data0[0]=='\'')
		forbidAutoIds= true;
	char *data = fixQuotesAndTrim(editable(data0));

	int lenge = (int) strlen(data);
	if(lenge==0)return OK;
	if (data[lenge - 1] == '\n') {
		data[lenge - 1] = 0;
		lenge--;
	}
	if (data[lenge - 1] == '\r') {
		data[lenge - 1] = 0;
		lenge--;
	}
	if (data[lenge - 1] == '\'') {
		data[lenge - 1] = 0;
		lenge--;
	}
	if (eq(data0, ""))return OK;
//	if (!isprint(data0[0]) and not umlaut --) // chinese \0a etc
//		return OK;
	if (data[0] == '\'')data++;
	if (data[0] == '!')((char *) data)[0] = ':';// norm!
	if (data[0] == 'Q' and data[0] >='0' and data[1] <= '9')data++;//Q1325845
	if (data[0] == ':')appendFile("logs/commands.log", data);
	else appendFile("logs/query.log", data);

	vector<string> &args = splitString(data, " "); // WITH 0==cmd!!!
//	vector<char*> args=vector<char*>(splitStringC(data, ' ')); // WITH 0==cmd!!!
//	char** args=splitStringC(data, ' '); // WITH 0==cmd!!!

	clearAlgorithmHash(true); // maybe messed up

	// special server requests first:
	if (contains(data, "limit") or contains(data, ":limit", true)) {
		char *limit = (char *) strstr(data, "limit");
		sscanf(limit, "limit %d", &resultLimit);
		pf("LIMIT SET TO %d\n", resultLimit);// quiet bug
		lookupLimit = resultLimit * 10;//todo
		if (limit > data) *(limit - 1) = 0;
		*limit = 0;
		//		if(len<2)return OK;
	}
	if (startsWith(data, ":entities") or startsWith(data, "entities") or startsWith(data, "EE") or
	    startsWith(data, "ee") or startsWith(data, ":ee")) {
		data = next_word(data);
		return show(findEntites(data));
	}

	if (startsWith(data, "qa") or startsWith(data, ":qa")) {
		data = next_word(data);
		return show(findAnswers(data));
	}
	if (startsWith(data, ":build-seo")) {
		buildSeoIndex();
		return OK;
	}
	if (startsWith(data, "seo") or startsWith(data, ":seo")) {
		data = next_word(data);
		string seos = generateSEOUrl(data);
		cchar *seo = seos.data();
		pf("SEO form: %s	result(s):\n", seo);
		N n = getSeo(seo);
		if (n) {
			if (!n)return OK;// HOW???
			if (n->statementCount < 3) {
				S s = findStatement(Any, Label, n);
				if (s)n = s->Subject();
			}
			return wrap(n);
		} else return OK;
	}
	if(info)
		p("Type :help for command list");//showHelpMessage();
	if (eq(data, "help") or eq(data, ":help") or eq(data, "?")) {
		showHelpMessage();
		//    printf("type exit or word");
		return OK;
	}
	if (eq(data, ":more")) {
		resultLimit = resultLimit * 2;
		if (lastCommand) return parse(lastCommand, false, false);
		else return OK;
	}
	if (eq(data, ":x")) {
		save();
		exit(1);
	}
	lastCommand = clone(data);
	if (eq(data, ":w")) save();
	if (eq(data, ":exit") or eq(data, ":quit") or eq(data, ":q"))
		exit(1); // before return!
//	if (eq(data, "q")) return OK;
//	if (eq(data, "x")) return OK;
	if (eq(data, ":console")) {
		console();
		return OK;
	}
	if (eq(data, ":export")) {
		export_csv();
		return OK;
	}
	if (eq(data, ":cl") or eq(data, ":clear") or eq(data, ":clean") or eq(data, ":cleanup")) {
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
	if (eq(data, ":quit") or eq(data, ":exit")) return OK;
	if (eq(data, ":quiet") or eq(data, ":!debug")) {
		debug = false;
		quiet = true;
		return OK;
	}// !-> NOT !!
	if (eq(data, ":debug") or eq(data, ":!quiet")) {
		debug = true;
		quiet = false;
		return OK;
	}
	if (startsWith(data, ":if")) {
		importFreebase();
		return OK;
	}
	if (startsWith(data, ":ia")) {
		importAmazon();
		return OK;
	}
	if (eq(data, ":il")) {
		import("labels");// import/labels.csv
		return OK;
	}
	if (eq(data, ":it")) {
		testImportWiki();
		return OK;
	}
	if (startsWith(data, ":iwd") or startsWith(data, ":wd")) {
		importWikiData();
		return OK;
	}
	if (startsWith(data, ":iw") or startsWith(data, ":wi")) {
		if (endsWith(data, "!")) deleteWord("acceptant");
		if (!hasWord("acceptant")) importWordnet();
		return OK;
	}
	if (startsWith(data, ":iy") or startsWith(data, ":yi")) {
		if (endsWith(data, "!")) deleteWord("yagoGeoEntity");
		importAllYago();
		return OK;
	}
	if (startsWith(data, ":autoids")||startsWith(data, ":autoIds")) {
		p("use Q12134 P123 if needed ");
	}
	if (startsWith(data, ":i ") or eq(data, ":i") or startsWith(data, ":import")) { // import/
		autoIds = false;
		string arg = next_word(string(data));
		if (arg.length() > 2) import(arg.c_str());
		else importAll();
		return OK;
	}
	if (!forbidAutoIds) autoIds = true;
	if (startsWith(data, ":image")) {
		data = next_word(data);
		return wrap(getThe(getImage(data)));
	}
//	if(startsWith(data, ":the")){
//		data=next_word(data);
//		pf("special %s",data);
//		N t=getThe(data);
//		show(t);
//		if(!t){
//			pf("No special %s",data);
//			t=getAbstract(data);
//		}
//		return wrap(t);
//	}
	if (contains(data, ":lookup")) {
		char *limit = (char *) strstr(data, "lookup");
		sscanf(limit, "lookup %d", &lookupLimit);
		pf("LOOKUP LIMIT SET TO %d\n", lookupLimit);
		if (limit > data) *(limit - 1) = 0;
		*limit = 0;
		if (lenge < 2)return OK;
	}

	if (eq(data, ":load")) {
		load(true);
		return OK;
	}
	if (eq(data, ":ca")) {
		collectAbstracts();
		return OK;
	}
	if (eq(data, ":ca!")) {
		collectAbstracts(true);
		return OK;
	}//clear!
	if (eq(data, ":ci")) {
		collectInstances();
		return OK;
	}
	if (eq(data, ":load_files") or eq(data, ":lf") or eq(data, ":l!") or eq(data, ":load!")) {
		load(true);
		return OK;
	}
	if (eq(data, ":save") or eq(data, ":s")) {
		save();
		return OK;
	}
	if (eq(data, ":hack")) {
		collectAbstracts();
//		Context* c=context;
//		c->nodeCount-=1000; //hack!
		//		maxNodes += 1000;
		return OK;
	}
	if (eq(data, ":c") or eq(data, ":context")) {
		showContext(context);
		return OK;
	}
	if (eq(data, ":t") or eq(data, ":test") or eq(data, ":tests")) {
		exitOnFailure = false;
		testAll();
		return OK;
	}
	if (eq(data, ":tb") or eq(data, ":tbn")) {
		exitOnFailure = false;
		testBrandNewStuff();
		return OK;
	}
	if (eq(data, ":debug") or eq(data, ":debug on") or eq(data, ":debug 1")) {
		debug = true;
		return OK;
	}
	if (eq(data, ":debug off") or eq(data, ":no debug") or eq(data, ":debug 0")) {
		debug = true;
		return OK;
	}
	if (startsWith(data, ":d ") or startsWith(data, ":delete ") or startsWith(data, ":del") or startsWith(data, ":remove ")) {
		bool completely = contains(data, "all");
		string d = next_word(data);
		const char *what = d.data();
		if(what[0]=='S')deleteStatement(atoi(++what));
		else deleteWord(what, completely);
		return OK;
	}
	if (contains(data, ":english") or contains(data, ":en")) {
		germanLabels = false;
		initRelations();
		return OK;
	}
	if (contains(data, ":german") or contains(data, ":de")) {
		germanLabels = true;
		initRelations();
		return OK;
	}
	if (args.size() == 0)
		return OK;
	if (eq(args[0], ":show")) {// not ":show" here!!
		N da = getAbstract(data + 5);
		show(da, true);
		return wrap(da);
	}
	if (startsWith(data, ":merge ")) {
		int target, node = 0;
		sscanf(data, ":merge %d %d", &target, &node);
		Node *targetNode = get(target);
		if (node)
			return wrap(mergeNode(targetNode, get(node)));
		else if (target)
			return wrap(mergeAll(targetNode->name));
		else
			return wrap(mergeAll(args[1].c_str()));// merge <string>
	}
	if (startsWith(data, ":path ") or startsWith(data, ":p ")) {
		Node *from = getAbstract(args[1]);
		Node *to = getAbstract(args[2]);
		return shortestPath(from, to);
	}
	if (startsWith(data, ":script ")) {
		cchar *file = args[1].c_str();
		return runScript(file);
	}
	if (startsWith(data, ":rh")) {
		return runScript("logs/commands.log");
	}
	if (args.size() > 1 and (startsWith(data, ":has "))) {
		Node *from = getAbstract(args.at(1));
		Node *to = getAbstract(args.at(2));
		return memberPath(from, to);
	}
	if (args.size() > 1 and (startsWith(data, ":is "))) {
		Node *from = getAbstract(args.at(1));
		Node *to = getAbstract(args.at(2));
		return parentPath(from, to);
	}
	if (args.size() == 3 and contains(data, ":exclude ")) {// no ":" here!
		autoIds = true;
		Node *node = getAbstract(args[0]);
		if (eq(args[0], "exclude"))node = getAbstract(args[1]);
		addStatement(node, getAbstract("exclude"), getAbstract(args[2]));
		return wrap(getAbstract(args[0]));
	}
	if (startsWith(data, ":exclude ")) {// no ":" here!
		autoIds = true;
		addStatement(get("excluded"), get("exclude"), getAbstract(data + 9));
		return wrap(get("excluded"));
	}
	if (args.size() >= 3 and contains(data, ":include ")) {// no ":" here!
		autoIds = true;
		Node *node = getAbstract(args[0]);
		Node *to_include = getAbstract(args[2]);// next //join(sublist(args,2)," "));
		if (eq(args[0], ":include"))node = getAbstract(args[1]);
		else to_include = getAbstract(cut_to(data, "include "));
		addStatement(node, getAbstract("include"), to_include);
		N type = getType(node);// auto add to type!!!
		if (type)node = type;
		addStatement(type, getAbstract("include"), to_include);
		return wrap(node);
	}
// //	if (startsWith(data, "include ")){// globally included? NAH! How? bad
//    addStatement(get("included"), Instance,getAbstract(data+8));
//    return wrap(get("included"));
//  }

	if (eq(args[0], ":handle") or eq(args[0], ":h")) {
		if (args.size() >= 2) {
			string what = next_word(data);
			handle(what.c_str());// like server (debug)
		} else { handle(""); }// default page
		return OK;
	}

	if (startsWith(data, ":select ")) return query(data);

	//  Ch��teau
	if (eq(args[0], ":words"))// all // ambiguous!
		return *findAllWords(data + 5);
	if (eq(args[0], ":matches"))// ambiguous!
		return *findWordsByName(wordnet, data + 13, false, true);
	if (eq(args[0], ":find"))// ambiguous!
		return *findWordsByName(wordnet, data + 6, false);
	if (startsWith(data, ":all ") or startsWith(data, "all/")) {
//		INCLUDE_CLASSES=true;
		N da = getAbstract(next_word(data));
		NS all = findAll(da, instanceFilter);
		return setToVector(all);
	}
	if (startsWith(data, ":predicates ")) {
		N da = getThe(next_word(data));
		NS all = getPredicates(da);
		return setToVector(all);
	}
	if (startsWith(data, ":children") or startsWith(data, ":list") or startsWith(data, ":recurse") or
	    startsWith(data, ":fetch")) {
		N da = getAbstract(next_word(data));
		NS all = findAll(da, childFilter);
		return setToVector(all);
	}
	if (startsWith(data, ":instances")) {
		N da = getAbstract(next_word(data));
		//		if(endsWith(data, "s"))data[-1]=0
		NS all = findAll(da, instanceFilter);// childFilter
		return setToVector(all);
	}
	//		//Dusty the Klepto Kitty Organism type ^ - + -- -! 	Cat ^
	//Big the Cat 	x Species ^ - + -- -!
	if (startsWith(data, ":tree") or startsWith(data, ":subclasses") or startsWith(data, "subclasses")) {
		data = next_word(data);
		if (startsWith(data, "of"))data = next_word(data);
		N da = getAbstract(data);
		NS all = findAll(da, subclassFilter);// ok, show!
//		show(all);
		return setToVector(all);
//		lookupLimit=100;
//		bool allowInverse=true;// ONLY inverse of superclass!!
//		NodeVector all=findProperties(next_word(data).c_str(),"superclass",allowInverse);
//		if(all.size()<resultLimit){
//			NodeVector more=query(data);
//			mergeVectors(&all,more);
//		}
//		return all,true);
	}

	if (startsWith(data, ":fix")) {
		fixCurrent();
		return OK;
	}
	if (startsWith(data, ":build_seo")) {
		buildSeoIndex();
		return OK;
	}
	if (startsWith(data, ":replay")) {
		replay("logs/replay.log");
		return OK;
	}
	if (startsWith(data, ":new")) {
		data = next_word(data);
		return wrap(add(data));
	}

	if (startsWith(data, ":topics")) {
		data = next_word(data);
		autoIds = true;
		return getTopics(getThe(data));
	}
	if (startsWith(data, ":topic") or startsWith(data, ":to")) {
		data = next_word(data);
		autoIds = true;
		return wrap(getTopic(getThe(data, More)));
	}
	if (startsWith(data, ":typ") or startsWith(data, ":kind")) {
		data = next_word(data);
		autoIds = true;
		N n = getType(getThe(data));
		if (checkNode(n))p(n->id);
		return wrap(n);
	}
	if (startsWith(data, ":class")) {
		data = next_word(data);
		autoIds = true;
		return wrap(getClass(getThe(data)));
	}

	if (startsWith(data, ":id")) {
		p(getID(getThe(next_word(data))));
		return OK;
	}

	if (startsWith(data, ":abstract") or startsWith(data, ":ab") or startsWith(data, ":a")) {
		data = next_word(data);
		if (isInteger(data))data = get(atoi(data))->name;
		return wrap(getAbstract(data));// ok, create here! VS hasWord
	}

	if (startsWith(data, ":printlabels")) {
		printlabels();
	}

	if (startsWith(data, ":lable ") or startsWith(data, ":label ") or startsWith(data, ":l ")){
		char *what = next_word(data);
		char *lable = next_word(what);
		replace(what, ' ', 0);
		addStatement(getThe(what), Label, getThe(lable));
		addStatement(getAbstract(what), Label, getAbstract(lable));
		return wrap(getThe(what));
	}

	if (startsWith(data, ":rename") or startsWith(data, ":name") or startsWith(data, ":n")) {
		const char *what = next_word(data);
		appendFile("import/labels.csv", what);
		cchar *wordOrId = args[1].c_str();
		const char *label = next_word(what).data();
		N n = getThe(wordOrId);
		setLabel(n, label);
		return wrap(n);
	}
	//	if (args.size() > 2 and eq(args[1], "of")) {
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
	//			pf("ANSWER: %s\n", found->name);
	//		}
	//		return wrap(found);
	//	}

	//	if (startsWith(q, "m/")) {
//	if (startsWith(data, "<m.") or startsWith(data, "<g.")) {
//		p("<m.0c21rgr> needs shared memory or boost ");
//		Node* n=getEntity((char*) data,true);
//		show(n);  //<g.11vjx36lj>
//		return OK;
//	}



//  update Stadt.Gemeindeart set type=244797 limit 100000
	if (startsWith(data, ":update")) {
		return update(data);
	}

	// eq(data, "server") or only in main() !
	if (startsWith(data, ":server") or startsWith(data, ":daemon") or startsWith(data, ":demon")) {
		printf("starting server\n");
		if (getenv("SERVER_PORT"))SERVER_PORT = atoi(getenv("SERVER_PORT"));
		while ((data = next_word(data)) and strlen(data) > 0)
			if (atoi(data) > 0)SERVER_PORT = atoi(data);
		start_server(SERVER_PORT);
		return OK;
	}
	if (eq(data, ":testing")) {
		testing = true;
		autoIds = false;
		clearMemory();
		return OK;
	}
	if (startsWith(data, ":complete")) {
		char *word = next_word(data);
		return *findWordsByName(current_context, word,/*first*/false,/*containsWord*/true);
	}


// LEARN
//	if (args.size() >= 3 and eq(args[1], "is"))
//		return wrap(learn(data)->Subject());
//	startsWith(data, "update") or 
//	eq(data, "daemon") or eq(data, "demon")

	bool QUESTIONS = true;//false; OK, not EE
	if (startsWith(data, ":select ") or startsWith(data, ":query ")) {// or and endsWith(data, "?")
		data = next_word(data);// whoot?
		QUESTIONS = true;
	}
	if (endsWith(data, "?")) {
		data[lenge - 1] = 0;
		QUESTIONS = true;
	}

	if (eq(args[0], ":norm")) {
		return wrap(normEntity(getThe(next_word(data))));
	}
	if (eq(args[0], "the") or eq(args[0], ":the") or eq(args[0], "my")) {
		autoIds= false;// :the 25474
		N da = getThe(next_word(data), More);
		return wrap(da);
	}
	if (QUESTIONS and !safeMode and !startsWith(data0, "'") and data0[0] != '"') {
		if (startsWith(data, "an ")) return query(data);
		if (startsWith(data, "a ")) return query(data);
		if (startsWith(data, "any ")) return query(data);

		//    if(startsWith(data,"is ")){ check_statement(data);return OK;}
		//    if(startsWith(data,"does ")){ check_statement(data);return OK;}
		if (contains(data, " in ")) return query(data);

		if (startsWith(data, "these ")) return query(data);
		if (contains(data, "that ")) return query(data);
		if (contains(data, "who ")) // who loves jule
			return query(data);

		if (eq(args[0], "a") or eq(args[0], "abstract")) {
			N da = getAbstract(next_word(data));
			return wrap(da);
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



		if (contains(data, ".")) {
			return wrap(parseProperty(data, false));
		}

		if ((args.size() > 2 and eq(args[1], "of")) or contains(data, " of ") or contains(data, " by ") or
		    (contains(data, "."))) {
			return query(data);
			// or (contains(data, ":")) and !contains(data, " "))) {
			if (!contains(data, "="))
				return parseProperties(data);
//		else return wrap(learn(string(data)));
		}

		if (args.size() >= 3 and eq(args[1], ":to")) {
			Node *from = getAbstract(args.at(0));
			Node *to = getAbstract(args.at(2));
			return shortestPath(from, to);
			//		NodeVector all = memberPath(from, to);
			//		if(all==EMPTY)parentPath(from,to);
			//		if(all==EMPTY)shortestPath(from,to);
		}
	}

	if (eq(args[0], ":last")) {
		p(context->lastNode);
        auto last = get(context->lastNode);
        return last!=Error?wrap(last):OK;
	}
	if ((eq(args[0], ":learn") or eq(args[0], ":l") or
	     eq(args[0], ":!"))) {// eq(args[0], "learn") or args.size() >= 4 and (
		string what = next_word(data);
		if (what.empty()) {
			load(/*,8,*/1);
			return OK;
		}
		Statement *learned = learn(what.data());
		if(!learned)
			return OK;// NOT!
		NodeVector nv = wrap(learned->Subject());

		// append to facts log
		FILE *fp = fopen((data_path + "/facts.ssv").data(), "a");
		if (!fp) {
			p("NO such file facts.ssv!");
			return OK;
		}
		fprintf(fp, "%s\n", what.data());
		fclose(fp);
		return nv;
	}
	if (data[lenge - 1] == '=')data[lenge - 1] = 0;
	if (data[lenge - 1] == ',')data[lenge - 1] = 0;
	data = replace(data, ' ', '_');

	if (data[0] == 'P' and data[0] >= '0' and data[1] <= '9'){// and !forbidAutoIds)
		N m=getEntity(data);
		N n=get(-atoi(++data) - propertyOffset);
		if(n and n->id!=0)return wrap(n);
		else return wrap(m);// P106 -> -10106
	}
	if (startsWith(data, ":")) {
		pf("UNKNOWN COMMAND %s\n", data)
		//showHelpMessage()
	}
	int i = atoi(data);
	if (startsWith(data, "$")) showStatement(getStatement(atoi(data + 1)));
	if (endsWith(data, "$")) showStatement(getStatement(i));
//	if(autoIds and i)return wrap(get(i));
	if ((i == 0 or forbidAutoIds) and !hasWord(data))
		return OK;// don't create / dissect here!
	Node *a = get(data);
//	if (!isAbstract(a)) {
//		if (i == 0 or !hasWord(a->name)) {
//			//		a->kind=abstractId;// not for singletons AMAZON!
//			insertAbstractHash(a, true);// fix bug! can't be!?
//		} else
//			addStatement(getAbstract(a), Instance, a, true, true);
//	}
	dissectWord(a, true);  //	if (i == 0) instanceFilter(a), true);
	//    findWord(context->id, data);
	if (isAbstract(a) and i == 0) {
		lookupLimit = resultLimit;
		N the = getThe(a,0, false);// DONT create!
		show(the);
		NV all = instanceFilter(a);
//		all.push_back(the);
		all.push_back(a);// include abstract!
//		sortNodes(all);
		showNodes(all, true);
		return all;
	}
	return wrap(a);
}

extern "C" Node **execute(const char *data, int *out) {
	NodeVector result = parse(data, true, false);
	int hits = (int) result.size();
	if (out) *out = hits;
	Node **results = (Node **) malloc((1 + hits) * nodeSize);
	for (int i = 0; i < hits; i++) {
		results[i] = result[i];
	}
	return results;
}
