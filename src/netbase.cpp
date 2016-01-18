#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <queue>

//#  -arch i686 # -arch x86-64 -Xarch-x86-64 # x86_64-apple-darwin
//g++ -g -w src/import.cpp src/netbase.cpp src/tests.cpp src/util.cpp /usr/lib/libsqlite3.dylib -o netbase  && ./netbase $@

// gdb   catch throw
// gdb   bt  // back trace

#include <stdio.h> // itoa
#include <stdlib.h>// itoa
// exit, malloc:
#include <cstdlib>
#include <string.h> // strcmp

#include "util.hpp"
//#include "relations.hpp"
#include "util.hpp"
#include "tests.hpp"
#include "import.hpp"
#include "export.hpp"
#include "console.hpp"
#include "md5.h"
#include "relations.hpp"
#include "webserver.hpp"
#include "init.hpp"
#include "query.hpp"
#include "netbase.hpp"
//#include "tests.cpp"
//#define assert(cond) ((cond)?(void)(0): (void)(fprintf (stderr,"FAILED: %s, file %s, line %d \n",#cond,__FILE__,__LINE__), (void)abort()))

using namespace std;

int nameBatch=100;
Context* context_root=0; // Base of shared memory after attached
Node* node_root=0;
Statement* statement_root=0;
char* name_root=0;
Node* abstract_root=0;
int* freebaseKey_root=0;
int defaultLookupLimit = 1000;
int lookupLimit = 1000;// set per query :( todo : param! todo: filter while iterating 1000000 cities!!

//Node** keyhash_root=0;

#ifdef USE_SEMANTICS
bool useSemantics=true;
#else
bool useSemantics=false;
#endif

bool doDissectAbstracts=useSemantics;
bool storeTypeExplicitly=true;
bool exitOnFailure=true;
bool autoIds=false;
bool testing=false;// ;true

#ifdef __NETBASE_DEBUG__
bool debug=true;// false
#else
bool debug=true;// false
#endif


bool showAbstract=false;

int maxRecursions=7;
int runs=0;
Context* contexts; //[maxContexts];// extern
std::string path=""; // extern
string data_path="";
string import_path="./import/";

//extern "C" inline
Context* currentContext() {
	return &contexts[current_context];
}

int badCount;
int current_context=wordnet;

//map<string, Node*> abstracts;
Ahash* abstracts; // Node name hash
Ahash* extrahash; // for hashes that are not unique, increasing

//map<const char*,Node*> abstracts;
map<int, int> wn_map;
//map<int, int> wn_map2;

map<Node*, bool> yetvisited;
map<double, short> yetvisitedIsA;
bool useYetvisitedIsA=false; // BROKEN!! true; // false; EXPENSIVE!!!

//static map < Node*, bool> yetvisited;
//static map <double, short> yetvisitedIsA;
//static map < Node*, short> yetvisitedIsA;
//static map <double, bool> yetvisitedIsA;

// int search;// use in external maps!
// int subjectContext;//... na! nur in externer DB!

void flush() {
	fflush(stdout);
}

//inline
void bad(){
	badCount++;
}
bool isAbstract(Node* object) {
	return object->kind == abstractId || object->kind == singletonId;
}

bool checkHash(Ahash* ah) {
	if(!debug)return true;
	//    if(pos>maxNodes*2)
	if (ah < abstracts || ah >&abstracts[maxNodes * 2]) { // times 2 because it can be an extraHash outside of abstracts!!
		p("ILLEGAL HASH!");
		//	pi(pos);
		px(ah);
		return false;
	}
	return true;
}

Ahash* insertAbstractHash(Node* a) {
	return insertAbstractHash(wordhash(a->name), a);
}

void debugAhash(int position) {
	Ahash* ah=&abstracts[position];
	if (!checkHash(ah)) return;
	//    if(pos==hash("city"))
	//		p(a->name);
	int i=0;
	while (ah->next) {
        if (i++ > 10) break;
		cchar* n="ERROR";
		if (checkNode(ah->abstract)) n=get(ah->abstract)->name;
		pf("%d | %d | >>%s<< | ", position, i, n);
		if (checkNode(ah->abstract)) show(get(ah->abstract), false);
		else p("XXX");
        if(ah->next<0||ah->next>maxNodes)break;
		ah=&abstracts[ah->next];
	}
	p("////4");
}

Ahash *getAhash(int position){
    if(position<0||position>maxNodes*2)return 0;
    return &abstracts[position];
}
// ./clear-shared-memory.sh After changing anything here!!
//int extrahashNr=0;// LOAD FROM CONTEXT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
map<Ahash*, bool> badAhashReported;// debug
Ahash * insertAbstractHash(int position, Node * a) {
	// DO NOT TOUCH THIS ALGORITHM (unless VERY CAREFULLY!!)
    if(a==0)
        return 0;
	Ahash* ah=getAhash(position);
	if (!checkHash(ah) || !checkNode(a) || a->name[0] == 0) return 0;
	int i=0;
	while (ah&&ah->next) {
		if (i++ > 300&&a->name[1]!=0) {	// allow 65536 One letter nodes
            bad();
            if(badAhashReported[ah])return ah;
            badAhashReported[ah]=true;
			debugAhash(position);
			p("insertAbstractHash FULL!");
			show(a);
            //			return 0;
			return ah;
		}
        //		if (ah->next == ah) { // fixed
        //			debugAhash(position);
        //			p("insertAbstractHash LOOP");
        //			show(a);
        //			return 0;
        //		}
		if (get(ah->abstract) == a)
            return ah; //schon da
        if(eq(get(ah->abstract)->name, a->name, true))
            return ah; // NAME schon da!!
		ah=getAhash(ah->next);
	}

	if (ah->abstract && get(ah->abstract)) { //schon was drin
		if (get(ah->abstract) == a)
			return ah;
        if(eq(get(ah->abstract)->name, a->name, true)){
            //            bool ok=hasWord(a->name);
            //            bool ok2=hasWord(a->name);
            return ah;
        }
        //        ah->next=extrahash++;
        ah->next=currentContext()->extrahashNr++;
        ah=getAhash(ah->next);
	}
	if(!checkHash(ah))return 0;
	ah->abstract=a->id;
	return ah;
}

inline bool eq(Statement* s,Statement* s2){
    if(!s || !s2)return false;
	return (s->subject==s2->subject&&s->predicate==s2->predicate&&s->object==s2->object);
}
inline bool eq(Statement* s,int statementId){
	return eq(s,getStatement(statementId));
}

bool appendLinkedListOfStatements(Statement *add_here, Node* node, int statementNr) {
	//    Statement* statement1=&c->statements[statementNr];
	if (add_here->id() == statementNr){
        //		if(debug)
        //		pf("BUG add_here->id==statementNr %d in %d %s\n", statementNr, node->id, node->name);
		return false;
	}
	if (add_here->Subject() == node) add_here->nextSubjectStatement=statementNr;
	if (add_here->Predicate() == node) add_here->nextPredicateStatement=statementNr;
	if (add_here->Object() == node) add_here->nextObjectStatement=statementNr;
    return true;
}

bool prependLinkedListOfStatements(Statement *to_insert, Node* node, int statementNr) {
	return  appendLinkedListOfStatements(to_insert, node, statementNr); // append old to new
}

//bool insert_at_start=true;// <<< TODO HACK, REMOVE!!

bool addStatementToNode(Node* node, int statementId,bool insert_at_start=false) {
    if(statementId==0){
        p("WARNING statementNr==0");
        return false;
    }
	int n=node->statementCount;
	if (n == 0) { // && ==0
		if (node->firstStatement != 0)
            pf("BUG node->firstStatement!=0 %d %s :%d", node->id, node->name, node->firstStatement);
		node->firstStatement=statementId;
		node->lastStatement=statementId;
	} else {
#ifdef useContext
		Context* context=getContext(node->context);
#else
        Context* context=currentContext();
#endif
		Statement* to_insert=&context->statements[statementId];
		//		if (to_insert->Predicate == Instance && to_insert->Subject == node || to_insert->Predicate == Type && to_insert->Object == node) {
		bool push_back=to_insert->subject!=node->id;
		push_back=push_back|| to_insert->Predicate() == Instance;
		push_back=push_back||(to_insert->Predicate() == Type && to_insert->Object()==node);
		push_back=push_back|| to_insert->Predicate() == node;
		// why is that important? to skip 100000 instances (cities) when accessing abstract properties
		if(insert_at_start)push_back=false;// force
		if (push_back) { // ALL!
			Statement* add_here=&context->statements[node->lastStatement];
			if(eq(add_here,statementId))return false;
			appendLinkedListOfStatements(add_here, node, statementId); // append new to old
			node->lastStatement=statementId;
		} else { // invert sort -> sort again in webserver
			if(eq(to_insert,node->firstStatement))return false;
			prependLinkedListOfStatements(to_insert, node, node->firstStatement); // append old to new
			node->firstStatement=statementId;
		}
	}
	node->statementCount++;
	return true;
}

// ONLY USED IN mergeNode!
bool addStatementToNodeDirect(Node* node, int statementId) {
	int n=node->lastStatement;
	if (n == 0) {
		node->firstStatement=statementId;
		node->lastStatement=statementId;
		node->statementCount++;
		return true;
	} else {
		int statement2Nr=0; // find free empty (S|P|O)statement slot of lastStatement
#ifdef useContext
		Context* context=getContext(node->context);
#else
        Context* context=currentContext();
#endif
		Statement* statement0=&context->statements[node->lastStatement]; // last statement
		Statement* statement1=&context->statements[statementId]; // target
		if (statement0->Subject() == node) {
			statement2Nr=statement0->nextSubjectStatement; // rescue old
			statement0->nextSubjectStatement=statementId;
		}
		if (statement0->Predicate() == node) {
			statement2Nr=statement0->nextPredicateStatement;
			statement0->nextPredicateStatement=statementId;
		}
		if (statement0->Object() == node) {
			statement2Nr=statement0->nextObjectStatement;
			statement0->nextObjectStatement=statementId;
		}
		// squeeze statement1 in between statement0 and statement2Nr
		appendLinkedListOfStatements(statement1, node, statement2Nr); // put target into free slot
		node->lastStatement=statementId;
	}
	node->statementCount++;
	return true;
}

char* statementString(Statement * s) {
	char* name=&currentContext()->nodeNames[currentContext()->currentNameSlot];
	sprintf(name, "(%s %s %s)", s->Subject()->name, s->Predicate()->name, s->Object()->name);
    currentContext()->currentNameSlot=    currentContext()->currentNameSlot+(int)strlen(name)+1;
	return name;
}


bool checkStatement(Statement *s, bool checkSPOs, bool checkNamesOfSPOs) {
	if(!debug)return true;// bad idea!
	if (s == 0) return false;
	if (s < contexts[current_context].statements) return false;
	if (s >= contexts[current_context].statements + maxStatements) return false;
	if (s->id() == 0) return false; // !
	if (checkSPOs || checkNamesOfSPOs) if (s->Subject() == 0 || s->Predicate() == 0 || s->Object() == 0) return false;
	if (checkNamesOfSPOs) if (s->Subject()->name == 0 || s->Predicate()->name == 0 || s->Object()->name == 0) return false;
    if(s->subject==0&&s->predicate==0&&s->object==0)return false;// one 0 OK :ANY
	return true;
}


Node * reify(Statement * s) {
	if (!checkStatement(s,0,0)) return 0;
#ifdef useContext
	Node* reified=add(statementString(s), _reification, s->context);
#else
	Node* reified=add(statementString(s), _reification, current_context);
#endif
	reified->value.statement=s;
	reified->kind=_statement;
	return reified;
}

char* name(Node * node) {
	if (!node) return (char*)"NULL";
	if (node == 0) return (char*)"NULL";
	return node->name;
}

//static Context contexts[maxContexts];
// Context* contexts;

Context * getContext(int contextId) {
    //	if (contextId == 0) {
    //        p("Context#0!");
    //	}
	if (!multipleContexts) contextId=wordnet; // just one context

	Context* context=&(contexts[contextId]); //*(Context*)malloc(sizeof(Context*));

	if (context->nodes != null){// && context->id == contextId) {
		//		printf("Found context %d: %s\n",context->id,context->name);
		//		flush();
		return context;
	}

	printf("Reset context %d: %s", context->id, context->name);
	context->id=contextId;
	context->currentNameSlot=0; //context->nodeNames;
	context->extrahashNr=0;
#ifdef statementArrays
    context->statementArrays = (int*) malloc(maxStatements());
#endif
	initContext(context);
	if (contextId == wordnet) context->nodeCount=1000; //sick hack to reserve first 1000 words!
	else context->nodeCount=0;
	context->statementCount=1; //1000;
	strcpy(context->name, "Public");
	//
	//		printf("\nxx %d\n",context);// ==
	//        printf("aa %d\n",nodes);// ==
	//        printf("a0 %d\n",&nodes);// ==
	//        printf("a1 %d\n",context->nodes);// ==
	//	    printf("b2 %d\n",&context->nodes[0]);
	//	    printf("bb %d\n",context->nodes[0]);
	//	    printf("c3 %d\n",&context->nodes[context->nodeCount]);
	//	    printf("cc %d\n",context->nodes[context->nodeCount]);
	showContext(contextId);
	//		contexts[contextId]=context;
	return context;
}

void showContext(Context * cp) {
	if (quiet) return;
	Context c=*cp;
	cchar* format="Context#%d name:%s nodes:%d, statements:%d n#%p nN#%p s#%p\n";
	printf(format, c.id, c.name, c.nodeCount, c.statementCount, c.nodes, c.nodeNames, c.statements);
	flush();
}

void showContext(int nr) {
	showContext((Context*) getContext(nr));
}

void checkExpand(Context * context) {
	//    if(!context->nodeNames)
	context->nodeNames=(char*) malloc(nameBatch);
	if (context->currentNameSlot % nameBatch < 5) {
		void* tmp=realloc(context->nodeNames, context->currentNameSlot + nameBatch);
		if (!tmp) {
			p("Out of memory error");
		} else if (tmp != context->nodeNames) {
			p("context->names moved!! what is with the pointers??");
			context->nodeNames=(char*) tmp; //dare it
		} else if (!quiet) p("context->names checkExpanded");
	}
}
//bool isA4(Node* n, string match, bool recurse, bool semantic){
// return	isA4(n, match, recurse?1:0, semantic);
//}
bool isA4(Node* n, string match, int recurse, bool semantic) {

	if (!checkNode(n)) return false;
	if (eq(n->name, match.c_str())) return true; //&& n->name==match
	if (get(n->kind) && eq(get(n->kind)->name, match.c_str())) return true;

	if (recurse > 0) recurse++;
	else recurse=maxRecursions;
	if (recurse > maxRecursions) return false;

	if (semantic && has(n, "synonym", match, false, false, true)) return true;
	if (semantic && has(n, "plural", match, false, false, true)) return true;
	if (semantic && has(n, "type", match, false, false, false)) return true;
	if (semantic && has(n, "parent", match, false, false, false)) return true;
	//    if(semantic && has(n,"kind",match,false,false,false))return true;//?
	if (semantic && has(n, "name", match, false, false, false)) return true;
	if (semantic && has(n, "label", match, false, false, false)) return true;
	if (semantic && has(n, "is", match, false, false, false)) return true; // --

	if (semantic && n->kind == Abstract->id) { //|| isA(n,List)
		Statement* s=0;
		while ((s=nextStatement(n, s, false))) {
			if (s->Predicate() == Instance) if (isA4(s->Object(), match, recurse, semantic)) return true;
			if (s->Predicate() == Type) if (isA4(s->Subject(), match, recurse, semantic)) return true;
		}
		//		for (int i = 0; i < n->statementCount; i++) {// todo : iterate nextStatement(n,current)
		//			Statement* s = getStatementNr(n, i);
		//			if (s->Predicate == Instance)
		//				if (isA4(s->Object, match, recurse, semantic))
		//					return true;
		//		}
	}
	return false;
}

// global Statement
Statement * getStatement(int id, int context_id) {
	if (id == 0) {
		return null;// i.e. lastStatement
	}
	if (id < 0) {
		bad();
		return null;
	}
	if (id >= maxStatements) {
		p("maxStatements reached!");
		bad();
		return null;
	}
	Context* context=getContext(context_id);
	return &context->statements[id];
}


Statement * nextStatement(int node,int current) {
    return nextStatement(get(node),getStatement(current));
}
Statement* nextStatement(int node,Statement* current) {
    return nextStatement(get(node),current);
}
Statement * nextStatement(Node* n, Statement* current, bool stopAtInstances) {
	if (current == 0) return getStatement(n->firstStatement);
	if (stopAtInstances && current->Predicate() == Instance) return null;
	//	if (stopAtInstances && current->Object == n && current->Predicate == Type)return null; PUT TO END!!
	Statement* neu=null;
	if (current->Subject() == n) return getStatement(current->nextSubjectStatement);
	if (current->Predicate() == n)return  getStatement(current->nextPredicateStatement);
	if (current->Object() == n) return getStatement(current->nextObjectStatement);
//	if(current==neu){	// check here?
//		p("MEGABUG: current==neu");
//		return null;
//	}
	return neu;
}

Node * initNode(Node* node, int id, const char* nodeName, int kind, int contextId) {
	Context* context=getContext(contextId);
	if (!checkNode(node, id, false, false)) {
		p("OUT OF MEMORY!");
		return 0;
	}
	if (context->currentNameSlot + 1000 > averageNameLength * maxNodes) {
		p("OUT OF MEMORY!");
		return node;
	}
	node->id=id;
    setLabel(node, nodeName,false,false);

#ifdef useContext
 	node->context=contextId;
#endif

    node->kind=kind;
//	if (node->value.number)
	node->value.number=0; //Necessary? overwrite WHEN??
	if (id > 1000) {
		node->statementCount=0; // reset Necessary? overwrite WHEN?? better loss than corrupt
		node->lastStatement=0;
		node->firstStatement=0;
	}
#ifdef inlineStatements
	node->statements = 0; //nextFreeStatementSlot(context,0);
#endif
	return node;
}

bool checkNode(Node* node, int nodeId, bool checkStatements, bool checkNames,bool report) {//
//	bool report=true;
	if (node == 0) {
		bad();
		if (debug) printf("^"); // p("null node");
		//		p(nodeId);
		return false;
	}
	if(!debug)return true;
	Context* c=currentContext(); // getContext(node->context);
	void* maxNodePointer=&c->nodes[maxNodes];
	if (node < c->nodes) {
		bad();
		if(report){// not for abstract.node (can be number etc)
		printf("node* < c->nodes!!! %p < %p \n", node, c->nodes);
        p("OUT OF MEMORY or graph corruption");
		}
		return false;
	}
	if (node >= maxNodePointer) {
		bad();
		if(report){
        printf("node* >= maxNodes!!! %p > %p\n", node, maxNodePointer);
        p("OUT OF MEMORY or graph corruption");
//		exit(0);
		}
		return false;
	}
#ifdef useContext
	if (node->context < 0 || node->context > maxContexts) {
		bad();
		if(report){
		p("wrong node context");
		p("node:");
		p(nodeId);
		p("context:");
		p(node->context);
		}
		return false;
	}
#endif
	if (nodeId > maxNodes) {
		bad();
		if(report)
		pf("nodeId>maxNodes %d>%ld", nodeId, maxNodes);
		return false;
	}

	if (nodeId > 1 && node->id > 0 && node->id != nodeId) {
		bad();
		if(report)
		pf("node->id!=nodeId %d!=%d", node->id, nodeId);
		return false;
	}

	if (checkNames && node->name == 0) {// WHY AGAIN??
		bad();
		if(report)
		printf("node->name == 0 %p\n", node);
		return false;
	}
	if (checkNames && (node->name < c->nodeNames || node->name >= &c->nodeNames[averageNameLength * maxNodes])) {
		bad();
		if(report)
		printf("node->name out of bounds %p\n", node);
		return false;
	}
#ifdef inlineStatements
	if (checkStatements && node->statements == null) { //
		bad();
		if(report){
		p("node not loaded");
		p(nodeId);
		}
		// initNode(subject,subjectId,(char*)NIL_string,0,contextId);
		return false;
	}
#endif
	return true;
}
// return false if node not ok
// remove when optimized!!!!!!!!!!
bool checkNode(int nodeId, bool checkStatements, bool checkNames,bool report) {
	if(nodeId<0||nodeId>=maxNodes)return false;
	return checkNode(get(nodeId),nodeId , checkStatements,  checkNames,report);
}

Node * add(const char* key, const char* nodeName) {
	N node=add(nodeName);
	insertAbstractHash(wordhash(key), node);
	return node;
}

Node * add(const char* nodeName, int kind, int contextId) { //=node =current_context
    if (kind<0||kind>maxNodes)kind=abstractId;// blueprint messup!
#ifndef DEBUG
	if (!nodeName) return 0;
#endif
    contextId=wordnet;
	Context* context=getContext(contextId);
	Node* node=&(context->nodes[context->nodeCount]);
	if (context->nodeCount > maxNodes) {
		pf("context->nodeCount > maxNodes %d>%ld ",context->nodeCount ,maxNodes);
		p("MEMORY FULL!!!");
        //		exit(1);
		return 0;
	}
    initNode(node, context->nodeCount, nodeName, kind, contextId);
	context->nodeCount++;
	if (kind == abstractId|| kind == singletonId) return node;
	addStatement(getAbstract(nodeName), Instance, node, false);// done in initNode//setLabel !
	if (storeTypeExplicitly && kind > 105) // might cause loop?
        addStatement4(contextId, node->id, Type->id, kind, false); // store type explicitly!
	//	    why? damit alle Instanzen etc 'gecached' sind und algorithmen einfacher. Beth(kind:person).
	//  kosten : Speicher*2
	return node;
}

Node * add_force(int contextId, int id, const char* nodeName, int kind) {
	Context* context=getContext(contextId);
	// pi(context.nodes);// == &context.nodes[0] !!
	if (id > maxNodes) {
		if (quiet) return 0;
		printf("int context %d, int id %d id>maxNodes", contextId, id);
		return 0;
	}
#ifdef inlineStatements
	if (context->nodes[id].statements == null)
#endif
        Node* node=context->nodes + id;
	initNode(node, id, nodeName, kind, contextId);
	context->nodeCount++; // really?  add one , otherwise : overwrite
	return node;
}

#ifdef inlineStatements

bool addStatementToNode2(Node* node, int statement) {
	if (node->statementCount < maxStatementsPerNode) {
		node->statements[node->statementCount] = statement; //? nodeCount;//!! #statement dummy nodes ?? hmm --
		node->statementCount++;
	} else {
		if (maxStatementsPerNode == node->statementCount) { // warn once
			bad();
			// p("maxStatementsPerNode!!");
			// p(node->name);
		}
		return false;
	}
	return true;
}
#endif

Statement * addStatement4(int contextId, int subjectId, int predicateId, int objectId, bool checkNodes) { //bool checkDuplicate
	if (contextId < 0 || subjectId < 0 || predicateId < 0 || objectId < 0) {
		p("WARNING contextId<0||subjectId<0||predicateId<0||objectId<0");
		return 0;
	}
	if (contextId > maxContexts || subjectId > maxNodes || predicateId > maxNodes || objectId > maxNodes) {
		bad();
		p("WARNING contextId>maxContexts||subjectId>maxNodes||predicateId>maxNodes||objectId>maxNodes");
		return 0;
	}
	Context* context=getContext(contextId);
	// pi(context.nodes);// == &context.nodes[0] !!
	if (subjectId > maxNodes || predicateId > maxNodes || objectId > maxNodes) {
		if (quiet) return 0;
		printf("int context %d,subjectId %d>maxNodes || predicateId %d>maxNodes ||objectId %d>maxNodes ", contextId, subjectId, predicateId,
               objectId);
		return 0;
	}
	Node* subject=&context->nodes[subjectId];
	Node* predicate=&context->nodes[predicateId];
	Node* object=&context->nodes[objectId];

    //	if (checkDuplicate) {	//todo: add specifications but not generalizations?
    //		Statement* old=findStatement(subject, predicate, object, 0, 0, 0); //,true,true,true);
    //		if (old) return old; // showStatement(old)
    //	}

	if (subject == object) {
		pf("REALLY subject==object?? %s %s %s (%d->%d->%d)\n", subject->name, predicate->name, object->name, object->id,predicate->id,object->id);
        //		return 0;
	}
	//	if(predicate==Antonym)
	//		p("SD");

	if (checkNodes && !checkNode(subject, subjectId)) return 0;
	if (checkNodes && !checkNode(object, objectId)) return 0;
	if (checkNodes && !checkNode(predicate, predicateId)) return 0;

	Statement* s=getStatement( subject->lastStatement);
	if(s&&s->subject==subjectId&&s->predicate==predicateId&&s->object==objectId)
		return s;// quick checkDuplicate!

    int id=context->statementCount;
	Statement* statement=&context->statements[id]; // union of statement, node??? nee
    //	statement->id()=context->statementCount;
	context->statementCount++;
#ifdef explicitNodes
	statement->Subject=subject;
	statement->Predicate=predicate;
	statement->Object=object;
#endif

	statement->subject=subjectId;
	statement->predicate=predicateId;
	statement->object=objectId;
	if (!addStatementToNode(subject, id)) return statement;
	if (!addStatementToNode(object, id)) return statement;
	if (!addStatementToNode(predicate, id)) return statement;

	// predicate->statementCount++;
	// context->nodeCount++;
	return statement;
}

Statement * addStatement(Node* subject, Node* predicate, Node* object, bool checkDuplicate,bool force_insert_at_start) {
	if (!checkNode(subject)) return 0;
	if (!checkNode(object)) return 0;
	if (!checkNode(predicate)) return 0;

	Statement* s=getStatement( subject->lastStatement);
	if(s&&s->Subject()==subject&&s->Predicate()==predicate&&s->Object()==object)
		return s;// quick checkDuplicate!

	if (checkDuplicate) {	//todo: add specifications but not generalizations?
		Statement* old=findStatement(subject, predicate, object, 0, 0, 0); //,true,true,true);
		if (old) return old; // showStatement(old)
	}
	Context* context=currentContext(); //  getContext(subject->context); // todo!
	// pi(context.nodes);// == &context.nodes[0] !!

	//	if(isAbstract(object)&&( predicate==Type||predicate==SuperClass))
	//		object=getThe(object);
	if (subject == object && predicate->id < 1000) return 0;

    //	Statement* last_statement=&context->statements[context->statementCount-1];
    //	if(context->statementCount>1000&&last_statement->Subject=subject&&last_statement->Predicate=predicate&&last_statement->Object=object)
    //		return last_statement;// direct duplicate!
    
    int id=context->statementCount;
	Statement* statement=&context->statements[id]; // union of statement, node??? nee // 3 mal f�����r 3 contexts !!!
    //	statement->id=context->statementCount;
#ifdef useContext
	statement->context=current_context; //todo!!
#endif

#ifdef explicitNodes
	statement->Subject=subject;
	statement->Predicate=predicate;
	statement->Object=object;
#endif

	statement->subject=subject->id;
	statement->predicate=predicate->id;
	statement->object=object->id;

	bool ok=addStatementToNode(subject, id,force_insert_at_start);
	ok=addStatementToNode(predicate, id)&&ok;
	ok=addStatementToNode(object, id)&&ok;
//    if(!ok)p("warning: addStatementToNode skipped ");// probably quick duplicate check
    
	//	subject->statements[subject->statementCount]=context->statementCount;//? nodeCount;//!! #statement dummy nodes ?? hmm --
	//	subject->statementCount++;
	//	predicate->statements[predicate->statementCount]=context->statementCount;//? nodeCount;//!! #statement dummy nodes ?? hmm --
	//	predicate->statementCount++;
	//    object->statements[object->statementCount]=context->statementCount;//? nodeCount;//!! #statement dummy nodes ?? hmm --
	//	object->statementCount++;
	// pi(context->statementCount);
	// context->nodeCount++;
    
	context->statementCount++;
	return statement;
}

// has to be called with i, NOT with  n->statements[i]!!
// returns first Statement BEFORE Instance predicate!
// TODO!!! what if x->y->x !?!?!
// TODO firstInstanceGap too complicated, but needed for nodes with 1000000 instances (city etc)
// TODO only used in addStatementToNodeWithInstanceGap !!

Statement * getStatementNr(Node* n, int nr, bool firstInstanceGap) {
	//	if(nr==0)return 0;// todo ????
//    	if (nr >= maxStatementsPerNode) {
//    		bad();
//    		return null;
//    	}
	if (n == null) {
		bad();
		return null;
	}
	//	if(n->statements==null){bad();return null;}
	if (nr >= n->statementCount) {
		bad();
		return null;
	}
	if (n->firstStatement < 0) {
		bad();
		return null;
	}
#ifdef useContext
    Context* c=getContext(n->context);
#else
    Context* c=currentContext();
#endif
	Statement* statement=&c->statements[n->firstStatement];
	Statement* laststatement=statement;
	for (int i=0; i < nr; i++) {
		if (statement == 0) {
			p("CORRUPTED STATEMENTS!");
			show(n);
			bad();
			return null;
		}
		if (!checkStatement(statement, true, false)) break;
		if (i > 0 && firstInstanceGap && statement->Predicate() == Instance) {
			return laststatement;
		}
		if (statement->Subject() == n) {
			statement=&c->statements[statement->nextSubjectStatement];
			continue;
		}
		if (statement->Predicate() == n) {
			statement=&c->statements[statement->nextPredicateStatement];
			continue;
		}
		if (statement->Object() == n) {
			statement=&c->statements[statement->nextObjectStatement];
			continue;
		}
	}
	if (firstInstanceGap && statement->Predicate() == Instance) {
		return laststatement;
	}
	return statement;
}

#ifdef inlineStatements
// has to be called with i, NOT with  n->statements[i]!!

Statement * getStatement2(Node* n, int nr) {
	if (nr >= maxStatementsPerNode)return null;
	if (n == null)return null;
	if (n->statements == null)return null;
	if (nr >= n->statementCount)return null;
	int snr = n->statements[nr];
	Context* c = getContext(n->context);
	if (nr > c->statementCount)
        p("Error: getStatement statementCount exceeded");
	Statement* statement = &c->statements[snr];
	return statement;
}
#endif

// Abstract notes are necessary in cases where it is not known whether it is the noun/verb etc.
//inline

Node * get(const char* node) {
	return getAbstract(node);
}
//inline Node* get(char* node) {
//	return getAbstract(node);
//}


//inline
Node * get(int nodeId) {
	if (debug&&(nodeId<0 || nodeId > maxNodes)) { // remove when debugged
	    if (quiet)return Error;
	    printf("Error: 0 > nodeId %d > maxNodes %ld \n", nodeId, maxNodes);
	    return Error;
	}
	return &currentContext()->nodes[nodeId];
}

int getId(char* node){
    return getAbstract(node)->id;// for blueprint!
}

Node* getNodeS(int node){// for blueprint! debug
    return get(node);
}

Node* getNodeP(int node){// for blueprint! debug
    return get(node);
}

Node* getNode(int node){
    return get(node);
}


static map<Node*, bool> dissected;
void dissectParent(Node * subject,bool checkDuplicates) {
   	if (subject == (Node*) -1) dissected.clear();
	if (!checkNode(subject, -1, false, true)) return;
	//if(isAName(s)ret. // noe!
	string str=replace_all(subject->name, " ", "_");
	str=replace_all(str, "-", "_");


   	if (dissected[subject]) return;
   	dissected[subject]=true;

	int len=(int)str.length();
	bool plural=(char) str[len - 1] == 's' && (char) str[len - 2] != 's' && ((char) str[len - 2] != 'n' || (char) str[len - 3] == 'o');

	if (!contains(str, "_") && !plural) return;
	if (contains(subject->name, "(")) return;
	if (contains(subject->name, ",")) return;
	if (contains(str, "_von_")) return;
	if (contains(str, "_vor_")) return;
	if (contains(str, "_zu_")) return;
	if (contains(str, "_of_")) return;
	if (contains(str, "_by_")) return;
	if (contains(str, "_de_")) return;
	if (contains(str, "_am_")) return;
	if (contains(str, "_at_")) return;
	if (contains(str, "_bei_")) return;
	if (contains(str, "_in_")) return;
	if (contains(str, "_from_")) return;
	if (contains(str, "_for_")) return;
	//        p("dissectWord");
	//        p(subject->name);
	//	if(startsWith(str,"the_")){// the end ... NAH!
	//		addStatement(subject,Synonym,getThe(str.substr(4).data()));
	//		return;
	//	}

	int type=(int)str.find("_");
	if (type < 1) type=(int)str.find(".");
	if (type >= 0 && len - type > 2) {
		string xx=str.substr(type + 1);
		const char* type_name=xx.data();
		Node* word=getAbstract(type_name); //getThe
		dissectParent(word);
		if (!checkNode(word) || !eq(word->name, type_name)) return; // HOW???
		addStatement(word, Instance, subject, false); // true expensive!!! check before!!
		//		addStatement(subject, Type, word, false); // true expensive!!! check before!!
	} else if (plural) {
		const char* singular=str.substr(0, len - 1).c_str();
		Node* word=getAbstract(singular);
		if (!checkNode(word) || !eq(word->name, singular)) return; // HOW???
		addStatement(word, Instance, subject, false); // true expensive!!! check before!!
		dissectParent(word);
	}
}

// Ausnahmen:
//Zugsicherung    Zugbeeinflussungssystem_S-Bahn_Berlin
//Zugunfall       Kesselwagenexplosion_in_der_BASF
//Zugsicherung    Geschwindigkeits�����berwachung_Neigetechnik
//Zugsicherung    Zugsicherung_mit_Linienleiter_1990
//Zuggattung      ICE_International
//Zug_(Stadt)     Hochschule_Luzern_�������_Wirtschaft
//Zug_(Stadt)     Padagogische_Hochschule_Zentralschweiz
//Zug     Fliegender_Hamburger
//Zug     Doppelstock-Stromlinien-Wendezug_der_LBE
//Zeitschriftenverlag     Gruner_+_Jahr
//Zeitschriftenverlag     Smith,_Elder_&_Co.
//Zeitschriftenverlag     Verlag_Neue_Kritik
//Zeitschriftenverlag     Verlag_Otto_Beyer
//Zeitschriftenverlag     Verlag_Technik

Node* dissectWord(Node * subject,bool checkDuplicates) {
	Node* original=subject;
    if (dissected[subject]) return original;
	if (!checkNode(subject, true, true, true)) return original;
    if(subject->statementCount>1000)checkDuplicates=false;// expansive isA4 !!!!
    //    => todo dissectWord befor loading data!!!!!

	string str=replace_all(subject->name, " ", "_");
	str=replace_all(str, "-", "_");
	//        p("dissectWord");
	//        p(subject->name);
	const char *thing=str.data();
	if (contains(thing, "_") || contains(thing, " ") || contains(thing, ".") || (endsWith(thing, "s")&&!germanLabels))
		dissectParent(subject); // <<

   	dissected[subject]=true;

	int len=(int)str.length();
	int type=(int)str.find(",");
	if (type >= 0 && len - type > 2) {
		//		char* t=(str.substr(type + 2) + "_" + str.substr(0, type)).c_str();
		//		Node* word = getThe(t); //deCamel
		//		addStatement(word, Synonym, subject, true);
		Node* a=getThe((str.substr(0, type).c_str()));
		Node* b=getThe((str.substr(type + 2).c_str()));
		addStatement(a, Instance, subject, true);
		addStatement(b, Instance, subject, true);
		dissectWord(a,checkDuplicates);
		dissectWord(b,checkDuplicates);
		return original;
		//		str = word->name;
		//        subject=word;
	}
	type=(int)str.find("(");
	if (type > 0 && len - type > 2) {		// not (030)4321643 !
		int to=(int)str.find(")");
		string str2=str.substr(type + 1, to - type - 1);
		Node* clazz=getThe(str2.c_str()); //,str.find(")")
		Node* word;
		if (type > 0) word=getThe(str.substr(0, type - 1).c_str()); //deCamel
		else word=getThe(str.substr(to + 1, len - 1).c_str()); //deCamel
		addStatement(word, Instance, subject, true);
		addStatement(clazz, Instance, word, true);
		//        addStatement(clazz, Member, word, true);
		addStatement(subject, Instance, clazz, true);
		//	    return;
		str=word->name;
		//        subject=word;
	}
	type=(int)str.find("_in_");
	if (type < 0) type=(int)str.find("_am_");
	if (type < 0) type=(int)str.find("_at_");
	if (type >= 0 && len - type > 2) {
		Node* at=the(location);
		Node* word=getThe(str.substr(0, type).c_str()); //deCamel
		Node* ort=getThe(str.substr(type + 4).c_str());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(subject, at, ort,checkDuplicates);
		dissectParent(ort,checkDuplicates);
		return original;
	}
	type=(int)str.find("_from_");
	if (type >= 0 && len - type > 2) {
		Node* from=getThe("from");
		Node* word=getThe(str.substr(0, type).c_str()); //deCamel
		Node* ort=getThe(str.substr(type + 6).c_str());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(subject, from, ort, checkDuplicates);
	}
	type=(int)str.find("_for_");
	type=(int)str.find("_für_");
	if (type >= 0 && len - type > 2) {
		Node* from=getThe("for");
		Node* word=getThe(str.substr(0, type).c_str()); //deCamel
		Node* obj=getThe(str.substr(type + 5).c_str());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(subject, from, obj, checkDuplicates);
	}
	type=(int)str.find("_bei_");
	if (type >= 0 && len - type > 2) {
		Node* in=getThe("near");
		//        check(eq(getThe("near")->name,"near"));
		Node* word=getThe(str.substr(0, type).c_str()); //deCamel
		Node* ort=getThe(str.substr(type + 5).c_str());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(subject, in, ort, checkDuplicates);
		if (original != subject) addStatement(original, in, ort, checkDuplicates);
		addStatement(subject, the(location), ort, checkDuplicates);
	}

	type=(int)str.find("'s_");
	if (type < 0) type=(int)str.find("s'_");// Oswalds' Cave ?
	if (type >= 0 && len - type > 2) {
		Node* word=getThe(str.substr(0, type).c_str()); //deCamel
		const char* o=str.substr(type + 4).c_str();
		Node* ort=getThe(o);
		addStatement(ort, Instance, subject, checkDuplicates);
		addStatement(subject, Member, ort, checkDuplicates);
        //		addStatement(word, Member, ort, checkDuplicates);
		addStatement(word, Instance, subject, checkDuplicates);
	}
	type=(int)str.find("_of_");// board of directors
	if (type < 0) type=(int)str.find("_de_"); // de_la_Casa
	if (type < 0) type=(int)str.find("_du_");
	//_della_ de la del des
	if (type >= 0 && len - type > 2) {
		Node* hat=Member;
		Node* word=getThe(str.substr(0, type).c_str()); //deCamel
		const char* o=str.substr(type + 4).c_str();
		Node* ort=getThe(o);
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(ort, hat, subject, checkDuplicates);
//		addStatement(ort, hat, word, checkDuplicates);
	}
	type=(int)str.find("_der_");
	if (type < 0) type=(int)str.find("_des_");
	if (type < 0) type=(int)str.find("_del_");
	//_della_
	if (type >= 0 && len - type > 2) {
		Node* hat=Member;
		Node* word=getThe(str.substr(0, type).c_str()); //deCamel
		string so=str.substr(type + 5); // keep! dont autofree
		Node* ort=getThe(so.data());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(ort, hat, subject, checkDuplicates);
	}
	type=(int)str.find("_von_");
	if (type >= 0 && len - type > 2) {
		Node* hat=Member;
		Node* word=getThe(str.substr(0, type).data()); //deCamel
		string so=str.substr(type + 5); // keep! dont autofree
		Node* ort=getThe(so.data());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(ort, hat, subject, checkDuplicates);
	}
	type=(int)str.find("._");
	if (type >= 0 && len - type > 2) {
		Node* nr=getThe(str.substr(0, type).c_str()); //deCamel
		Node* word=getThe(str.substr(type + 2).c_str());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(subject, Number, nr, checkDuplicates);
	}
	type=(int)str.find("_");
	if (type >= 0 && len - type > 2) {
		const char* rest=str.substr(type + 1).c_str();
		if(startsWith(rest, "of_"))rest+=3;// ...
		Node* word=getThe(rest);
		addStatement(word, Instance, subject, checkDuplicates);
	}
    return original;
	// todo: zu (ort/name) _der_ (nicht:name) bei von auf_der auf am (Angriff_)gegen (Schlacht_)um...
	//    free(str);
}

bool abstractsLoaded=true;

Node * getNew(const char* thing, Node* type) {
	if(type==Abstract)return getAbstract(thing);//||type==Singleton
    if (type<node_root||type>&node_root[maxNodes]) type=Object;// default parameter hickup through JNA
	N n=add(thing, type->id);
	return n;
}


Node * getClass(const char* word,Node* hint) {
	return getThe(word,hint);
}

Node * getSingleton(const char* thing) {
    //    if(	getThe(thing) ...)
    return getAbstract(thing);
}

Node * getThe(string thing, Node* type) {
	return getThe(thing.data(), type);
}

Node * getThe(const char* thing, Node* type){//, bool dissect) {
	if (thing == 0 || thing[0] == 0) {
		bad();
		return 0;
	}
//	if(eq("of Directors",thing))
//		thing=thing+3;
//	if(startsWith(thing,"of "))
//		thing=thing+3;
//	if(startsWith(thing,"in "))thing=thing+3;
    if(autoIds&&isInteger(thing))return get(atoi(thing));
	if (getRelation(thing)) // not here! doch
		return getRelation(thing);
//    replaceChar((char*)thing,'_',' ');// NOT HERE!
	Node* abstract=getAbstract(thing);
	Node* insta=getThe(abstract, type); // todo: best?
	if(insta && !insta->value.number && atof(thing)){//!=0&&eq(itoa(atof(thing)),thing)){
		insta->value.number=atof(thing);// hack, shouldn't be here!
		if(!type)insta->kind=_number;
	}
	if (insta) return insta;
    if(type==More)type=0;
	if (type) insta=add(thing, type->id);
	else insta=add(thing, Object->kind);
	if (insta == 0) {
		p("add node failed!!");
		ps(thing);
		return 0;
	}
    // else if(atoi(thing)!=0)//&&eq(itoa(atoi(thing)),thing)){
    //     insta->value.number=atoi(thing);
    //     if(!type)insta->kind=_integer;
    // }
//        object=value(thing,atof(thing),Number);
    
    //	if (dissect) dissectWord(insta); // dont remove! doch!
	return insta;
}



// only for relationships!
// void forceAbstract(Node* n){
//	long h = hash(n->name);
//	Ahash* found = &abstracts[abs(h) % maxNodes]; // TODO: abstract=first word!!! (with new 'next' ptr!)
//	found->abstract=n;
//	found->next=0;
//}

bool hasNode(const char* thingy) {
    return hasWord(thingy);
}

Node * hasWord(const char* thingy) {
	if (!thingy || thingy[0] == 0) return 0;
    //	if (thingy[0] == ' ' || thingy[0] == '_' || thingy[0] == '"') // get rid of "'" leading spaces etc!
    //    char* fixed=editable(thingy); // free!!!
    //	thingy=(const char*) fixQuotesAndTrim(fixed);// NOT HERE!
	int h=wordhash(thingy);
	Ahash* found=&abstracts[abs(h) % maxNodes]; // TODO: abstract=first word!!! (with new 'next' ptr!)
    Node* first;
	if (found&& found->abstract>0 && (first=get(found->abstract))){
        //		if (contains(found->abstract->name, thingy))// get rid of "'" leading spaces etc!
        //			return found->abstract;
		if (eq(first->name, thingy, true))	// tolower
			return first;
    }
    //	int tries=0; // cycle bugs

    //    	map<Node*, bool> visited;
//#ifdef DEBUG
//	map<int, bool> visited;// relatively EXPENSIVE!!
//#endif
    //	map<Ahash*, bool> visited;
	// ./clear-shared-memory.sh After changing anything here!!
	while (found >= abstracts && found < &extrahash[maxNodes]) {
//#ifdef DEBUG
//		if (visited[found->abstract] == 1) {// Only happens after messing with full memory
//			debugAhash(h);
//			p("visited[found] == 1 How the hell can that even be??? "); clear
//			p(found->abstract);
//			return 0;
//		}
//		visited[found->abstract]=1;
//#endif
		if (checkNode(found->abstract)) {
			//			if (contains(found->abstract->name, thingy))//contains enough ?? 0%Likelihood of mismatch?
			//				return found->abstract;
			if (eq(get(found->abstract)->name, thingy, true))			//teuer? n��, if 1.letter differs
				return get(found->abstract);
		}
        //		if (get(found->next) == found) {
        //        debugAhash(h);
        //			p("found->next == found How the hell can that even be? ");
        //			break;
        //		}
        if(!found->next)break;
        if(found->next<=0||found->next>maxNodes*2||found==&abstracts[found->next])break;
		found=&abstracts[found->next];
	}
	return 0;
}


Node* number(int nr){
    bool tmp=autoIds;
    autoIds=false;
    N n= getSingleton(itoa(nr).data());
    autoIds=tmp;
    return n;
}

extern "C" int nodeCount(){
    return currentContext()->nodeCount;
}extern "C" int statementCount(){
    return currentContext()->statementCount;
}
extern "C" int nextId(){
    return currentContext()->nodeCount++;
}

extern "C"
Node* getType(Node* n){
    Statement* s=findStatement(n,Type,Any);
    if(!checkStatement(s))return 0;// n
    return s->Object();
}


Node* dateValue(const char* val) {
	Node* n=getAbstract(val);// getThe(val);
	n->kind=Date->id;
//	n->value == 	char *	"1732-02-22\""
	return n;
	//	return value(val, atoi(val), Date);
}
/*
 <g.11vjx3759>   <measurement_unit.dated_percentage.source>      <g.11x1gf2m6>   .
 <g.11vjx3759>   <#type> <measurement_unit.dated_percentage>     .
 <g.11vjx3759>   <measurement_unit.dated_percentage.date>        "2005-04"^^<#gYearMonth>        .
 <g.11vjx3759>   <measurement_unit.dated_percentage.rate>        4.5     .
 <g.11vjx3759>   <#type> <measurement_unit.dated_percentage>     .
 */
Node* rdfValue(char* name) {
	if (name[0] == '"') name++; // ignore quotes "33"
	char* unit0=strstr(name, "^");
	if (!unit0 || unit0 > name + 1000 || unit0 < name) return 0;
	if(unit0[-1]=='"')unit0[-1]=0;
	while (unit0[0] == '^'){unit0[0]=0; unit0++;}
	if(name[0]==0)return 0;
	if (unit0[0] == '<') unit0++;
	if (unit0[0] == '#') unit0++;
	if (unit0[0] == '"') unit0++;
	const char* unit=unit0;
	if (eq(unit, ",)")) return 0; // LOL_(^^,) BUG!
	if (eq(unit, "xsd:integer")) unit=0; //-> number
	else if (eq(unit, "integer")) unit=0; //-> number
	else if (eq(unit, "int")) unit=0; //-> number
	else if (eq(unit, "double")) unit=0; //-> number
	else if (eq(unit, "xsd:decimal")) unit=0; //-> number return value(key, atof(key), Number);; //-> number
	else if (eq(unit, "xsd:float")) unit=0; //-> number
	else if (eq(unit, "xsd:nonNegativeInteger")) unit=0; //-> number
	else if (eq(unit, "yago0to100")) unit=0;
	if (!unit) return value(name, atof(name), Number);// unit==0 means number, ignore extra chars 123\"

	if (eq(unit, "m")) unit="Meter";
	else if (eq(unit, "%")) ; // OK
	else if (eq(unit, "s")) unit="Seconds";
	else if (eq(unit, "second")) unit="Seconds";
	else if (eq(unit, "r")) unit="Seconds";
	else if (eq(unit, "/km")) unit="Kilometer";
	else if (eq(unit, "km")) unit="Kilometer";
	else if (eq(unit, "kilometre")) unit="Kilometer";
	else if (eq(unit, "millimetre")) unit="mm";
	else if (eq(unit, "centimetre"));
	else if (eq(unit, "meter"));
	else if (eq(unit, "tonne"));
	else if (eq(unit, "volt"));
	else if (eq(unit, "g")) unit="Gram";
	else if (eq(unit, "gram")) unit="Gram";
	else if (eq(unit, "kilogram")) unit="kg";
	else if (eq(unit, "kilogramPerCubicMetre")) unit="kg/m^3";
	else if (eq(unit, "milligram"));
	else if (eq(unit, "hectopascal"));
	else if (eq(unit, "kilowatt"));
	else if (eq(unit, "byte"));
	else if (eq(unit, "knot"));
	else if (eq(unit, "litre"));
	else if (eq(unit, "bar"));
	else if (eq(unit, "kilonewton"));
	else if (eq(unit, "megawatt"));
	else if (eq(unit, "squareMetre"))unit="m^2";
	else if (eq(unit, "kilometrePerHour"))unit="km/h";
	else if (eq(unit, "xsd:date")) ; // parse! unit = 0; //-> number
	else if (eq(unit, "kelvin")) ; // ignore
	else if (eq(unit, "degreeCelsius")) unit="C"; // ignore
	else if (eq(unit, "degreeFahrenheit")) unit="F"; // ignore
	else if (eq(unit, "degreeRankine")) unit="R"; // ignore
	else if (eq(unit, "degrees")) ; // ignore
	else if (eq(unit, "dollar")) ; // ignore
	else if (eq(unit, "usDollar"))unit="dollar" ; // ignore
	else if (eq(unit, "euro")) ; // ignore
	else if (eq(unit, "squareKilometre"))unit="km^2" ; // ignore
	else if (eq(unit, "megabyte")) ; // ignore
	else if (eq(unit, "gramPerCubicCentimetre"))unit="g/cm^3" ; // ignore
	else if (eq(unit, "metrePerSecond"))unit="m/s" ; // ignore
	else if (eq(unit, "kilometrePerSecond"))unit="km/s" ; // ignore


	else if (eq(unit, "yagoISBN")) unit="ISBN"; // ignore
	else if (eq(unit, "yagoTLD")) unit="TLD"; // ???
	else if (eq(unit, "yagoMonetaryValue")) unit="dollar";// USD $
	else if (eq(unit, "gYear")) unit="year"; //Date;
	else if (eq(unit, "date")) return dateValue(name);
	else if (eq(unit, "dateTime")) return dateValue(name);
	else if (eq(unit, "gYearMonth")) return dateValue(name);
	else if (eq(unit, "gMonthDay")) return dateValue(name);

	else {
		//		printf("UNIT %s \n", unit); // "<" => SIGSEGV !!
		//		return 0;
	}
	//		, current_context, getYagoConcept(unit)->id
	//	return add(name);
	Node* unity=getThe(unit); // getThe(unit);//  getYagoConcept(unit);
	return value(name, atof(name), unity);
}

// Abstract nodes are necessary in cases where it is not known whether it is the noun/verb etc.
extern "C"
Node* getAbstract(const char* thing) {			// AND CREATE!
	if (thing == 0) {
		bad();
		return 0;
	}
    while(thing[0]==' '||thing[0] == '"')thing++;
	if(contains(thing,"^^"))
		return rdfValue(modifyConstChar(thing));
//		if(startsWith(thing,"of "))
//	if(eq("of Directors",thing))// WTFFFFF???
//		thing=thing+3;
    if(autoIds&&isInteger(thing))return get(atoi(thing));
	//	char* thingy = (char*) malloc(1000); // todo: replace \\" ...
	//	strcpy(thingy, thing);
	//	fixNewline(thingy);
//    replaceChar((char*)thing,'_',' ');// wah! careful with const!!! NOT HERE!
	Node* abstract=hasWord(thing);
	if (abstract) return abstract;
	abstract=add(thing, abstractId, abstractId); // abstract context !!
	if (!abstract) {
		p("out of memory!");
		//		exit(0);
		//		throw "out of memory exception";
		return 0;
	}
    //	Ahash* ok=
    insertAbstractHash(wordhash(thing), abstract);
    //	if (ok == 0) insertAbstractHash(wordhash(thing), abstract);		// debug
	if (doDissectAbstracts && (contains(thing, "_") || contains(thing, " ") || contains(thing, ".")))
		dissectParent(abstract);// later! else mess!?
    //	else dissectAbstracts(am Ende)
	//	collectAbstractInstances(abstract am Ende);
	return abstract;
}
void collectAbstractInstances(Node* abstract) {
	Context* c=getContext(current_context);
	for (int i=0; i < c->nodeCount; i++) {
		Node* n=&c->nodes[i];
		char* nname=n->name;
		if (eq(abstract->name, nname))
			addStatement(abstract, Instance, n);
	}
}
//Node* getThe(const char* word){
//    Node* n=find(current_context,word,true);
//    if(n==0)n=add(word);
//    return n;
//}

void showStatement(int id){
    showStatement(getStatement(id));
}
void showStatement(Statement * s) {
	//	if (quiet)return;
	Context* c=currentContext();
	if (s < c->statements || s > &c->statements[maxStatements]) {
		if (quiet) return;
		p("illegal statement:");
		printf("%p", s);
		return;
	}
	if (s == null) return;
    if(s->subject==0&&s->predicate==0&&s->object==0)return;//null / deleted
    
	if (checkNode(s->Subject(), s->subject) && checkNode(s->Predicate(), s->predicate) && checkNode(s->Object(), s->object))
        //        if(s->Object->value.number)
        //            printf("%d\t%s\t\t%s\t\t%g %s\t%d\t%d\t%d\n", s->id, s->Subject->name, s->Predicate->name, s->Object->value.number,s->Object->name, s->subject, s->predicate, s->object);
        //        else
        printf("<%d>\t%s\t\t%s\t\t%s\t\t%d->%d->%d\n", s->id(), s->Subject()->name, s->Predicate()->name, s->Object()->name, s->subject, s->predicate,
               s->object);

	else printf("#%d %d->%d->%d  [%p]\n", s->id(), s->subject, s->predicate, s->object, s);
	flush();
	// printf("%s->%s->%s\n",s->Subject->name,s->Predicate->name,s->Object->name);

}

//, bool showAbstract
extern "C"  const char* getName(int node){
    if(!checkNode(node,false,true))return "<ERROR>";//_IN_GET_NAME>";//
    return get(node)->name;
}

char* getLabel(Node * n) {
	Context* context=getContext(current_context);
	if (n->value.text > context->nodeNames && n->value.text < context->nodeNames + context->currentNameSlot) return n->value.text;
	Statement * s=findStatement(n, Label, Any, 0, false, false);
	if (s) return s->Object()->name;

	return 0;
}

void show(vector<char*>& v){
    for (int i=0; i<v.size(); i++) {
        p(v.at(i));
    }
}
void show(Statement * s){
	showStatement(s);
}
bool show(Node* n, bool showStatements) {		//=true
	//	if (quiet)return;
	if (!checkNode(n)) return 0;
    if(n->statementCount<=1){
        //        pf("%d|",n->id);
        //        return false;// !!! HIDE!!!
    }

	// Context* c=getContext(n->context);
	// if(c != null && c->name!=null)
	// printf("Node: context:%s#%d id=%d name=%s statementCount=%d\n",c->name, c->id,n->id,n->name,n->statementCount);
	//    printf("%s  (#%d)\n", n->name, n->id);
	string img="";
	cchar* text="";
	bool showLabel=false;//!debug;
	if (showLabel) img=getImage(n->name);
	if (showLabel && getLabel(n)) text=getLabel(n);
	//    if(n->value.number)
	//    printf("%d\t%g %s\t%s\n", n->id,n->value.number, n->name, img.data());
	//    else
	//		printf("Node#%p: context:%d id=%d name=%s statementCount=%d kind=%d\n",n,n->context,n->id,n->name,n->statementCount,n->kind);
	//		printf("%d\t%s\t%s\t%s\t(%p)\n", n->id, n->name,text, img.data(),n);
	printf("%d\t%s\t\t%s\t%s\t%d statements\n", n->id, n->name, text, img.data(), n->statementCount);
//    if(n->statementCount<=1)return false;
	//	printf("%s\t\t(#%d)\t%s\n", n->name, n->id, img.data());
	// else
	// printf("Node: id=%d name=%s statementCount=%d\n",n->id,n->name,n->statementCount);
	int i=0;
    //	int maxShowStatements=40; //hm

	if (showStatements) {
		Statement* s=0;
		while ((s=nextStatement(n, s))) {
			if (i++ > resultLimit) break;
			if (checkStatement(s)) showStatement(s);
            else pf("NOOOOO! BROKEN STATEMENT: %p",s);
		}
        printf("-----------------------^ %s #%d (kind: %d), %d statements ^---------------\n", n->name, n->id,n->kind,n->statementCount);
        flush();
	}
	return 1; // daisy
}
Node * showNode(Node* n) {
    show(n);return n;
}
Node * showNode(int id) {
	Node* n=&currentContext()->nodes[id];
	if (!checkNode(n, id)) return 0;
	show(n);
	return n;
}

// saver than iterating through abstracts?
NodeVector* findWords(int context, const char* word, bool first,bool containsWord) {	//=false
	// pi(context);
	NodeVector* all=new NodeVector();
	Context* c=getContext(context);
	for (int i=0; i < c->nodeCount; i++) {
		Node* n=&c->nodes[i];
		if (n->id==0||!checkNode(n, i, true, false)) continue;
        bool good=eq(n->name, word, true);
        if(containsWord)good=good||contains(n->name, word,true);
		if (good) {
			all->push_back(n);
			show(n);
			if (first) return all;
		}
	}
	if (all->size() == 0)
		if (!quiet) printf("cant find node %s in context %d\n", word, context);
	N a=getAbstract(word);
	NV is=instanceFilter(a);// expensive!!!
	for (int i=0; i < all->size(); i++){
		Node *n=all->at(i);
		if(!contains(is,n))addStatement(a,Instance,n);
	}
	return all;
}

NodeVector* findAllWords(const char* word) {
    return findWords(current_context,word,false,false);
}
NodeVector* findAllMatches(const char* word) {
    return findWords(current_context,word,false,true);
}

Statement* findStatement(int subject, int predicate, int object, int recurse, bool semantic, bool symmetric, bool semanticPredicate,bool matchName) {
    return findStatement(get(subject),get(predicate),get(object), recurse, semantic, symmetric,  semanticPredicate, matchName);
}

// DO    NOT	TOUCH	A	SINGLE	LINE	IN	THIS	ALGORITHM	!!!!!!!!!!!!!!!!!!!!
Statement * findStatement(Node* subject, Node* predicate, Node* object, int recurse, bool semantic, bool symmetric, bool semanticPredicate,bool matchName) {
	// DO    NOT	TOUCH	A	SINGLE	LINE	IN	THIS	ALGORITHM	!!!!!!!!!!!!!!!!!!!!
	if (recurse > 0) recurse++;
	else recurse=maxRecursions;
	if (recurse > maxRecursions || subject == 0) return 0;

	Statement * s=0;
	map<Statement*, bool> visited;
	while ((s=nextStatement(subject, s, predicate != Instance))) { // kf predicate!=Any 24.1. REALLY??
		if (visited[s]) return 0;
		visited[s]=1;
//        if(s->id()==4334||subject->id==4904654) // debug id
//        p(s);
		if (!checkStatement(s)) continue;
        //#ifdef useContext
		if (s->context == _pattern) continue;
        if(predicate==Type&&s->predicate!=_Type&& visited.size()>100)
			return 0;// expect types on top HAS TO BE ORDERED!!
        //#endif

		//		if(s->Predicate!=Any){
		if (s->Object() == Adjective && object != Adjective) continue; // bug !!
		if (s->Predicate() == Derived) continue; // Derived bug !!
		if (s->Predicate() == get(_attribute)) continue; // Derived bug !!
		if (s->Predicate() == get(50)) continue; // also bug !!
		if (s->Predicate() == get(91)) continue; // also bug !!
		if (s->Predicate() == get(92)) continue; // also bug !!
		//		}
		// ^^ todo

		//		X any X error
		//		native		derived		native		301562->81->251672
		//		good		also		good		302044->50->302076
		//		evil		attribute		evil		226940->60->302081
		//		showStatement(s); // to reveal 'bad' runs (first+name) ... !!!

		//		if (s->Object->id < 100)continue; // adverb,noun,etc bug !!
		if (subject == s->Predicate()) {
            ps("NO predicate statements!");
			break;
		}

		//        if(s->context != current_context)continue;// only queryContext
#ifdef use_instance_gap
		if (s->Predicate == subject || i > 1 && s->Predicate == Instance && predicate != Instance || i > 1 && s->Predicate == Type && predicate != Type) {
            ps("skipping Predicate/Instance/Kind statements");
			//            continue;
			break;// todo : make sure statements are ordered!
		}
#endif

		if (s->Predicate() == Instance && predicate != Instance && predicate != Any) continue; //  return 0; // DANGER!
		//		NOT COMPATIBLE WITH DISSECTED WORDS!!!!! PUT TO END!!!
        if(predicate==Any&&eq(s->Subject()->name,s->Object()->name))continue;

		// DO    NOT	TOUCH	A	SINGLE	LINE	IN	THIS	ALGORITHM	!!!!!!!!!!!!!!!!!!!!
		bool subjectMatch=s->Subject() == subject || subject == Any || isA4(s->Subject(), subject, false, false); //DONT CHANGE quick
        subjectMatch=subjectMatch||(matchName&& eq(s->Subject()->name,subject->name ));
		bool predicateMatch=(s->Predicate() == predicate || predicate == Any);
		predicateMatch=predicateMatch || (predicate == Instance && s->Predicate() == SubClass);
		predicateMatch=predicateMatch || (predicate == SubClass && s->Predicate() == Instance);
		predicateMatch=predicateMatch || isA4(s->Predicate(), predicate, false, false);
        predicateMatch=predicateMatch || ((matchName||semanticPredicate)&& eq(s->Predicate()->name, predicate->name ));
		bool objectMatch=s->Object() == object || object == Any || object->id==0|| isA4(s->Object(), object, false, false);// by name OK!
        objectMatch=objectMatch||(matchName&& eq(s->Object()->name,object->name ));

		if (subjectMatch && predicateMatch && objectMatch){
			return s;// GOT ONE!
		}

		// READ BACKWARDS
		// OR<-PR<-SR
		bool subjectMatchReverse = subject == s->Object() || subject == Any || isA4(s->Object(), subject, false, false);
		subjectMatchReverse=subjectMatchReverse||(matchName&& eq(subject->name,s->Object()->name  ));
		bool objectMatchReverse = object == s->Subject() || object == Any || isA4(s->Subject(), object, false, false);
		objectMatchReverse=objectMatchReverse||(matchName&& eq(object->name,s->Subject()->name  ));
		bool predicateMatchReverse=predicate == Any; // || inverse
		symmetric=symmetric || s->Predicate() == Synonym || predicate == Synonym || s->Predicate() == Antonym || predicate == Antonym;
		symmetric=symmetric && !(s->Predicate() == Instance); // todo : ^^ + more
		predicateMatchReverse=predicateMatchReverse || (predicate == Instance && s->Predicate() == Type);
		predicateMatchReverse=predicateMatchReverse || (predicate == Type && s->Predicate() == Instance);
		predicateMatchReverse=predicateMatchReverse || (predicate == SuperClass && s->Predicate() == SubClass);
		predicateMatchReverse=predicateMatchReverse || (predicate == SubClass && s->Predicate() == SuperClass);
		predicateMatchReverse=predicateMatchReverse || (predicate == Antonym && s->Predicate() == Antonym);
		predicateMatchReverse=predicateMatchReverse || (predicate == Synonym && s->Predicate() == Synonym);
//		predicateMatchReverse=predicateMatchReverse || (predicate == Translation && s->Predicate() == Translation);
		predicateMatchReverse=predicateMatchReverse || predicate == Any;
		predicateMatchReverse=predicateMatchReverse || (predicateMatch && symmetric);
		// DO    NOT	TOUCH	A	SINGLE	LINE	IN	THIS	ALGORITHM	!!!!!!!!!!!!!!!!!!!!

		//		predicateMatchReverse = predicateMatchReverse || predicate == invert(s->Predicate);// invert properties ?? NAH!!
		//		predicateMatchReverse = predicateMatchReverse || invert(predicate) == s->Predicate;// invert properties ?? NAH!!
		// sick:
		//        predicateMatchReverse = predicateMatchReverse || predicate == Instance && s->Predicate == SuperClass;
		//        predicateMatchReverse = predicateMatchReverse || predicate == SuperClass && s->Predicate == Instance;
		//        predicateMatchReverse = predicateMatchReverse || predicate == SubClass && s->Predicate == Type;
		//        predicateMatchReverse = predicateMatchReverse || predicate == Type && s->Predicate == SubClass;
		if (subjectMatchReverse && predicateMatchReverse && objectMatchReverse){
			return s;// GOT ONE!
		}

		if (!semantic) continue;
		///////////////////////// SEMANTIC /////////////////////////////
		// DO    NOT	TOUCH	A	SINGLE	LINE	IN	THIS	ALGORITHM
		subjectMatch=subjectMatch || (semantic && isA4(s->Subject(), subject, recurse, semantic));
		if (subjectMatch) objectMatch=objectMatch || (semantic && isA4(s->Object(), object, recurse, semantic));
		if ((subjectMatch && objectMatch) || symmetric) {
			if (semanticPredicate) predicateMatch=predicateMatch || isA4(s->Predicate(), predicate, recurse, semantic);
			else predicateMatch=predicateMatch || eq(s->Predicate()->name, predicate->name) || isA4(s->Predicate(), predicate, false, false);
		}
		if (subjectMatch && predicateMatch && objectMatch) {
			return s;// GOT ONE!
		}
		// DO    NOT	TOUCH	A	SINGLE	LINE	IN	THIS	ALGORITHM	!!!!!!!!!!!!!!!!!!!!
		predicateMatchReverse=predicateMatchReverse || (symmetric && predicateMatch);
		if (predicateMatchReverse) {
			subjectMatchReverse=subjectMatchReverse || isA4(s->Object(), subject, recurse, semantic);
			objectMatchReverse=objectMatchReverse || isA4(s->Subject(), object, recurse, semantic);
		}
		if (subjectMatchReverse && predicateMatchReverse && objectMatchReverse){
			return s;// GOT ONE!
		}
		///////////////////////// END SEMANTIC /////////////////////////////
	}
	return null;
}

void removeStatement(Node* n, Statement * s) {
	if (!n || !s) return;
	Statement *last=0;
	Statement *st=0;
	while((st=nextStatement(n,st))){ // FAST OK??
//	for (int i=0; i < n->statementCount; i++) { // SAVE but slow
//    	Statement* st=getStatementNr(n, i);
		if (st == s) {
			if (last == 0) {
				if (s->Subject() == n) n->firstStatement=s->nextSubjectStatement;
				if (s->Predicate() == n) n->firstStatement=s->nextPredicateStatement;
				if (s->Object() == n) n->firstStatement=s->nextObjectStatement;
			} else {
				if (s->Subject() == n) last->nextSubjectStatement=s->nextSubjectStatement;
				if (s->Predicate() == n) last->nextPredicateStatement=s->nextPredicateStatement;
				if (s->Object() == n) last->nextObjectStatement=s->nextObjectStatement;
			}
		}
		last=st;
	}
}

void deleteStatement(int id){
    deleteStatement(getStatement(id));
}
void deleteStatement(Statement * s) {
	if (!checkStatement(s, true, false)) return;
	removeStatement(s->Subject(), s);
//	removeStatement(s->Predicate(), s);// can be very expensive a->is->b 10000000 is Statements!
	removeStatement(s->Object(), s);
	s->Subject()->statementCount--;
	s->Predicate()->statementCount--;
	s->Object()->statementCount--;
	//	currentContext()->statements[s->id]=0;
	memset(s, 0, sizeof(Statement));
}

void deleteWord(const char* data, bool completely) {
	pf("deleteWord %s \n", data);
	Context* context=&contexts[current_context];
    if(data[0]=='$'){
        deleteStatement(&context->statements[atoi(data+1)]);
        return;
    }
	int id=atoi(data);
	if (id <= 0) {
		deleteNode(getThe(data));
		if (completely) {
			NodeVector* words=findWords(current_context, data, true,false); //getAbstract(data);
			for(int i=0;i<words->size();i++){
				Node* word=words->at(i);
				pf("deleteNode %s \n", word->name);
				deleteNode(word); // DANGER!!
			}
		}
	} else if (checkNode(&context->nodes[id], id, false, false)) deleteNode(&context->nodes[id]);
	else if (checkStatement(&context->statements[id], false, false)) deleteStatement(&context->statements[id]);

	else ps("No such node or statement: " + string(data));
}

void deleteWord(string * s) {
	remove(s->c_str());
}

void deleteNode(int id){
    deleteNode(get(id));
}
void deleteNode(Node * n) {
	if (!checkNode(n)) return;
	if (n->kind == abstractId) {
        NodeVector nv=instanceFilter(n);
        for (int i=0; i < nv.size(); i++) {
            Node* n=nv[i];
            deleteNode(n);
        }
		deleteStatements(n);
	}else{ //!!
		N a=getAbstract(n->name);
		deleteStatements(n);
        memset(n, 0, sizeof(Node)); // hole in context!
	}

}

void deleteStatements(Node * n) {
	Statement* s=0;
	Statement* previous=0;
	while ((s=nextStatement(n, s))){
		if(previous)deleteStatement(previous);
		previous=s;
	}
	if(previous)deleteStatement(previous);// last one
    //	for (int i=0; i < minimum(n->statementCount, 10000); i++) {
    //		Statement* s=getStatementNr(n, i);
    //		deleteStatement(s);
    //	}
	n->statementCount=0;
	n->firstStatement=0;
	n->lastStatement=0;
}

Node * parseValue(const char* aname) {
	if (contains(aname, " ")) {
		string s=(aname);
		string unit=s.substr(s.find(" ") + 1);
		return value(aname, atof(aname), unit.data());
	} else return value(aname, atof(aname), Number);//_number
}

//#include <stdlib.h>
//#include <math.h> //floor


cchar* shortName(cchar* unit){
    if(unit==0||unit[0]==0)
        return "";
    if (eq(unit, "kilometre")) unit="km";
	else if (eq(unit, "millimetre")) unit="mm";
	else if (eq(unit, "centimetre")) unit="cm";
	else if (eq(unit, "meter"))unit="m";
	else if (eq(unit, "tonne"))unit="cm";
	else if (eq(unit, "volt"))unit="V";
   	else if (eq(unit, "gram")) unit="g";
   	else if (eq(unit, "kilogram")) unit="kg";
   	else if (eq(unit, "Contains")) unit=" ";
    else if (eq(unit, "number")) unit=" ";
    return unit;
}
int valueId(const char* aname, double v, int unit){
    return value(aname,v,get(unit))->id;
}
Node * value(const char* aname, double v, const char* unit) {
	return value(aname,v,getThe(unit));
}
Node * value(const char* name, double v, Node* unit/*kind*/) {
	char* new_name=0;
    
    if(name==0||strlen(name)==0){
        new_name=(char*)malloc(1000);// no todo: name_root done in getThe()
        if(unit==Number||unit==Integer||unit==0){// double / long
            sprintf(new_name, "%g", v); //Use the shorter of %e or %f  3.14 or 24E+35
        }else if (unit&&unit!=Bytes&&unit->name) {
            if(v>1000000000||v<1000)
                sprintf(new_name, "%g %s", v, shortName(unit->name)); //Use the shorter of %e or %f  3.14 or 24E+35
            else
                sprintf(new_name, "%d %s", (int)v, shortName(unit->name)); // round
        }
        name=new_name;
    }
	Node *n;
	if(!unit||unit->id<1000)
		n= getThe(name); // 69 (year vs number!!)
	else
		n= getThe(name,unit);// 45cm != 45Volt !!!
   	if (unit)n->kind=unit->id;
    else n->kind=_number;
	n->value.number=v;
//    if(unit)addStatement(unit, Instance, n);// NO, getThe !!
    //    	n->value=v;
    if(new_name)free(new_name);
	return n;
}

extern "C" void saveData(int node,void* data,int size,bool copy){
    N n=getNode(node);
    if(!copy){n->value.data=data;return;}
    void* target=&currentContext()->nodeNames[currentContext()->currentNameSlot];
    
    memcpy(target, data,size);
    n->value.data=target;// data
    currentContext()->currentNameSlot+=size+1;
}

extern "C" void* getData(int node){
    N n=getNode(node);
    return n->value.data;
}

extern "C" Value getValue(int node){
    N n=getNode(node);
    return n->value;
}

Node * has(Node* n, string predicate, string object, int recurse, bool semantic, bool symmetric) {
	return has(n, getAbstract(predicate.data()), getAbstract(object.data()), recurse, semantic, symmetric,true);
}

Node * has(Node* subject, Node* predicate, Node* object, int recurse, bool semantic, bool symmetric, bool predicatesemantic,bool matchName) {
	if (recurse > 0) recurse++;
	else recurse=maxRecursions - 1;
	if (recurse > maxRecursions) return 0;
	if (recurse <= 2 && subject->kind == Abstract->id) {
		NodeVector all=instanceFilter(subject);// need big lookuplimit here? :( todo: filter onthe fly!
		for (int i=0; i < all.size(); i++) {
			Node* insta=(Node*) all[i];
			Node* resu=has(insta, predicate, object, recurse, semantic, symmetric, predicatesemantic,matchName);
			if (resu) return resu;
		}
	} else if(recurse>0) {
		NodeVector all=parentFilter(subject);
		for (int i=0; i < all.size(); i++) {
			Node* insta=(Node*) all[i];
			if (yetvisited[insta]) continue;
			Node* resu=has(insta, predicate, object, recurse, semantic, symmetric, predicatesemantic,matchName);
			//			yetvisited[insta]=true;
			if (resu) return resu;
		}
	}

	//    if(recurse>maxRecursions/3)semantic = false;
	//    printf("findStatement(n %d,predicate %d,object %d,recurse %d,semantic %d,symmetric %d)\n",n,predicate,object,recurse,semantic,symmetric);
	Statement* s=findStatement(subject, predicate, object, recurse, semantic, symmetric, predicatesemantic,matchName);
	if (s != null && s->Subject() == subject) return s->Object(); // a.b=*? return *
	if (s != null && s->Object() == subject) return s->Subject();
	return 0;
}

Node * isEqual(Node* subject, Node * object) {

	if (!isA4(subject, object)) return 0;
	//    if (isA4(subject, object))return subject;
	//    if(subject->kind==object->kind)
	if (subject->value.number == object->value.number) return subject;
	if (atof(subject->name) > 0 && atof(subject->name) == atof(object->name)) return subject;

	return 0;
}

Node * isGreater(Node* subject, Node * object) {
	//            if(subject->kind!=object->kind)return 0;
	double v=subject->value.number;
    if(!v)v=atof(subject->name);
	double w=object->value.number;
    if(!v)v=atof(object->name);
	if (subject->kind != 0 && subject->kind == object->kind && v > w) return subject;
	if (v > w) return subject;// todo !!
	return 0;
}

Node * isLess(Node* subject, Node * object) {
	//            if(subject->kind!=object->kind)return 0;
    double v=subject->value.number;
    if(!v)v=atof(subject->name);
	double w=object->value.number;
    if(!v)v=atof(object->name);
	if (subject->kind == object->kind && v < w) return subject;
	if (v && w && v < w) return subject;

	return 0;
}

Node * isAproxymately(Node* subject, Node * object) {
	if (isEqual(subject, object)) return subject;
	//    if(soundex(subject->name)==soundex(object->name))return subject;
	//    if(levin_distance(subject->name,object->name)<3)return subject;
	if (object->value.number == 0 && subject->value.number * subject->value.number > 0.1) return 0;
	float r=subject->value.number / object->value.number;
	if (r * r > 0.9 && r * r < 1.1) return subject;

	return 0;
}

// berlin (size greater 200 sqm)/*float*/

Node * has(Node* subject, Statement* match, int recurse, bool semantic, bool symmetric, bool predicatesemantic) {
	//    if(findStatement(match))
	//    if (match->Predicate == Equals)return has(subject, match->Subject, match->Object);

	//todo : iterate all property_values !!
	Node* property_value=has(subject, match->Subject(), Any, recurse, semantic, symmetric, predicatesemantic);
	//has(subject, match->Subject);
	//    if (!property_value)property_value = has(subject, match->Subject);// second try expensive!
	if (!property_value) return 0;
	if (match->Predicate() == Equals) return isEqual(property_value, match->Object());
	else if (match->Predicate() == Greater) return isGreater(property_value, match->Object());
	else if (match->Predicate() == Less) return isLess(property_value, match->Object());
	else if (match->Predicate() == Circa) return isAproxymately(property_value, match->Object());
	else if (match->Predicate() == Not) return isEqual(property_value, match->Object())? False: match->Object();//True;
	else return has(subject, match->Predicate(), match->Object()); // match->Subject == Subject?

	return 0;
	//    if(match->Predicate==Range) isGreater and isLess
}
int recursions=0;

bool hasValue(Node * n) {

	return (*(int*) &n->value) != 0;
}

bool areAll(Node* a, Node * b) {

	return isA4(a, b, true, true); // greedy isA4
}

bool isA4(Node* n, Node* match, int recurse, bool semantic, bool matchName) {
	if (n == match) return true;
	if (!n || !n->name || !match || !match->name) return false; //!!
	if (n->kind == match->id) return true; //
	//	if (n->id < 100 && match->id < 100)return false; // danger!
	if (get(n->kind) && eq(get(n->kind)->name, match->name, true))return true;	// danger: instance, noun
	if (n->id == match->id) return true; // how so??? "Type" overwritten by "kind" !!!!
	if(isAbstract(match))matchName=true;
	if (matchName&&eq(n->name, match->name, true)) return true;// only sometimes !!!
	long badHash=n->id + match->id * 10000000;
	if (useYetvisitedIsA) {
		if (yetvisitedIsA[badHash] == -1) return false;
		if (yetvisitedIsA[badHash] == 1) return true;
	}
	//        else if (yetvisitedIsA[n]==null)yetvisitedIsA[n]=true;
	//        else yetvisitedIsA[n]++;

	if (recurse > 0) recurse++;
	//    else recurse=maxRecursions;
	if (recurse > maxRecursions) return false;
	runs++;

	if (hasValue(n)) {	// false else?
		//        if(isA4(n->kind,match->kind))
		if (n->kind == match->kind) {
			if (n->value.number == match->value.number) return true;
			if (n->value.datetime == match->value.datetime) return true;
			if (n->value.statement == match->value.statement) return true;
			else return false;
		}
	}
	//        if (recurse > maxRecursions / 3)
	//            semantic = false;


	// todo:semantic true (level1)
	bool quickCheckSynonym=recurse == maxRecursions; // todo !?!??!
	if (quickCheckSynonym && findStatement(n, Synonym, match, false, false, true)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}

	bool semantic2=semantic && recurse > 5; // && ... ?;

	if (semantic && findStatement(n, Synonym, match, recurse, semantic2, true)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}
	//    if(semantic && has(n,Plural,match,false,false,true))return true;
	if (semantic && has(n, SuperClass, match, recurse, semantic2, false)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}
	if (semantic && has(n, Type, match, recurse, semantic2, false)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}
	if (semantic && has(match, Instance, n, recurse, semantic2, false)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}
	if (semantic && has(match, SubClass, n, recurse, semantic2, false)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}
	if (semantic && has(n, Label, match, false, false, false)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}
	if (semantic && recurse > 0 && findStatement(n, Plural, match, maxRecursions - 1, semantic2)) {
		yetvisitedIsA[badHash]=true;
		return true;
	} //(n,Plural,match,0,true,true))return true;
	if (semantic && recurse > 0 && findStatement(match, Plural, n, maxRecursions - 1, semantic2)) {
		yetvisitedIsA[badHash]=true;
		return true;
	} //(n,Plural,match,0,true,true))return true;

	//    if(isA(n,match->name,false,false))return true;// compare by name
	if (isAbstract(n) && recurse>0 && recurse < 3) { //|| isA(n,List)
		Statement* s=0;
		map<Statement*, bool> visited;
		while ((s=nextStatement(n, s, false))) {
			if (visited[s]) return 0;
			visited[s]=1;
			if (s->Predicate() == Instance) if (recurse && isA4(s->Object(), match, recurse, semantic)) {
				yetvisitedIsA[badHash]=true;
				return true;
			}
		}
	}
    if(useYetvisitedIsA)
        yetvisitedIsA[badHash]=-1;

	return false;
}

// todo: a.b=c findMember(b,b) error

Node * findMember(Node* n, string match, int recurse, bool semantic) {
	if (!n) return 0;
	if (recurse > 0) recurse++;
	if (recurse > maxRecursions) return 0;
	//	if (debug&&!eq(match.data(),"wiki_image"))
	//		show(n);
	for (int i=0; i < n->statementCount; i++) {
		Statement* s=getStatementNr(n, i); // Not using instant gap
		if (!s) {
			bad();
			continue;
		}
		//		if (debug)showStatement(s);
		if (isA4(s->Predicate(), match, recurse, semantic)){
            if (s->Subject() == n) return s->Object();
            else return s->Subject();
        }
		if (isA4(s->Object(), match, recurse, semantic)) return s->Object();

		if (isA4(s->Subject(), match, recurse, semantic)) return s->Subject();
	}
	return null;
}

// xpath    cities[population>100000, location=europe]
//Statement* toStatement(Node* n)
//Statement* getStatement(Node* n)

Statement * isStatement(Node * n) {
	if (n && n->kind == _statement) return n->value.statement;
	return 0;
}

Node* findProperty(Node* n , const char* m,bool allowInverse){
Statement* s=0;
while ((s=nextStatement(n,s))) {
    if(eq(s->Predicate()->name,m,true))
		if(allowInverse||eq(s->Subject(),n))
        return s->Object();
}
    return 0;
}

NodeVector findProperties(Node* n, const char* m,bool allowInverse=true){
    if(isInteger(m)) m=getThe(m)->name;
    NodeVector good;
    Statement* s=0;
    while ((s=nextStatement(n,s))) {// todont: lookuplimit
        if(eq(s->Predicate()->name,m,true)){
            if(s->Object()==n&&allowInverse)// wrong semantics egal  makes of mazda  "1991 Mazda 323 Hatchback		Make		Mazda"
                good.push_back(s->Subject());
            else if (!contains(good, s->Object(), false))
				if(allowInverse||eq(s->Subject(),n))
                good.push_back(s->Object());
            if(good.size()>resultLimit)return good;
        }
    }
    return good;
}

NodeVector findProperties(Node* n , Node* m,bool allowInverse=true){
    NodeVector good;
    if(!m){
        pf("Warning: empty property null for node %d\t%s",n->id,n->name);
     return good;
    }
    NV all=    instanceFilter(n);// need big lookuplimit here :( todo: filter onthe fly!
    //    if(!isAbstract(n))
    all.push_back(n);// especially for freebase singletons!
    // OR//    findStatement(Node *subject, Node *predicate, Node *object)
    for (int i=0; i<all.size(); i++){
        mergeVectors(&good, findProperties(all[i],m->name,false));
        if(good.size()>resultLimit)return good;
    }
	if(allowInverse)
	for (int i=0; i<all.size(); i++){
		mergeVectors(&good, findProperties(all[i],m->name,true));
        if(good.size()>resultLimit)return good;
    }
    return good;
}

NodeVector findProperties(const char* n, const char* m,bool allowInverse){
    if(isInteger(n)) return findProperties(getAbstract(n),m,allowInverse);
    if(isInteger(m)) return findProperties(getAbstract(n),getThe(m),allowInverse);
    NodeVector good;
    N a=getAbstract(n);
    NV all=    instanceFilter(a);// need big lookuplimit here :( todo: filter onthe fly!
    all.push_back(a);// especially for freebase singletons!
    for (int i=0; i<all.size(); i++){
        NV more=findProperties(all[i],m,allowInverse);
        mergeVectors(&good, more);
        if(good.size()>resultLimit)return good;
    }
    return good;// dedup(good);
}

Node * has(const char* n, const char* m) {
	return has(getAbstract(n), getAbstract(m));
}

Node * has(Node* n, Node * m) {
	clearAlgorithmHash(true);
	int tmp=resultLimit;
	resultLimit=1;
	NodeVector all=memberPath(n, m);
	resultLimit=tmp;
	if (all.size() > 0) return all.front();

	// how to find paths with property predicates?? so:
	clearAlgorithmHash();
	Node *no=0;
	if (!no) no=has(n, m, Any); // TODO: test
	return no; // others already done!!

	// deprecated:
	//	if (m->value.text != 0)// hasloh population:3000
	//		no = has(n, m, m->value); // TODO: test
	//    findPath(n,m,hasFilter);// Todo new algoritym
	if (!no) no=has(n, Part, m);
	if (!no) no=has(n, Attribute, m);
	if (!no) no=has(n, Substance, m);
	if (!no) no=has(n, Member, m);
	if (!no) no=has(n, UsageContext, m);
	if (!no) no=has(n, get(_MEMBER_DOMAIN_CATEGORY), m);
	if (!no) no=has(n, get(_MEMBER_DOMAIN_REGION), m);
	if (!no) no=has(n, get(_MEMBER_DOMAIN_USAGE), m);
	//inverse
	if (!no) no=has(m, Owner, n);
	if (!no) no=has(m, PartOf, n);
	if (!no) no=has(m, get(_DOMAIN_CATEGORY), n);
	if (!no) no=has(m, get(_DOMAIN_REGION), n);
	if (!no) no=has(m, get(_DOMAIN_USAGE), n);

	//    if(!n)n=has(n,Predicate,m);// TODO!
	//	if (!no)no = has(save, Any, m); //TODO: really?
	return no;
}

Statement * findRelations(Node* from, Node * to) {
	Statement* s=findStatement(from, Any, to, false, false, false, false);
	if (!s) return findStatement(to, Any, from, false, false, false, false);
	else return s;
}

Node * findRelation(Node* from, Node * to) {	// todo : broken Instance !!!
	Statement* s=findStatement(from, Any, to, false, false, false, false);
	if (!s) s=findStatement(to, Any, from, false, false, false, false);
	if (s) {
		if (s->Subject() == from) return s->Predicate();
		if (s->Object() == to) return s->Predicate();
		if (s->Subject() == to) return invert(s->Predicate());
		if (s->Object() == from) return invert(s->Predicate());
		//		if(s->Subject==from)return s->Predicate;
		//		else if(s->Object==from) return invert(s->Predicate);
		//		if(s->Object==to)return s->Predicate;
		//		else if(s->Subject==to) return invert(s->Predicate);
	}
	return null;
}

NodeVector show(NodeVector all){
 return    showNodes(all);
}
void show(NodeSet all, bool showStatements, bool showRelation, bool showAbstracts){
	NodeSet::iterator it;
	for(it = all.begin(); it != all.end(); ++it) {
			Node* node=(Node*)*it;
			show(node, showStatements);
	}
}

NodeVector showNodes(NodeVector all, bool showStatements, bool showRelation, bool showAbstracts) {
	int size=(int)all.size();
	ps("+++++++++++++++++++++++++++++++++++");
	for (int i=0; i < size && i<resultLimit; i++) {
		Node* node=(Node*) all[i];

		if (i > 0 && showRelation) {
			S r=findRelations(all[i - 1], node);
			N n=findRelation(all[i - 1], node);
			if (!r || !n) p("???->??? \n");
			else {
				pf("$%d-> ", r->id());
				pf("%s\n", n->name);
			}
		}
		show(node, showStatements);
	}
	if (!showRelation) {
		pf("++++++++++ Hits : %d ++++++++++++++++++++\n", size);
	} else ps("+++++++++++++++++++++++++++++++++++");
	return all;
}
//NodeVector match_all(string data){
//}
int getStatementId(long pointer){
	return (int)(pointer-(long)statement_root)/sizeof(Statement);
}
//NodeVector find_english(string& data) {
//}

Node * first(NodeVector rows) {
	if (rows.size() > 0) {
		Node* n=(Node *) rows[0];

		return n;
	}
	return 0;
}

Node * last(NodeVector rows) {
	int s=(int)rows.size();
	if (s > 0) {
		Node* n=(Node *) rows[s - 1];

		return n;
	}
	return 0;
}

void initUnits() {
	//    printf("Abstracts %p\n", abstracts);
	printf("Abstracts %p\n", abstracts);
	Ahash* ah=&abstracts[wordhash("meter") % maxNodes];//???;
	if (ah < abstracts || ah > extrahash /**2*/) {
		ps("abstracts kaputt");
        //		collectAbstracts();
	}
	Node* u=getThe("meter", Unit);
	addStatement(a(length), has_the("unit"), u);

	//    addSynonyms(u,"m");
	u=getThe("m^2", Unit);
	u=getThe("km^2", Unit);
	u=getThe("millisecond", Unit);
	u=getThe("mile", Unit);
	u=getThe("km", Unit);
	//    u=add("phone number",String);// normalize!
	//    addSynonyms(u,"m^2","sqm");
}

// SAME as evaluate!!
Statement * learn(string sentence) {
	ps("learning " + sentence);
	Statement* s=evaluate(sentence);
	if (checkStatement(s)) {
		showStatement(s);
		return s;
	} else {
		ps("not a valid statement:");
		ps(sentence);
		return 0;
	}
}

/*
 int collectAbstracts3() {
 Context* c = currentContext();
 Node* found = 0;
 int abstractId = Abstract->id;
 int max = c->nodeCount; // maxNodes;

 // collect Abstracts
 for (int i = 0; i < max; i++) {
 Node* n = &c->nodes[i];
 if (n == null || n->name == null || n->id == 0 || n->context == 0)
 continue;
 if (n->kind == abstractId)
 insertAbstractHash(n);
 }
 for (int i = 0; i < max; i++) {
 Node* n = &c->nodes[i];
 if (n == null || n->name == null || n->id == 0 || n->context == 0) {
 if (i > 1000) {
 ps("collectAbstracts : bad node:");
 pi(i);
 break;
 }
 continue;
 }
 Node* abstract = hasWord(n->name);
 if (abstract) {
 if (n->kind == abstractId)
 continue;
 else
 addStatement(abstract, Instance, n);//what?? no possible
 } else {
 if (n->kind == abstractId) {
 printf("%s should already be mapped!?!", n->name);
 insertAbstractHash(n);
 continue;
 } else {
 abstract = add(n->name, Abstract->id, c->id);


 abstracts->insert(pair<long,Node*>(hash(n->name), abstract));
 addStatement(abstract, Instance, n);
 }

 }
 }
 p("found abstracts:");
 pi(abstracts->size());
 return abstracts->size();
 }*/
//Node* nextNode(char* name){
//    add(name);
//}
//69 (Q30203): year
//69 (Q713048): natural number
//void cleanAbstracts(Context* c){
Node * getThe(Node* abstract, Node * type) {// first instance, TODO
	if (!abstract || !abstract->name) return 0;
    if(abstract->kind==singletonId)return abstract;
    if (getRelation(abstract->name)) // not here! doch
        return getRelation(abstract->name);
    if (type<node_root||type>&node_root[maxNodes]) type=0;// default parameter hickup through JNA
	if(abstract->value.node && checkNode(abstract->value.node,0,false,false,false) and eq(abstract->value.node->name,abstract->name))
		return abstract->value.node; // abstract->value.node as cache for THE instance
	if (type == 0) {
		// CAREFUL: ONLY ALLOW INSTANCES FOR ABSTRACTS!!!
		Statement* s=0;
		while ((s=nextStatement(abstract, s)))
			if (s->Predicate() == Instance && s->object!=0) {		// ASSUME RIGHT ORDER!?
				abstract->value.node=s->Object();
				return s->Object();
//				if (s->Subject() == abstract && eq(s->Object()->name,abstract->name))
//                    //			abstract->value.node = last->Object; // + cache!
//					return s->Object();
//				if (s->Object() == abstract && eq(s->Subject()->name,abstract->name))
//					if (isAbstract(s->Subject())) // abstract was not abstract!!!??
//						return s->Object();
//				//			abstract->value.node = last->Object; // + cache!
//				return s->Subject();
			}
		N first= add(abstract->name, 0); // NO SUCH!! CREATE!?
		if(!atoi(abstract->name))
			abstract->value.node=first; // CACHE! -> DON't store numbers in abstract->value (69: year, natural number, ...)
		return first;
	}
	if (type->id == abstractId || type->id == singletonId)
		return getAbstract(abstract->name); // safe

	Statement * s=0;
    Node* best=0;
	map<Statement*, int> visited; // You don't have to do anything. Just let the variable go out of scope.
	while ((s=nextStatement(abstract, s))) {
		if (visited[s]) return 0;
		visited[s]=1;
		if (!checkStatement(s, true, false)) continue;
		bool subjectMatch=(s->Subject() == abstract || abstract == Any);
		bool predicateMatch=(s->Predicate() == Instance);
		bool typeMatch=(type == 0 || s->Object() == type || s->Object()->kind == type->id);
		typeMatch=typeMatch||type==More || isA4(s->Object(), type, maxRecursions, true); //semantic, depth 0
		if (subjectMatch && predicateMatch && typeMatch){
            if(type!=More)
                return s->Object();
            if(!best||s->Object()->statementCount>best->statementCount)
                best=s->Object();
        }
		bool subjectMatchReverse=s->Object() == abstract;
		bool predicateMatchReverse=s->Predicate() == Type; // || inverse
		bool typeMatchReverse=(type == 0 || s->Subject() == type); // isA4(s->Object, type)

		if (type!=More&&subjectMatchReverse && predicateMatchReverse && typeMatchReverse) return s->Subject();
	}
	return best;
}

Node * getProperty(Node* node, cchar* key) {
	S s=findStatement(node, getThe(key), Any);
	//findStatement(node, getAbstract(key), Any); // todo? egal
	if(!checkStatement(s))return 0;
	return s->Object();
}

bool isA(Node* fro, Node * to) {
	if (isA4(fro, to, 0, 0)) return true;

	Statement* s=0;
	while ((s=nextStatement(fro, s)))
		if (s->Object() == fro && isA4(s->Predicate(), to)) return true;// x.son=milan => milan is_a son

	if (fro->kind == Abstract->id) {
		NodeVector all=instanceFilter(fro);
		for (int i=0; i < all.size(); i++) {
			Node* d=(Node*) all[i];
			while ((s=nextStatement(d, s)))
				if (s->Object() == d && isA4(s->Predicate(), to)) return true; // x.son=milan => milan is_a son
			if (!findPath(d, to, parentFilter).empty()) return true;
		}
	}
	return !findPath(fro, to, parentFilter).empty();
}

// all mountains higher than Krakatao
// todo:wordnet bugs: vulgar

void setValue(Node* node, Node* property, Node * value) {
	Statement *s=findStatement(node, property, value);
	if (s) {
		if (!eq(s->Object(), value)) {
			printf("value already set %s.%s=%s ... replacing with %s", node->name, property->name, s->Object()->name, value->name);
			removeStatement(node, s); // really?? save history?
		} else return; //Wert schon da => nix?
	}
	addStatement(node, property, value, false);
}

void setName(int node, cchar* label){
    return setLabel(get(node),label,false,false);
}
void setLabel(Node* n, cchar* label,bool addInstance,bool renameInstances) {
//    if(addInstance && n!=get(n->id))n=save(n);// HOW!?! WHAT?
//	if(label[0]=='<')
//		bad(); "<span dbpedia parser fuckup etc
    int len=(int)strlen(label);
    Context* c=currentContext();
	char* newLabel = name_root + c->currentNameSlot;
    if(n->name==0){
        n->name=newLabel;// c->currentNameSlot;
        if(!addInstance)n->kind=abstractId;
        //        addInstance=true;// unless abstract
    }
    else if(eq(n->name, label,false))return;

	if (strlen(n->name)>=len){// reuse! NOT when sharing char*s !!
        strcpy(n->name, label);
        n->name[len]=0;
    }else{
        strcpy(newLabel, label);
        int len=(int)strlen(label);
        newLabel[len]=0;
        n->name=newLabel;// c->currentNameSlot;
        c->currentNameSlot+=len + 1;
    }
	if (n->kind == _internal)return;
	if (n->kind == abstractId) {
		if(!renameInstances)return;
        NV all=instanceFilter(n);
        for(int i=0;i<all.size();i++)
            setLabel(all[i],label,false);
        if(!hasWord(label))
            insertAbstractHash(n);
        //        else
		//            mergeNode(getAbstract(label),n);
	} else {
		Node* a=getAbstract(label);
		if(addInstance)
			addStatement(a, Instance, n);
	}
    //    p(n);
    //    return n->name;
}

bool checkParams(int argc, char *argv[], const char* p) {
	string minus="-";
	string colon=":";
	string equals="=";
	for (int i=1; i <= argc; i++) {
		if (eq(argv[i], p)) return true;
		if (eq(argv[i], (colon + p[0]).c_str())) // import
			return true;
		if (eq(argv[i], (minus + p).c_str())) return true;
		if (eq(argv[i], (minus + minus + p).c_str()))

            return true;
	}
	return false;
}

string formatImage(Node* image,int size,bool thumb){
	if (!image || !checkNode(image)) return "";
    char* name=replaceChar(image->name,' ','_');
	if(startsWith(name, "File:")) name+=5;
	string hash=md5(name);
	string base="http://upload.wikimedia.org/wikipedia/commons/";
    if(!thumb)	return base + hash[0] + "/" + hash[0] + hash[1] + "/" +  name;
    char ssize[12];
    sprintf(ssize, "%d", size);
	return base +"thumb/"+ hash[0] + "/" + hash[0] + hash[1] + "/" + name + "/" + ssize + "px-" + name;
}

string getImage(cchar* a, int size,bool thumb) {
	return getImage(getAbstract(a),size,thumb);
}
string getImage(Node* a, int size,bool thumb) {
	if(!a||!checkNode(a))return 0;
	Node* i=findProperty(a, "wiki_image");
	if(!i)i=findProperty(a, "Bild");
	if(!i)i=findProperty(a, "Wappen");
	if (!i || !checkNode(i))if(a->kind!=abstractId)return getImage(a->name);
    return formatImage(i,size,thumb);
}

Node* mergeAll(char* target){
    Node* node=getAbstract(target);
    NV all=instanceFilter(node);
    for(int i=0;i<all.size();i++)
        mergeNode(node,all[i]);
    return node;
}

void setKind(int id,int kind){
    get(id)->kind=kind;
}

extern "C" Node* save(Node* copy){
    p("SAVING");
    p(copy);
    Node* node=get(copy->id);
    if(node==copy)return node;
    memcpy(node,copy,nodeSize);
    return node;
}

extern "C" void save2(Node n){
    if(get(n.id)==&n)return;
    memcpy(get(n.id),&n,nodeSize);
}

int test2() {
	return 12345;
}		// RUBY/ JNA !!

Node* mergeNode(Node* target,Node* node){
    Statement* s=getStatement( node->firstStatement);
    addStatementToNodeDirect(target,node->firstStatement);
	
    Statement* next;
    while (s) {
        next=nextStatement(target, s);
        if(s->Predicate()==Instance&&s->Subject()==target)
            deleteStatement(s);
        else{
            if(s->Subject()==node)s->subject=target->id;
            if(s->Predicate()==node)s->predicate=target->id;
            if(s->Object()==node)s->object=target->id;
        }
        s=next;
    }
    deleteNode(node);
    return target;
}


//#include <csignal> // or signal.h if C code // Generate an interrupt
//void SIGINT_handler(int x){
//	shutdown_webserver();
//}
//#define _MAIN_
int main(int argc, char *argv[]) {
	char* data=getenv("QUERY_STRING");
	if (data) {
		printf("Content-Type: text/plain;charset=us-ascii\n\n");
		printf("got QUERY_STRING %s", data);
		init();
		parse(data);
		//	start_server();
	}

	// load environment variables or fall back to defaults
	SERVER_PORT= getenv("SERVER_PORT")? atoi(getenv("SERVER_PORT")): SERVER_PORT;
	resultLimit= getenv("resultLimit")? atoi(getenv("resultLimit")): resultLimit;
	lookupLimit= getenv("lookupLimit")? atoi(getenv("lookupLimit")): lookupLimit;
	defaultLookupLimit=lookupLimit;

	//    signal(SIGSEGV, handler); // only when fully debugged!
    //	signal(SIGINT, SIGINT_handler); // only when fully debugged! messes with console!
	//    setjmp(loop_context);
	path=argv[0];
	path=path.substr(0, path.rfind("/") + 1);
	if (path.rfind("/dist") != -1) {
		path=path.substr(0, path.rfind("/dist") + 1);
	}
	system(string("cd " + path).c_str());
	data_path=path + "/data/";
	import_path=path + "import/";

	//	path=sprintf("%s/data",path);
	if (checkParams(argc, argv, "quiet")) quiet=true;
	if (checkParams(argc, argv, "quit")) exit(0);

	init();
	//    import();

	if (checkParams(argc, argv, "clear"))clearMemory();

	if (checkParams(argc, argv, "query")) {
		load();
		parse((const char*)cut_to(join(argv, argc).data(), "query "));
		exit(0);
		//        import();
	}

	if (checkParams(argc, argv, "import")) {
        if (checkParams(argc, argv, "all")||argc<2)
            importAll();
        else import(argv[2]); // danger netbase clear import save
		if (checkParams(argc, argv, "save")) save();
	}
	if (checkParams(argc, argv, "load")) load(false);
	else if (checkParams(argc, argv, "load_files")) load(true);
	else if (checkParams(argc, argv, "debug")) {
		printf("debugging\n");
		//        load();
		//		clean();
		testBrandNewStuff(); // << PUT ALL HERE!
		//		start_server();
		//		return 0;
		//		tests();
	} else {
		showHelpMessage();
		// *******************************
		parse(join(argv, argc).c_str()); // <<< HERE
		// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	}
	if (checkParams(argc, argv, "export")) export_csv();
	if (checkParams(argc, argv, "test")) testAll();
	if (checkParams(argc, argv, "server") || checkParams(argc, argv, "daemon") || checkParams(argc, argv, "demon")) {
		printf("starting server\n");
		start_server();
		return 0;
	}

	printf("Warnings: %d\n", badCount);
    printf("Node limit: %d\n",(int)maxNodes);
    printf("Statements: %d\n",(int)maxStatements);
	if (checkParams(argc, argv, "exit")) exit(0);
	//	testBrandNewStuff();
	console();
	//    } catch (std::exception const& ex) {
}
// _MAIN_ ^^^
