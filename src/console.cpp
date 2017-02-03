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

#define USE_READLINE
// compile with -lreadline !
#ifdef USE_READLINE
//ai libreadline-dev
#include <readline/history.h> // libreadline-dev
#include <readline/readline.h>
//#include <readline.h>
//#include <history.h>
#endif

using namespace std;
NodeVector OK;
// static struct termios stored_settings;

void showHelpMessage() {
    ps("");
	ps("AVAILABLE COMMANDS:");
	ps("help :h or ?");
	ps(":load :l [force]\tfetch the graph from disk or mem");
	//	ps("load_files :lf");
	ps(":import :i [<file>|dbpedia|wordnet|images|labels|...]");
	ps(":export :e (all to csv)");
	//    ps("print :p");
	ps(":delete :d <node|statement|id|name>");
	ps(":save :s or :w");
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
	ps("subclasses of human");
	ps("entities of human limit 1000");
	//    ps("all animals that have feathers");
	ps(":all animals with feathers");
//	ps("select * from dogs where terrier");
	ps("all city with countrycode=ZW");
	ps("select longitude,latitude from city where Population>10000000");
	ps("Population of Gehren");
	ps("opposite of bad");
	ps(":seo loud-as");
	ps(":entities loud-as");
	ps("");
	ps("Modify data:");
//	ps(":set Gehren.Population to 244797");
//		ps(":update Gehren.Population set current=244797");
	ps(":update Gehren set Population=244797");
	//    ps("delete from Stadt.Gemeindeart where typ=244797");
	ps(":delete blah (entity)");
	ps(":delete 1234 (entity-id)");
	ps(":delete $1234 (statement-id)");
	ps(":learn Gehren ist Ort");
	ps(":learn 1001 5 2442 (via ids)");
	//	ps("update city set type=town where population<10000");
}

bool file_read_done=false;

void getline(char *buf) {
	if (buf == 0) return; // end
	int MAXLENGTH=10000;
	const char* PROMPT="netbase> ";
#ifdef RL_READLINE_VERSION // USE_READLINE
	if (!file_read_done) file_read_done=1 + read_history(0);
	char *tmp=readline(PROMPT);
	if (tmp == 0 || strlen(tmp) == 0) {
		return;
	}
//	tmp=fixQuotesAndTrim(tmp);// LATER!
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
	free(property);
	return found;
}



void console() {
	quiet=false;
	if(germanLabels)printf("\nDeutsch");
	printf("\nNetbase C++ Version 1.5.3.2\n");

	char* data=(char*) malloc(10000);
#ifdef signal
	setjmp(try_context); //recovery point
#endif
	while (true) {
		//		clearAlgorithmHash();
		getline(data);
		bool _autoIds=autoIds;
		parse(data,/*safeMode=*/false);// safeMode only for web access
		autoIds=_autoIds;
	}
}

NodeVector runScript(const char* file){
	FILE *fp= fopen(file,"r");
	if(fp==0){return OK;}
	char line[1000];
	NodeVector last;
	while (fgets(line, sizeof(line), fp) != NULL) {
		if(startsWith(line, ":i"))continue;// don't import here!
		if(startsWith(line, ":s"))continue;// don't
		if(startsWith(line, ":rh"))continue;// don't loop
	last=parse(line);
}
	fclose(fp);
	return last;
}

// CENTRAL CONSOLE INSTRUCTION PARSING
NodeVector parse(const char* data0,bool safeMode/*true*/) {
	if (eq(data0, null)) return OK;
	if (!isprint(data0[0])) // chinese \0a etc
		return OK;
	if (eq(data0, "")) {
		return OK;
	}
	context=getContext(current_context);
	char* data=fixQuotesAndTrim(editable(data0));
	int len=(int)strlen(data);
	if(data[len-1]=='\n'){data[len-1]=0;len--;}
	if(data[len-1]=='\r'){data[len-1]=0;len--;}
	if(data[len-1]=='\''){data[len-1]=0;len--;}
	if(data[0]=='\'')data++;
	if(data[0]=='!')((char*)data)[0]=':';// norm!
	if(data[0]=='Q' && data[1]<='9')data++;//Q1325845
	if(data[0]==':')appendFile("logs/commands.log",data);
	else appendFile("logs/query.log", data);

	vector<string> args=splitString(data, " "); // WITH 0==cmd!!!
//	vector<char*> args=vector<char*>(splitStringC(data, ' ')); // WITH 0==cmd!!!
//	char** args=splitStringC(data, ' '); // WITH 0==cmd!!!

	clearAlgorithmHash(true); //  maybe messed up


	// special server requests first:
	if (contains(data, "limit")||contains(data, ":limit")) {
		char* limit=(char*)strstr(data,"limit");
		sscanf(limit, "limit %d", &resultLimit);
		pf("LIMIT SET TO %d\n",resultLimit);// quiet bug
		lookupLimit=resultLimit*10;//todo
		if(limit>data) *(limit-1)=0;
		*limit=0;
		//		if(len<2)return OK;
	}

	if(startsWith(data, ":entities")||startsWith(data, "entities")||startsWith(data, "EE")||startsWith(data, "ee")||startsWith(data, ":ee")){
		data=next_word(data);
		return show(findEntites(data));
	}
	if(startsWith(data, ":build-seo")){
		buildSeoIndex();
	}

	if(startsWith(data, "seo")||startsWith(data, ":seo")){
		data=next_word(data);
		string seos= generateSEOUrl(data);
		cchar* seo= seos.data();
		pf("SEO form: %s	result(s):\n",seo);
		N n=getSeo(seo);
		if(n){
			if(!n)return OK;// HOW???
			if(n->statementCount<3){
				S s= findStatement(Any, Label, n);
				if(s)n=s->Subject();
			}
			return showWrap(n);
		}else return OK;
	}
	



	//		scanf ( "%s", data );
	if (eq(data, "help") ||eq(data, ":help") || eq(data, "?")) {
		showHelpMessage();
		//        printf("type exit or word");
		return OK;
	}
    
	if (eq(data, ":more")) {
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

	if (eq(data, ":exit")||eq(data, ":quit")||eq(data, ":q"))
		exit(1); // before return!


//	if (eq(data, "q")) return OK;
//	if (eq(data, "x")) return OK;
    if (eq(data, ":console")){
        console();
        return OK;}
	if (eq(data, ":export")) {
		export_csv();
		return OK;
	}
	if (eq(data, ":cl")||eq(data, ":clear")||eq(data, ":clean")||eq(data, ":cleanup")) {
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
	if (eq(data, ":quit")||eq(data, ":exit")) return OK;
	if (eq(data, ":quiet")||eq(data,":!debug")){debug=false; quiet=true;return OK;}// !-> NOT !!
    if (eq(data, ":debug")||eq(data, ":!quiet")){debug=true; quiet=false;return OK;}

	if (startsWith(data, ":if")) {
		importFreebase();
		return OK;
	}
	if (startsWith(data, ":ia")) {
		importAmazon();
		return OK;
	}
	if (eq(data, ":il")){
		import("labels");// import/labels.csv
		return OK;
	}
	if (eq(data, ":it")){
		importTest();
		return OK;
	}
	if (startsWith(data, ":iw") || startsWith(data, ":wi")) {
		if (endsWith(data, "!")) deleteWord("acceptant");
		if (!hasWord("acceptant")) importWordnet();
		return OK;
	}
	if (startsWith(data, ":iwd") || startsWith(data, ":wd")) {
		importWikiData();
		return OK;
	}
	if (startsWith(data, ":iy") || startsWith(data, ":yi")) {
		if (endsWith(data, "!")) deleteWord("yagoGeoEntity");
		importAllYago();
		return OK;
	}
	if (startsWith(data, ":i ") || eq(data, ":i") || startsWith(data, ":import")) {
		autoIds=false;
		string arg=next_word(string(data));
		if (arg.length() > 2) import(arg.c_str());
		else importAll();
		return OK;
	}
	autoIds=true;

	if(startsWith(data, ":image")){
		data=next_word(data);
		return nodeVectorWrap(getThe(getImage(data)));
	}

	if(startsWith(data, ":the")){
		data=next_word(data);
		return nodeVectorWrap(getThe(data));
	}

	if (contains(data, ":lookup")) {
		char* limit=(char*)strstr(data,"lookup");
		sscanf(limit, "lookup %d", &lookupLimit);
		pf("LOOKUP LIMIT SET TO %d\n",lookupLimit);
		defaultLookupLimit=lookupLimit;
		if(limit>data) *(limit-1)=0;
		*limit=0;
		if(len<2)return OK;
	}

	if (eq(data, ":load")) {
		load(true);
		return OK;
	}
	if (eq(data, ":ca")) collectAbstracts();
	if (eq(data, ":ci")) collectInstances();
	if (eq(data, ":load_files") || eq(data, ":lf") || eq(data, ":l!") || eq(data, ":load!")) {
		load(true);
		return OK;
	}
	if (eq(data, ":save")||eq(data, ":s")) {
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
	if (eq(data, ":c")|| eq(data, ":context")) {
		showContext(context);
		return OK;
	}
	if (eq(data, ":t") || eq(data, ":test") || eq(data, ":tests")) {
		exitOnFailure=false;
		testAll();
		return OK;
	}
	if (eq(data, ":tb")||eq(data, ":tbn")) {
		exitOnFailure=false;
		testBrandNewStuff();
		return OK;
	}
	if (eq(data, ":debug") || eq(data, ":debug on") || eq(data, ":debug 1")) {
		debug=true;
		return OK;
	}
	if (eq(data, ":debug off") || eq(data, ":no debug") || eq(data, ":debug 0")) {
		debug=true;
		return OK;
	}
	if (startsWith(data, ":d ") || startsWith(data, ":delete ") ||startsWith(data, ":del ") || startsWith(data, ":remove ")) {
		string d=next_word(data);
		const char* what=d.data();
		deleteWord(what);
        return OK;
	}
    
    if(contains(data,":english")||contains(data,":en")){germanLabels=false;initRelations();return OK;}
    if(contains(data,":german")||contains(data,":de")){germanLabels=true;initRelations();return OK;}
	if(args.size()==0)
		return OK;
    if (eq(args[0], ":show")) {// not ":show" here!!
		N da=getAbstract(data + 5);
        show(da,true);
		return nodeVectorWrap(da);
	}

	if (startsWith(data, ":merge ")) {
        int target,node=0;
		sscanf(data, ":merge %d %d", &target,&node);
        Node* targetNode=get(target);
        if(node)
            return showNodes(nodeVectorWrap(mergeNode(targetNode,get(node))));
        else if(target)
            return showNodes(nodeVectorWrap(mergeAll(targetNode->name)));
        else
            return showNodes(nodeVectorWrap(mergeAll(args[1].c_str())));// merge <string>
    }
    
	if (startsWith(data, ":path ") || startsWith(data, ":p ")) {
		Node* from=getAbstract(args[1]);
		Node* to=getAbstract(args[2]);
		return shortestPath(from, to);
	}
    
	if (startsWith(data, ":script ")) {
		cchar* file=args[1].c_str();
		return runScript(file);
	}
	if (startsWith(data, ":rh")) {
		return runScript("logs/commands.log");
	}


	if (args.size() > 1 && (startsWith(data, ":has "))) {
		Node* from=getAbstract(args.at(1));
		Node* to=getAbstract(args.at(2));
		return memberPath(from, to);
	}
	if (args.size() > 1 && (startsWith(data, ":is "))) {
		Node* from=getAbstract(args.at(1));
		Node* to=getAbstract(args.at(2));
		return parentPath(from, to);
	}
    
    if (args.size() ==3 && contains(data,":exclude ")){// no ":" here!
        autoIds=true;
        Node *node=getAbstract(args[0]);
        if(eq(args[0], "exclude"))node=getAbstract(args[1]);
        addStatement(node,getAbstract("exclude"),getAbstract(args[2]));
        return nodeVectorWrap(getAbstract(args[0]));
    }
    if (startsWith(data, ":exclude ")){// no ":" here!
        autoIds=true;
        addStatement(get("excluded"), get("exclude"),getAbstract(data+9));
        return nodeVectorWrap(get("excluded"));
    }
        if (args.size() >=3 && contains(data,":include ")){// no ":" here!
            autoIds=true;
            Node *node=getAbstract(args[0]);
            Node *to_include=getAbstract(args[2]);// next //join(sublist(args,2)," "));
            if(eq(args[0], ":include"))node=getAbstract(args[1]);
            else to_include=getAbstract(cut_to(data, "include "));
            addStatement(node,getAbstract("include"),to_include);
            N type=getType(node);// auto add to type!!!
            if(type)node=type;
            addStatement(type,getAbstract("include"),to_include);
            return nodeVectorWrap(node);
        }
//  //	if (startsWith(data, "include ")){// globally included? NAH! How? bad
//        addStatement(get("included"), Instance,getAbstract(data+8));
//        return nodeVectorWrap(get("included"));
//    }

	if (eq(args[0], ":handle")){
		if(args.size() >= 2 ){
		string what=next_word(data);
		handle(what.c_str());// like server (debug)
		}else{handle("");}// default page
		return OK;
	}

	if (startsWith(data, ":select ")) return query(data);


	//    Ch��teau
	if (eq(args[0], ":words"))// all // ambiguous!
		return *findAllWords(data + 5);
	if (eq(args[0], ":matches"))// ambiguous!
		return *findWordsByName(wordnet, data + 13, false,true);
	if (eq(args[0], ":find"))// ambiguous!
		return *findWordsByName(wordnet, data + 6, false);

	//		//Dusty the Klepto Kitty Organism type ^ - + -- -! 	Cat ^
	//Big the Cat 	x Species ^ - + -- -!
	if (startsWith(data, ":show ")||// show?? really?
		startsWith(data, ":tree")||startsWith(data, ":subclasses")||startsWith(data, "subclasses")){
		data=next_word(data);
		if(startsWith(data,"of"))data=next_word(data);
		N da=getAbstract(data);
		NS all=findAll(da,subclassFilter);// ok, show!
//		show(all);
		return setToVector(all);
//		lookupLimit=100;
//		bool allowInverse=true;// ONLY inverse of superclass!!
//		NodeVector all=findProperties(next_word(data).c_str(),"superclass",allowInverse);
//		if(all.size()<resultLimit){
//			NodeVector more=query(data);
//			mergeVectors(&all,more);
//		}
//		return showNodes(all,true);
	}

	if(startsWith(data, ":fix")){
		fixCurrent();
		return OK;
	}
	if(startsWith(data, ":build_seo")){
		buildSeoIndex();
		return OK;
	}
	if(startsWith(data, ":replay")){
		replay();
		return OK;
	}


	if(startsWith(data, ":new")){
		data=next_word(data);
		return nodeVectorWrap(add(data));
	}

	if(startsWith(data, ":topics")){
		data=next_word(data);
		autoIds=true;
		return show(getTopics(getThe(data)));
	}
	if(startsWith(data, ":topic")||startsWith(data, ":to")){
		data=next_word(data);
		autoIds=true;
		return showWrap(getTopic(getThe(data)));
	}
	if(startsWith(data, ":type")||startsWith(data, ":kind")){
		data=next_word(data);
		autoIds=true;
		N n=getType(getThe(data));
		if(checkNode(n))p(n->id);
		return showWrap(n);
	}
	if(startsWith(data, ":class")){
		data=next_word(data);
		autoIds=true;
		return showWrap(getClass(get(data)));
	}

	if(startsWith(data, ":abstract")||startsWith(data, ":ab")||startsWith(data, ":a")){
		data=next_word(data);
		if(isInteger(data))data=get(atoi(data))->name;
		return showWrap(getAbstract(data));// ok, create here! VS hasWord
	}

	if (startsWith(data, ":all ")||startsWith(data, ":children")||startsWith(data, ":instances")){//||startsWith(data, "children ")
		N da=getAbstract(next_word(data));
		NS all=findAll(da,instanceFilter);// childFilter
//		show(all);// ok, show!
		return setToVector(all);
	}

	if (startsWith(data, ":printlabels")){
		printlabels();
	}


	if (startsWith(data, ":label ") || startsWith(data, ":l ") || startsWith(data, ":rename")|| startsWith(data, ":name")) {
		const char* what=next_word(data);
		appendFile("import/labels.csv",what);
		cchar* wordOrId=args[1].c_str();
		const char* label=next_word(what).data();
		N n=getThe(wordOrId);
		setLabel(n, label);
		return nodeVectorWrap(n);
	}
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
//	if (startsWith(data, "<m.") || startsWith(data, "<g.")) {
//		p("<m.0c21rgr> needs shared memory or boost ");
//		Node* n=getEntity((char*) data,true);
//		show(n);    //<g.11vjx36lj>
//		return OK;
//	}

	

//   update Stadt.Gemeindeart set type=244797 limit 100000
	if (startsWith(data, ":update")){
        return update(data);
    }

	// eq(data, "server") || only in main() !
	if (startsWith(data, ":server") || startsWith(data, ":daemon") || startsWith(data, ":demon")) {
		printf("starting server\n");
		if(getenv("SERVER_PORT"))SERVER_PORT=atoi(getenv("SERVER_PORT"));
		while((data=next_word(data))&&strlen(data)>0)
			if(atoi(data)>0)SERVER_PORT=atoi(data);
		start_server(SERVER_PORT);
		return OK;
	}
	if(eq(data,":testing")){testing=true;autoIds=false;clearMemory();return OK;}
	if(startsWith(data,":complete")){
		char* word=next_word(data);
		return *findWordsByName(current_context,word,/*first*/false,/*containsWord*/true);
	}


// LEARN
//	if (args.size() >= 3 && eq(args[1], "is"))
//		return nodeVectorWrap(learn(data)->Subject());
//	startsWith(data, "update")||
//	eq(data, "daemon") || eq(data, "demon")

	bool QUESTIONS=true;//false; OK, not EE
	if (startsWith(data, ":select ")||startsWith(data, ":query ")) {// ||&&endsWith(data, "?")
		data=next_word(data);// whoot?
		QUESTIONS=true;
	}
	if(endsWith(data, "?")){
		data[len-1]=0;
		QUESTIONS=true;
	}

	if(QUESTIONS && !safeMode && !startsWith(data0,"'") && data0[0]!='"'){
		if (startsWith(data, "an ")) return query(data);
		if (startsWith(data, "a ")) return query(data);
		if (startsWith(data, "any ")) return query(data);

		//        if(startsWith(data,"is ")){  check_statement(data);return OK;}
		//        if(startsWith(data,"does ")){  check_statement(data);return OK;}
		if (contains(data, " in ")) return query(data);

		if (startsWith(data, "these ")) return query(data);
		if (contains(data, "that ")) return query(data);
		if (contains(data, "who ")) // who loves jule
			return query(data);

		if (eq(args[0], "the") || eq(args[0], "my")) {
			N da=getThe(next_word(data), More);
			show(da);
			return nodeVectorWrap(da);
		}
		if (eq(args[0], "a") || eq(args[0], "abstract")) {
			N da=getAbstract(next_word(data));
			show(da);
			return nodeVectorWrap(da);
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


		if ((args.size() > 2 && eq(args[1], "of")) || contains(data, " of ") || contains(data, " by ") || (contains(data, "."))) {
		// || (contains(data, ":")) && !contains(data, " "))) {
		if(!contains(data, "="))
			return parseProperties(data);
//		else return nodeVectorWrap(learn(string(data)));
	}

	if (args.size() >= 3 && eq(args[1], ":to")) {
		Node* from=getAbstract(args.at(0));
		Node* to=getAbstract(args.at(2));
		return shortestPath(from, to);
		//		NodeVector all = memberPath(from, to);
		//		if(all==EMPTY)parentPath(from,to);
		//		if(all==EMPTY)shortestPath(from,to);
	}
	}

	if (eq(args[0], ":last")){
		p(context->lastNode);
		return nodeVectorWrap(get(context->lastNode));
	}
	if ((eq(args[0], ":learn")||eq(args[0], ":l")||eq(args[0], ":!"))){// eq(args[0], "learn")|| args.size() >= 4 && (
		string what=next_word(data);
		NodeVector nv=nodeVectorWrap(learn(what.data())->Subject());
		FILE *fp= fopen((data_path+"/facts.ssv").data(), "a");
		if(!fp){p("NO such file facts.ssv!"); return OK;}
		fprintf(fp,"%s\n",what.data());
		fclose(fp);
		return nv;
	}
    
	data=replace((char*) data, ' ', '_');
    
	int i=atoi(data);
	if(data[0]=='P' && data[1]<='9')
		return nodeVectorWrap(get(-atoi(++data)-10000));// P106 -> -10106
	if (startsWith(data, ":")){pf("UNKNOWN COMMAND %s\n",data);showHelpMessage();pf("UNKNOWN COMMAND %s\n",data);}
	if (startsWith(data, "$")) showStatement(getStatement(atoi(data + 1)));
	if (endsWith(data, "$")) showStatement(getStatement(i));
//	if(autoIds && i)return nodeVectorWrap(get(i));
	if(i==0 && !hasWord(data))return OK;// don't create / dissect here!

	Node* a=get(data);
	if(!isAbstract(a)){
		if(i == 0 || !hasWord(a->name)){
//		a->kind=abstractId;// not for singletons AMAZON!
		insertAbstractHash(a,true);// fix bug! can't be!?
		}
	}
	dissectWord(a, true);
	show(a);
    //	if (i == 0) showNodes(instanceFilter(a), true);
	//        findWord(context->id, data);
    if(isAbstract(a)&&i == 0) {
		lookupLimit=resultLimit;
		NV all=instanceFilter(a);
//		all.insert(a);// include abstract!
		all.push_back(a);// include abstract!
//		sortNodes(all);
		return all;
	}
	return nodeVectorWrap(a);
}

extern "C" Node** execute(const char* data,int* out){
    NodeVector result=parse(data,true);
    int hits=(int)result.size();
	if(out) *out=hits;
    Node** results=(Node** ) malloc((1+hits)*nodeSize);
    for (int i=0; i< hits; i++) {
        results[i]=result[i];
	}
    return results;
}
