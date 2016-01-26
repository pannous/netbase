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
#include <readline/history.h>
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
	ps(":import :i [<file>]");
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
	ps("Type words, ids, or queries:");
	//    ps("all animals that have feathers");
	ps("all animals with feathers");
	ps("select * from dogs where terrier");
	ps("all city with countrycode=ZW");
	ps("Population of Gehren");
	ps("opposite of bad");
    ps("learn Germany has Gehren");
    ps("learn 1001 1003 2442 (via ids)");
    ps("update Stadt.Gemeindeart set typ=244797");
//    ps("delete from Stadt.Gemeindeart where typ=244797");
    ps("");
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
//	free(property);
	return found;
}



void console() {
	quiet=false;
	if(germanLabels)printf("\nDeutsch");
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


NodeVector parse(const char* data) {
	if (eq(data, null)) return OK;
	if (!isprint(data[0])) // ??
		return OK;
	if (eq(data, "")) {
		return OK;
	}

	data=fixQuotesAndTrim(editable(data));
	if(data[0]=='!')((char*)data)[0]=':';// norm!
	if(data[0]=='Q' && data[1]<='9')data++;//Q1325845
	if(data[0]==':')appendFile("commands.log",data);
	else appendFile("query.log", data);

	vector<char*> args=splitString(data, " "); // WITH 0==cmd!!!
	clearAlgorithmHash(true); //  maybe messed up
    
	//		scanf ( "%s", data );
	if (eq(data, ":exit")) return OK;
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
	if (eq(data, ":q")) exit(1);
    
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
	if (eq(data, ":il")){
		import("labels");// import/labels.csv
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
	if (startsWith(data, ":i") || startsWith(data, ":import")) {
		autoIds=false;
		string arg=next_word(string(data));
		if (arg.length() > 2) import(arg.c_str());
		else importAll();
		return OK;
	}
	autoIds=true;
	if (contains(data, "lookup")||contains(data, ":lookup")) {
		char* limit=(char*)strstr(data,"lookup");
		sscanf(limit, "lookup %d", &lookupLimit);
		pf("LOOKUP LIMIT SET TO %d\n",lookupLimit);
		defaultLookupLimit=lookupLimit;
		if(limit>data) *(limit-1)=0;
		*limit=0;
		if(strlen(data)<2)return OK;
	}
	if (contains(data, "limit")||contains(data, ":limit")) {
		char* limit=(char*)strstr(data,"limit");
		sscanf(limit, "limit %d", &resultLimit);
		pf("LIMIT SET TO %d\n",resultLimit);// quiet bug
		lookupLimit=resultLimit*10;//todo
		if(limit>data) *(limit-1)=0;
		*limit=0;
//		if(strlen(data)<2)return OK;
	}
	if (eq(data, ":load")) { // || eq(data, ":l") learn?
		load(false);
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
		printf("deleting %s\n", what);
		deleteWord(what);
        return OK;
	}
    
    if(contains(data,":english")||contains(data,":en")){germanLabels=false;initRelations();return OK;}
    if(contains(data,":german")||contains(data,":de")){germanLabels=true;initRelations();return OK;}
    
    if (eq(args[0], "show")) {// not ":show" here!!
		N da=getAbstract(data + 5);
        show(da,true);
		return nodeVectorWrap(da);
	}
    //    Ch��teau
	if (eq(args[0], ":all"))
		return *findAllWords(data + 5);
	if (eq(args[0], ":matches"))
		return *findWords(wordnet, data + 13, false,true);
	if (eq(args[0], ":find"))
		return *findWords(wordnet, data + 6, false);
    
	if (startsWith(data, ":merge ")) {
        int target,node=0;
		sscanf(data, ":merge %d %d", &target,&node);
        Node* targetNode=get(target);
        if(node)
            return showNodes(nodeVectorWrap(mergeNode(targetNode,get(node))));
        else if(target)
            return showNodes(nodeVectorWrap(mergeAll(targetNode->name)));
        else
            return showNodes(nodeVectorWrap(mergeAll(args[1])));// merge <string>
    }
    
	if (startsWith(data, ":path ") || startsWith(data, ":p ")) {
		Node* from=getAbstract(args.at(1));
		Node* to=getAbstract(args.at(2));
		return shortestPath(from, to);
	}
    
	if (startsWith(data, ":script ")) {
		char* file=args.at(1);
		return runScript(file);
	}
	if (startsWith(data, ":rh")) {
		return runScript("commands.log");
	}


	if (args.size() > 1 && (startsWith(data, ":has ")||startsWith(data, "has "))) {
		Node* from=getAbstract(args.at(1));
		Node* to=getAbstract(args.at(2));
		return memberPath(from, to);
	}
	if (args.size() > 1 && (startsWith(data, ":is ")||startsWith(data, "is "))) {
		Node* from=getAbstract(args.at(1));
		Node* to=getAbstract(args.at(2));
		return parentPath(from, to);
	}
    
    if (args.size() ==3 && contains(data,"exclude ")){// no ":" here!
        autoIds=true;
        Node *node=getAbstract(args[0]);
        if(eq(args[0], "exclude"))node=getAbstract(args[1]);
        addStatement(node,getAbstract("exclude"),getAbstract(args[2]));
        return nodeVectorWrap(getAbstract(args[0]));
    }
    if (startsWith(data, "exclude ")){// no ":" here!
        autoIds=true;
        addStatement(get("excluded"), get("exclude"),getAbstract(data+8));
        return nodeVectorWrap(get("excluded"));
    }
        if (args.size() >=3 && contains(data,"include ")){// no ":" here!
            autoIds=true;
            Node *node=getAbstract(args[0]);
            Node *to_include=getAbstract(args[2]);// next //join(sublist(args,2)," "));
            if(eq(args[0], "include"))node=getAbstract(args[1]);
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

	if (args.size() >= 2 && (eq(args[0], "handle")||eq(args[0], ":handle"))){
		string what=next_word(data);
		handle(what.c_str());
		return OK;
	}

	if (startsWith(data, "select ")) return query(data);
	if (startsWith(data, "all ")||startsWith(data, "show ")||startsWith(data, ":all ")||startsWith(data, ":show ")) {
		lookupLimit=100;
		bool allowInverse=true;// ONLY inverse of superclass!!
		NodeVector all=findProperties(next_word(data).c_str(),"superclass",allowInverse);
		//Dusty the Klepto Kitty Organism type ^ - + -- -! 	Cat ^
		//Big the Cat 	x Species ^ - + -- -!
		if(all.size()<resultLimit){
			NodeVector more=query(data);
			mergeVectors(&all,more);
		}
		return showNodes(all,true);
	}
	if (startsWith(data, "these ")) return query(data);
	if (contains(data, "that ")) return query(data);
	if (contains(data, "who ")) // who loves jule
		return query(data);
    
	if (eq(args[0], "the") || eq(args[0], "my")) {
		N da=getThe(next_word(data).data(), More);
		show(da);
		return nodeVectorWrap(da);
	}
	if (eq(args[0], "a") || eq(args[0], "abstract")) {
		N da=getAbstract(next_word(data).data());
		show(da);
		return nodeVectorWrap(da);
	}

	if (startsWith(data, ":printlabels")){
		printlabels();
	}

	if (startsWith(data, ":label ") || startsWith(data, ":l ") || startsWith(data, ":rename ")) {
		const char* what=next_word(data).data();
		appendFile("import/labels.csv",what);
		char* wordOrId=args[1];
		const char* label=next_word(what).data();
		N n=getThe(wordOrId);
		setLabel(n, label);
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
		Node* n=getFreebaseEntity((char*) data,true);
		show(n);    //<g.11vjx36lj>
		return OK;
	}
    
	

//   update Stadt.Gemeindeart set type=244797 limit 100000
	if (startsWith(data, "update")||startsWith(data, ":update")){
        update(data);
    }
    
	if (eq(data, "server") ||eq(data, ":server") || 
		eq(data, ":daemon") || eq(data, ":demon")|| 
		eq(data, "daemon") || eq(data, "demon")) {
		printf("starting server\n");
		start_server();
		return OK;
	}
    if(eq(data,"testing")){testing=true;autoIds=false;clearMemory();return OK;}
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
		else return nodeVectorWrap(learn(data));
	}

	if (args.size() >= 3 && eq(args[1], "to")) {
		Node* from=getAbstract(args.at(0));
		Node* to=getAbstract(args.at(2));
		return shortestPath(from, to);
		//		NodeVector all = memberPath(from, to);
		//		if(all==EMPTY)parentPath(from,to);
		//		if(all==EMPTY)shortestPath(from,to);
	}
    
	
	if (args.size() >= 4 && (eq(args[0], "learn")||eq(args[0], ":learn")||eq(args[0], ":l")||eq(args[0], ":!"))){
		string what=next_word(data);
		NodeVector nv=nodeVectorWrap(learn(what)->Subject());
		FILE *fp= fopen((data_path+"/facts.ssv").data(), "a");
		if(!fp){p("NO such file facts.ssv!"); return OK;}
		fprintf(fp,"%s\n",what.data());
		fclose(fp);
		return nv;
	}
	if (args.size() >= 3 && eq(args[1], "is"))
        return nodeVectorWrap(learn(data)->Subject());
    
	data=replace((char*) data, ' ', '_');
    
	int i=atoi(data);
	if(data[0]=='P' && data[1]<='9')return nodeVectorWrap(get(-atoi(++data)-10000));// P106 -> -10106

	if (startsWith(data, "$")) showStatement(getStatement(atoi(data + 1)));
	if (endsWith(data, "$")) showStatement(getStatement(i));
//	if(autoIds && i)return nodeVectorWrap(get(i));
	if(i==0 && !hasWord(data))return OK;// don't create / dissect here!

	Node* a=get(data);
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


extern "C"  Node** execute(const char* data,int* out){
//    quiet=true;
    NodeVector result=parse(data);
    int hits=(int)result.size();
//    printf(">>>>>>>>>>> %p\n",out);
    if(out) *out=hits;
    Node** results=(Node** ) malloc((1+hits)*nodeSize);
    for (int i=0; i< hits; i++) {
//        results[i]=result[i];
//        pf("%d %s | ",result[i]->id,result[i]->name);
//        pf(" %ld\n",(long)(void*)result[i]);
        //        memccpy(&results[i],result[i],0,sizeof(Node));
    }
    flush();
    return results;
}


