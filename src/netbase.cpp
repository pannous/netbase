#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <queue>

//# -arch i686 # -arch x86-64 -Xarch-x86-64 # x86_64-apple-darwin
//g++ -g -w src/import.cpp src/netbase.cpp src/tests.cpp src/util.cpp /usr/lib/libsqlite3.dylib -o netbase  and ./netbase $@

// gdb  catch throw
// gdb  bt // back trace

#include <stdio.h> // itoa
#include <stdlib.h>// itoa
#include <cstdlib> // exit, malloc
#include <string.h> // strcmp
#include <sys/stat.h> // mkdir

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
bool NO_TOPICS = false;//true;  true=speed for mimir02!!! DONT GO LIVE!
int nameBatch = 100;
Context *context_root = 0; // Base of shared memory after attached
Node *node_root = 0;
Statement *statement_root = 0;
char *name_root = 0;// points to english_words or german_words
char* english_words = 0;
char* german_words = 0;
Node *abstract_root = 0;
int *freebaseKey_root = 0;
int defaultLookupLimit = 1000;
int lookupLimit = 1000;// set per query :( todo : param! todo: filter while iterating 1000000 cities!!
int typeLimit = 1000;// internal relations
int queryLimit = 10000000;
bool count_nodes_down= false;
bool out_of_memory= false;
//Node** keyhash_root=0;

#ifdef USE_SEMANTICS
bool useSemantics=true;
#else
bool useSemantics = false;
#endif

bool autoDissectAbstracts = useSemantics;// ?
bool storeTypeExplicitly = false;
bool exitOnFailure = true;
bool autoIds = false;
bool testing = false;// ;true

#ifdef __NETBASE_DEBUG__
bool debug=true;// false
#else
bool debug = true;// false
#endif


bool showAbstract = false;

int maxRecursions = 7;
int runs = 0;
Context *contexts; //[maxContexts];// extern
std::string path = ""; // extern
string data_path = "";
string import_path = "./import/";

//extern "C" inline
//Context* context {
//	return &contexts[current_context];
//}

int badCount;
int current_context = wordnet;

//map<string, Node*> abstracts;
Ahash *abstracts; // Node name hash
Ahash *extrahash; // for hashes that are not unique, increasing

//map<const char*,Node*> abstracts;
map<int, int> wn_map;
//map<int, int> wn_map2;


bool useYetvisitedIsA = false; // BROKEN!! EXPENSIVE!!! true; // false;
map<double, short> yetvisitedIsA;
map<Node *, bool> yetvisited;

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
//map<string,int> reasons;
//string last_reason="";
//void bad(string reason) {
Node *bad(char* reason) {
	if(debug and reason)
		printf("%s\n",reason);
	else
		printf(".");// no reason

//	if(reason!=0)reasons[reason]=(reasons[reason]||0)+1;
	badCount++;
	return Error;
}

bool isAbstract(Node *object) {
	if(!object)return false;
	return object->kind == _abstract or object->kind == _singleton;// or object->kind ==0;// 0 WTF how?
}

bool isValue(Node *object) {
	return object->value.number != 0;
}

bool checkHash(Ahash *ah) {
	if (!debug)return true;
	//  if(pos>maxNodes*2)
	if (ah < abstracts or
	    ah > &abstracts[maxNodes * 2]) { // times 2 because it can be an extraHash outside of abstracts!!
		p("ILLEGAL HASH!");
		//	pi(pos);
		px(ah);
		return false;
	}
	return true;
}


void debugAhash(int position) {
	Ahash *ah = &abstracts[position];
	if (!checkHash(ah)) return;
	//  if(pos==hash("city"))
	//		p(a->name);
	int i = 0;
	while (ah->next) {
		if (i++ > 10) break;
		cchar *n = "ERROR";
		if (checkNode(ah->abstract)) n = get(ah->abstract)->name;
		pf("%d | %d | >>%s<< | ", position, i, n);
		if (checkNode(ah->abstract)) show(get(ah->abstract), false);
		else p("XXX");
		if (ah->next < 0 or ah->next > maxNodes)break;
		ah = &abstracts[ah->next];
	}
}

Ahash *getAhash(int position) {
	if (position < 0 or position > maxNodes * 2)return 0;
	return &abstracts[position];
}

// ./clear-shared-memory.sh After changing anything here!!
//int extrahashNr=0;// LOAD FROM CONTEXT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
map<Ahash *, bool> badAhashReported;// debug
Ahash *insertAbstractHash(unsigned int position, Node *a, bool overwrite/*=false*/, bool seo/*=false*/) {
	// DO NOT TOUCH THIS ALGORITHM (unless VERY CAREFULLY!!)
	if (a == 0)
		return 0;
	if (a == Error)
		return 0;
	if(!checkNode(a))
		return 0;
	Ahash *ah = getAhash(position);
	cchar *name = a->name;
	string retain;
	if (seo and name) {
		retain = generateSEOUrl(name);
		name = retain.data();
	}
	if (!checkHash(ah) or !checkNode(a) or !a->name or !strlen(name)) return 0;
	int i = 0;
	while (ah and ah->next) {// check existing
		if (i++ > 300 and name[1] != 0) {    // allow 65536 One letter nodes
			bad();
			if (badAhashReported[ah])return ah;
			badAhashReported[ah] = true;
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
		N ab = get(ah->abstract);
		if(ab->id==0){
			ah->abstract=a->id; // fix first deleted
			return ah;
		}
		if (ab == a)
			return ah; //schon da
		if (ab and checkNode(ab,0,0,1) and eq(ab->name, name, true)) {
			if (overwrite) {
//				if(ah->abstract>-propertySlots and ah->abstract<maxNodes-propertySlots)
//					get(ah->abstract)->kind=_entity;// 'free' old abstract BREAKS:(
				ah->abstract = a->id;
			}
			return ah; // NAME schon da!!
		}
		ah = getAhash(ah->next);
	}

	N ab = ah and ah->abstract ? get(ah->abstract) : 0;
	if (ab && checkNode(ab,0, false,true)) { //schon was drin
		if (ab == a)
			return ah; //schon da
		if (ab and eq(ab->name, name, true)) {
			if (overwrite) {
//								if(ah->abstract>-propertySlots and ah->abstract<maxNodes-propertySlots)
//									get(ah->abstract)->kind=_entity;// 'free' old abstract BREAKS:(
				ah->abstract = a->id;
			}
			return ah; // NAME schon da!!
		}
		ah->next = context->extrahashNr++;
		ah = getAhash(ah->next);
	}
	if (!ah or !checkHash(ah))
		return 0;
	ah->abstract = a->id;
	return ah;
}

Ahash *insertAbstractHash(Node *a, bool overwrite/*=false*/) {
	return insertAbstractHash(wordHash(a->name), a, overwrite);
}

inline bool eq(Statement *s, Statement *s2) {
	if (!s or !s2)return false;
	return (s->subject == s2->subject and s->predicate == s2->predicate and s->object == s2->object);
}

inline bool eq(Statement *s, int statementId) {
	return eq(s, getStatement(statementId));
}

bool appendLinkedListOfStatements(Statement *add_here, Node *node, int statementNr) {
	//  Statement* statement1=&c->statements[statementNr];
	if (add_here->id() == statementNr) {
		//		if(debug)
		//		pf("BUG add_here->id==statementNr %d in %d %s\n", statementNr, node->id, node->name);
		return false;
	}
	if (add_here->Subject() == node) add_here->nextSubjectStatement = statementNr;
	if (add_here->Predicate() == node) add_here->nextPredicateStatement = statementNr;
	if (add_here->Object() == node) add_here->nextObjectStatement = statementNr;
	return true;
}

bool prependLinkedListOfStatements(Statement *to_insert, Node *node, int statementNr) {
	return appendLinkedListOfStatements(to_insert, node, statementNr); // append old to new
}

//bool insert_at_start=true;// <<< TODO HACK, REMOVE!!

bool addStatementToNode(Node *node, int statementId, bool insert_at_start = false) {
	if (statementId == 0) {
		p("WARNING statementNr==0");
		return false;
	}
	int n = node->statementCount;
	if (n == 0) { // and ==0
		if (node->firstStatement != 0)
			pf("BUG node->firstStatement!=0 %d %s :%d\n", node->id, node->name, node->firstStatement);
		node->firstStatement = statementId;
		node->lastStatement = statementId;
	} else {
#ifdef useContext
		context=getContext(node->context);
#endif
		Statement *to_insert = &context->statements[statementId];
		//		if (to_insert->Predicate == Instance and to_insert->Subject == node or to_insert->Predicate == Type and to_insert->Object == node) {
		Node *predicate = to_insert->Predicate();
		insert_at_start = insert_at_start or predicate == ID and to_insert->subject == node->id;
		insert_at_start = insert_at_start or predicate == Label and to_insert->subject == node->id;
		insert_at_start = insert_at_start or predicate == Type and to_insert->subject == node->id;
		insert_at_start = insert_at_start or predicate == SuperClass and to_insert->subject == node->id;
		bool push_back = !insert_at_start;// default for all ~
		push_back = push_back or to_insert->subject != node->id;
		push_back = push_back or predicate == Instance;
		push_back = push_back or predicate == Type and to_insert->Object() == node;// 10^6 type animal
		push_back = push_back or predicate == node;
		if (insert_at_start)push_back = false;// force
		if (push_back) { // ALL!
			Statement *add_here = &context->statements[node->lastStatement];
			if (eq(add_here, statementId))
				return true;//false;
			appendLinkedListOfStatements(add_here, node, statementId); // append new to old
			node->lastStatement = statementId;
		} else { // invert sort -> sort again in webserver
//			if(eq(to_insert->id(),node->firstStatement)) // wow, ghet auch ohne ->id()
//				return true;//false;// already there
			prependLinkedListOfStatements(to_insert, node, node->firstStatement); // append old to new
			node->firstStatement = statementId;
		}
	}
	node->statementCount++;
	return true;
}

// ONLY USED IN mergeNode!
bool addStatementToNodeDirect(Node *node, int statementId) {
	int n = node->lastStatement;
	if (n == 0) {
		node->firstStatement = statementId;
		node->lastStatement = statementId;
		node->statementCount++;
		return true;
	} else {
		int statement2Nr = 0; // find free empty (S|P|O)statement slot of lastStatement
#ifdef useContext
		context=getContext(node->context);
#endif
		Statement *statement0 = &context->statements[node->lastStatement]; // last statement
		Statement *statement1 = &context->statements[statementId]; // target
		if (statement0->Subject() == node) {
			statement2Nr = statement0->nextSubjectStatement; // rescue old
			statement0->nextSubjectStatement = statementId;
		}
		if (statement0->Predicate() == node) {
			statement2Nr = statement0->nextPredicateStatement;
			statement0->nextPredicateStatement = statementId;
		}
		if (statement0->Object() == node) {
			statement2Nr = statement0->nextObjectStatement;
			statement0->nextObjectStatement = statementId;
		}
		// squeeze statement1 in between statement0 and statement2Nr
		appendLinkedListOfStatements(statement1, node, statement2Nr); // put target into free slot
		node->lastStatement = statementId;
	}
	node->statementCount++;
	return true;
}

char *statementString(Statement *s) {
	char *name = &context->nodeNames[context->currentNameSlot];
	sprintf(name, "(%s %s %s)", s->Subject()->name, s->Predicate()->name, s->Object()->name);
	context->currentNameSlot = context->currentNameSlot + (int) strlen(name) + 1;
	return name;
}


bool checkStatement(Statement *s, bool checkSPOs, bool checkNamesOfSPOs) {
	if (!context)context=getContext();
	if (s == 0) return false;
	if (s < context->statements) return false;
	if (s >= context->statements + maxStatements) return false;
	if (!debug)return true;// bad idea!
	if (s->id() == 0) return false; // !
	if (checkSPOs or checkNamesOfSPOs) if (s->Subject() == 0 or s->Predicate() == 0 or s->Object() == 0) return false;
	if (checkNamesOfSPOs)
		if (s->Subject()->name == 0 or s->Predicate()->name == 0 or s->Object()->name == 0)
			return false;
	if (s->subject == 0 and s->predicate == 0 and s->object == 0)return false;// one 0 OK :ANY
	return true;
}


Node *reify(Statement *s) {
	if (!checkStatement(s, 0, 0)) return 0;
#ifdef useContext
	Node* reified=add(statementString(s), _reification, s->context);
#else
	Node *reified = add(statementString(s), _reification, current_context);
#endif
	reified->value.statement = s;
	reified->kind = _statement;
	return reified;
}

char *name(Node *node) {
	if (!node) return (char *) "NULL";
	if (node == 0) return (char *) "NULL";
	return node->name;
}

Context *getContext(int contextId,bool init) {
	if(context)return context;
	if (!multipleContexts) contextId = wordnet; // just one context
#ifdef WASM
    p("NEW CONTEXT");
    context=new Context();
#else
	context = &(contexts[contextId]);
#endif
	if (context->nodes != null) {// and context->id == contextId) {
		//		printf("Found context %d: %s\n",context->id,context->name);
		//		flush();
		return context;
	}
	printf("Reset context %d: %s", context->id, context->name);
	context->id = contextId;
	context->currentNameSlot = 0; //context->nodeNames;
	context->extrahashNr = 0;
#ifdef statementArrays
	context->statementArrays = (int*) malloc(maxStatements());
#endif
	if(init)initContext(context);
//	if (contextId == wordnet) context->lastNode=1; //sick hack to reserve first 1000 words!
//	else context->nodeCount=0;
//	context->statementCount=1; //1000;
	strcpy(context->name, "Public");
	//
	//		printf("\nxx %d\n",context);// ==
	//    printf("aa %d\n",nodes);// ==
	//    printf("a0 %d\n",&nodes);// ==
	//    printf("a1 %d\n",context->nodes);// ==
	//	  printf("b2 %d\n",&context->nodes[0]);
	//	  printf("bb %d\n",context->nodes[0]);
	//	  printf("c3 %d\n",&context->nodes[context->nodeCount]);
	//	  printf("cc %d\n",context->nodes[context->nodeCount]);
	showContext(contextId);
	//		contexts[contextId]=context;
	return context;
}

void showContext(Context *cp) {
	if (quiet) return;
	if (!cp)cp = getContext(current_context);
	Context c = *cp;
	printf("Context#%d name:%s\n", c.id, c.name);
	printf("Pointer nodes:%p\t\tstatements:%p\tchars:%p\n", c.nodes, c.statements, c.nodeNames);
	printf("Current nodes:%d\t\tstatements:%d\t\tchars:%ld\n", c.nodeCount, c.statementCount, c.currentNameSlot);
	printf("Maximum nodes:%ld\t\tstatements:%ld\t\tchars:%ld\n", maxNodes, maxStatements, maxChars);
	printf("Usage  nodes:%.2f%%\t\t\tstatements:%.2f%%\t\tchars:%.2f%%\n",
	       100. * c.nodeCount / maxNodes, 100. * c.statementCount / maxStatements, 100. * c.currentNameSlot / maxChars);
	flush();
}

void showContext(int nr) {
	showContext((Context *) getContext(nr));
}

void checkExpand(Context *context) {
	//  if(!context->nodeNames)
	context->nodeNames = (char *) malloc(nameBatch);
	if (context->currentNameSlot % nameBatch < 5) {
		void *tmp = realloc(context->nodeNames, context->currentNameSlot + nameBatch);
		if (!tmp) {
			p("Out of memory error");
		} else if (tmp != context->nodeNames) {
			p("context->names moved!! what is with the pointers??");
			context->nodeNames = (char *) tmp; //dare it
		} else if (!quiet) p("context->names checkExpanded");
	}
}

// global Statement
Statement *getStatement(int id, int context_id) {
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
	Context *context = getContext(context_id);
	return &context->statements[id];
}


Statement *nextStatement(int node, int current) {
	return nextStatement(get(node), getStatement(current));
}

Statement *nextStatement(int node, Statement *current) {
	return nextStatement(get(node), current);
}

map<int,bool> lookedUpStatements;
int nextStatement_lookupLimit=1000000;// TODO!
void newQuery(){
	lookedUpStatements.empty();
	nextStatement_lookupLimit=1000000;
}

Statement *nextStatement(Node *n, Statement *current, bool stopAtInstances) {
	if(!n){
		bad("No Node given!");
		return 0;
	}
	if (!importing && nextStatement_lookupLimit--<0){
		bad("nextStatement lookupLimit reached! Cyclic graph?\r\n");
		return null;
	}// per web request todo better
	if (current == null) return getStatement(n->firstStatement);
	if (stopAtInstances and current->Predicate() == Instance) return null;
	//	if (stopAtInstances and current->Object == n and current->Predicate == Type)return null; PUT TO END!!
	Statement *neu = null;
	if (current->Subject())
		neu = getStatement(current->nextSubjectStatement);
	if (current->Predicate() == n)
		neu = getStatement(current->nextPredicateStatement);
	if (current->Object() == n)
		neu = getStatement(current->nextObjectStatement);
	if(current==neu){	// check here?
//		p("Cyclic graph current==neu");
		return null;
	}
	return neu;
}

Node *initNode(Node *node, int id, const char *nodeName, int kind, int contextId) {
	Context *context = getContext(contextId);
	if (!checkNode(node, id, false, false)) {
		p("OUT OF MEMORY");
		return 0;
	}
	if (context->currentNameSlot + 1000 > averageNameLength * maxNodes) {
		p("OUT OF NAME MEMORY!");
		return node;
	}
	node->id = id;
	setLabel(node, nodeName, false, false);

#ifdef useContext
	node->context=contextId;
#endif

	node->kind = kind;
//	if (node->value.number)
	node->value.number = 0; //Necessary? overwrite WHEN??
	if (id > 1000) {
		node->statementCount = 0; // reset Necessary? overwrite WHEN?? better loss than corrupt
		node->lastStatement = 0;
		node->firstStatement = 0;
	}
#ifdef inlineStatements
	node->statements = 0; //nextFreeStatementSlot(context,0);
#endif
//	if(node->kind==_abstract)// done in getAbstract
//		insertAbstractHash(node, true);// overwrite? sure??
	return node;
}

bool checkNode(Node *node, int nodeId, bool checkStatements, bool checkNames, bool report) {//
//	bool report=true;
	if (node == 0) {
		bad();// too common
//		bad("checkNode node == 0");
		if (debug)
			printf("^"); // p("null node");
		//		p(nodeId);
		return false;
	}
#ifdef WASM
if(nodeId<-propertySlots)return false;
if(nodeId>maxNodes)return false;
if(!node)return false;
if((long)node<=0)return false;
if((long)node>0x30000000)return false;// todo
#endif
//	if(!debug)return true;
	context = getContext(current_context);
	void *maxNodePointer = &context->nodes[maxNodes];
	if (node < context->nodes - propertySlots) {
		bad();
		if (report) {// not for abstract.node (can be number etc)
			printf("node* < context->nodes!!! %p < %p \n", node, context->nodes);
			p("OUT OF MEMORY or graph corruption");
		}
		return false;
	}
	if (node >= maxNodePointer) {
		bad();
		if (report) {
			printf("node* >= maxNodes!!! %p > %p\n", node, maxNodePointer);
			p("OUT OF MEMORY or graph corruption");
//		exit(0);
		}
		return false;
	}
#ifdef useContext
	if (node->context < 0 or node->context > maxContexts) {
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
		if (report)
			pf("nodeId>maxNodes %d>%ld", nodeId, maxNodes);
		return false;
	}
	if (nodeId < -propertySlots) {
		bad();
		if (report)pf("nodeId < -propertySlots %d<%d", nodeId, -propertySlots);
		return false;
	}

	if (nodeId > 1 and node->id > 0 and node->id != nodeId) {
		bad();
		if (report)pf("node->id!=nodeId %d!=%d\n", node->id, nodeId);
		return false;
	}

//	if (node->id == 0) { 0 == "SEE ALSO"
//		bad();
//		if(report)pf("node->id==0 !! \n", node->id, nodeId);
//		return false;
//	}

	if (checkNames and node->name == 0) {// WHY AGAIN??
		bad();
//		if (report)printf("node->name == 0 %i\n", node->id); // todo
		return false;
	}
	if (checkNames and (node->name >= &context->nodeNames[averageNameLength * maxNodes])) {
		bad();
		if (report)printf("node->name out of bounds %i\n", node->id);
		return false;
	}
	if (checkNames and (node->name < context->nodeNames)) {
		bad();
		if (report)printf("node->name out of bounds %i\n", node->id);
		return false;
	}
#ifdef inlineStatements
	if (checkStatements and node->statements == null) { //
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
bool checkNode(int nodeId, bool checkStatements, bool checkNames, bool report) {
	if (nodeId < -propertySlots or nodeId >= maxNodes - propertySlots)return false;
	return checkNode(get(nodeId), nodeId, checkStatements, checkNames, report);
}

Node *add(const char *key, const char *nodeName) {
	N node = add(nodeName);
	insertAbstractHash(wordHash(key), node);
	return node;
}


void checkOutOfMemory() {
	if (context->lastNode <=0) {
		p("lastNode <=0 MEMORY FULL!!!");
		out_of_memory=true;
	}
	if (context->lastNode >= maxNodes - propertySlots) {
		pf("context->lastNode > maxNodes %d>%ld ", context->lastNode, maxNodes);
		p("MEMORY FULL!!!");
		out_of_memory=true;
//					exit(1);
	}
}


Node *add(const char *nodeName, int kind, int contextId) { //=node =current_context
	if (kind < -propertySlots or kind > maxNodes)
		kind = _abstract;// blueprint messup!
#ifndef DEBUG
	if (!nodeName) return 0;
#endif
	Node *abstract = hasWord(nodeName);
	if (abstract and kind == _abstract)
		return abstract;
	Node *node;
	do {
        context->lastNode++;// DON't MOVE!
		node = &(context->nodes[context->lastNode]);
		checkOutOfMemory();
		if(out_of_memory)return Error;
	} while (node->id != 0);

	if (abstract) {
		if (eq(nodeName, abstract->name)) {
			node->name = abstract->name;// save memory
//			nodeName=abstract->name;// wofür?
		} else
			p("HOW NOT?");
	}
	initNode(node, context->lastNode, nodeName, kind, contextId);
	context->nodeCount++;
	if (kind == _abstract or kind == _singleton) return node;
	if (abstract)
		addStatement(abstract, Instance, node, false);// done in initNode//setLabel !
	if (storeTypeExplicitly and kind > 105) // might cause loop?
		addStatement4(contextId, node->id, _Type, kind, false); // store type explicitly!
	//	  why? damit alle Instanzen etc 'gecached' sind und algorithmen einfacher. Beth(kind:person).
	// kosten : Speicher*2
	return node;
}

int getNodeId(long pointer){
	return (int((long)context->nodes-pointer));
}


Node *add_force(int contextId, int id, const char *nodeName, int kind) {
	Context *context = getContext(contextId);
	// pi(context.nodes);// == &context.nodes[0] !!
	if (id > maxNodes) {
		bad();
		if (quiet) return 0;
		printf("int context %d, int id %d id>maxNodes", contextId, id);
		return 0;
	}
#ifdef inlineStatements
	if (context->nodes[id].statements == null)
#endif
	Node *node = context->nodes + id;
	initNode(node, id, nodeName, kind, contextId);
	if (id > 1000)
		context->nodeCount++; // really? add one , otherwise : overwrite
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

Statement *
addStatement4(int contextId, int subjectId, int predicateId, int objectId, bool checkNodes) { //bool checkDuplicate
	if(subjectId==_error || predicateId==_error || objectId==_error )return 0;

	if (contextId < 0 or subjectId < -propertySlots or predicateId < -propertySlots or objectId < -propertySlots) {
		p("WARNING contextId<0 or subjectId < -propertySlots or predicateId < -propertySlots or objectId < -propertySlots");
		return 0;
	}
//	if (contextId < 0 or subjectId < 0 or predicateId < 0 or objectId < 0) {
//		p("WARNING contextId<0 or subjectId<0 or predicateId<0 or objectId<0");
//		return 0;
//	}
	if (contextId > maxContexts or subjectId > maxNodes or predicateId > maxNodes or objectId > maxNodes) {
		bad();
		p("WARNING contextId>maxContexts or subjectId>maxNodes or predicateId>maxNodes or objectId>maxNodes");
		return 0;
	}
	Context *context = getContext(contextId);
	// pi(context.nodes);// == &context.nodes[0] !!
	if (subjectId > maxNodes or predicateId > maxNodes or objectId > maxNodes) {
		if (quiet) return 0;
		printf("int context %d,subjectId %d>maxNodes or predicateId %d>maxNodes or objectId %d>maxNodes ", contextId,
		       subjectId, predicateId,
		       objectId);
		return 0;
	}
	Node *subject = &context->nodes[subjectId];
	Node *predicate = &context->nodes[predicateId];
	Node *object = &context->nodes[objectId];

	//	if (checkDuplicate) {	//todo: add specifications but not generalizations?
	//		Statement* old=findStatement(subject, predicate, object, 0, 0, 0); //,true,true,true);
	//		if (old) return old; // showStatement(old)
	//	}

	if (subject == object) {
		pf("REALLY subject==object?? %s %s %s (%d->%d->%d)\n", subject->name, predicate->name, object->name, object->id,
		   predicate->id, object->id);
		//		return 0;
	}
	//	if(predicate==Antonym)
	//		p("SD");

	if (checkNodes and !checkNode(subject, subjectId)) return 0;
	if (checkNodes and !checkNode(object, objectId)) return 0;
	if (checkNodes and !checkNode(predicate, predicateId)) return 0;

	Statement *s = getStatement(subject->lastStatement);
	if (s and s->subject == subjectId and s->predicate == predicateId and s->object == objectId)
		return s;// quick checkDuplicate!

	int id = context->statementCount;
	Statement *statement = &context->statements[id]; // union of statement, node??? nee
	if (!statement)return 0;
	//	statement->id()=context->statementCount;
	context->statementCount++;
#ifdef explicitNodes
	statement->Subject=subject;
	statement->Predicate=predicate;
	statement->Object=object;
#endif

	statement->subject = subjectId;
	statement->predicate = predicateId;
	statement->object = objectId;
	if (!addStatementToNode(subject, id)) return statement;
	if (!addStatementToNode(object, id)) return statement;
	if (!addStatementToNode(predicate, id)) return statement;

	// predicate->statementCount++;
	// context->nodeCount++;
	return statement;
}

Statement *addStatement(Node *subject, Node *predicate, Node *object, bool checkDuplicate, bool force_insert_at_start) {
	if (!checkNode(subject)) return 0;
	if (!checkNode(object)) return 0;
	if (!checkNode(predicate)) return 0;
	if(subject->id==_error || predicate->id==_error || object->id==_error )return 0;
	Statement *s = getStatement(subject->lastStatement);
	if (s and s->Subject() == subject and s->Predicate() == predicate and s->Object() == object)
		return s;// quick checkDuplicate!

	if (checkDuplicate) {    //todo: add specifications but not generalizations?
		Statement *old = findStatement(subject, predicate, object, 0, 0, 0); //,true,true,true);
		if (old) return old; // showStatement(old)
	}

	if (subject == object and predicate->id < 1000) return 0;// such loops not allowed

	if(predicate->id<0 && predicate!=Instance)
		force_insert_at_start= true;// !
	//	Statement* last_statement=&context->statements[context->statementCount-1];
	//	if(context->statementCount>1000 and last_statement->Subject=subject and last_statement->Predicate=predicate and last_statement->Object=object)
	//		return last_statement;// direct duplicate!

	int id = context->statementCount;
	Statement *statement = &context->statements[id];
	//	statement->id=context->statementCount;
#ifdef useContext
	statement->context=current_context; //todo!!
#endif

#ifdef explicitNodes
	statement->Subject=subject;
	statement->Predicate=predicate;
	statement->Object=object;
#endif

	statement->subject = subject->id;
	statement->predicate = predicate->id;
	statement->object = object->id;

//	if(force_insert_at_start)
	bool ok;
	ok = addStatementToNode(subject, id, force_insert_at_start);
	ok = addStatementToNode(predicate, id) and ok;
	ok = addStatementToNode(object, id) and ok;
	if (!ok)bad("warning: addStatementToNode skipped ");// probably quick duplicate check
	context->statementCount++;
	return statement;
}

// has to be called with i, NOT with n->statements[i]!!
// returns first Statement BEFORE Instance predicate!
// TODO!!! what if x->y->x !?!?!
// TODO firstInstanceGap too complicated, but needed for nodes with 1000000 instances (city etc)
// TODO only used in addStatementToNodeWithInstanceGap !!


// USE ITERATOR!! :::
// Statement* s = 0;int i=0;
// while ((s = nextStatement(n, s)) and i++<=n->statementCount) {
//	if (!checkStatement(s)){bad();continue;}
// USE ITERATOR!! ^^^
// DEPRECATED :
Statement *getStatementNr(Node *n, int nr, bool firstInstanceGap) {
	//	if(nr==0)return 0;// todo ????
//  	if (nr >= maxStatementsPerNode) {
//  		bad();
//  		return null;
//  	}
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
	Context *c = context;
#endif
	Statement *statement = &c->statements[n->firstStatement];
	Statement *laststatement = statement;
	for (int i = 0; i < nr; i++) {
		if (statement == 0) {
			p("CORRUPTED STATEMENTS!");
			show(n);
			bad();
			return null;
		}
		if (!checkStatement(statement, true, false)) break;
		if (i > 0 and firstInstanceGap and statement->Predicate() == Instance) {
			return laststatement;
		}
		if (statement->Subject() == n) {
			statement = &c->statements[statement->nextSubjectStatement];
			continue;
		}
		if (statement->Predicate() == n) {
			statement = &c->statements[statement->nextPredicateStatement];
			continue;
		}
		if (statement->Object() == n) {
			statement = &c->statements[statement->nextObjectStatement];
			continue;
		}
	}
	if (firstInstanceGap and statement->Predicate() == Instance) {
		return laststatement;
	}
	return statement;
}

#ifdef inlineStatements
// has to be called with i, NOT with n->statements[i]!!

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
Node *get(const char *node) {
	return getAbstract(node);
}

//inline
Node *get(char *node) {
	return getAbstract(node);
}


//inline
Node *get(int nodeId) {
	if (debug and (nodeId < -propertySlots)) { // remove when debugged
		if (quiet)return Error;
		bad();
//		printf("Error: nodeId < -propertySlots %d < %d \n", nodeId, -propertySlots);
		return Error;
	}
	if (debug and (nodeId > maxNodes - propertySlots)) { // remove when debugged
		if (quiet)return Error;
		bad();
//		printf("Error: nodeId %d > maxNodes %ld \n", nodeId, maxNodes);
		return Error;
	}
	return &context->nodes[nodeId];
}

//int getId(char *node) {
//	return getAbstract(node)->id;// for blueprint!
//}

Node *getNodeS(int node) {// for blueprint! debug
	return get(node);
}

Node *getNodeP(int node) {// for blueprint! debug
	return get(node);
}

Node *getNode(int node) {
	return get(node);
}


static map<Node *, bool> dissected;

void dissectParent(Node *subject, bool checkDuplicates) {
	if (subject == (Node *) -1) dissected.clear();
	if (!checkNode(subject, -1, false, true)) return;
	//if(isAName(s)ret. // noe!
	string str = replace_all(subject->name, " ", "_");
	str = replace_all(str, "-", "_");


	if (dissected[subject]) return;
	dissected[subject] = true;

	int len = (int) str.length();
	bool plural = (char) str[len - 1] == 's' and (char) str[len - 2] != 's' and
	              ((char) str[len - 2] != 'n' or (char) str[len - 3] == 'o');

	if (!contains(str, "_") and !plural) return;
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
	//    p("dissectWord");
	//    p(subject->name);
	//	if(startsWith(str,"the_")){// the end ... NAH!
	//		addStatement(subject,Synonym,getThe(str.substr(4).data()));
	//		return;
	//	}

	int type = (int) str.find("_");
	if (type < 1) type = (int) str.find(".");
	if (type >= 0 and len - type > 2) {
		string xx = str.substr(type + 1);
		const char *type_name = xx.data();
		Node *word = getAbstract(type_name); //getThe
		dissectParent(word);
		if (!checkNode(word) or !eq(word->name, type_name)) return; // HOW???
		addStatement(word, Instance, subject, false); // true expensive!!! check before!!
		//		addStatement(subject, Type, word, false); // true expensive!!! check before!!
	} else if (plural) {
		const char *singular = str.substr(0, len - 1).c_str();
		Node *word = getAbstract(singular);
		if (!checkNode(word) or !eq(word->name, singular)) return; // HOW???
		addStatement(word, Instance, subject, false); // true expensive!!! check before!!
		dissectParent(word);
	}
}

// Ausnahmen:
//Zugsicherung  Zugbeeinflussungssystem_S-Bahn_Berlin
//Zugunfall    Kesselwagenexplosion_in_der_BASF
//Zugsicherung  Geschwindigkeits�����berwachung_Neigetechnik
//Zugsicherung  Zugsicherung_mit_Linienleiter_1990
//Zuggattung   ICE_International
//Zug_(Stadt)   Hochschule_Luzern_�������_Wirtschaft
//Zug_(Stadt)   Padagogische_Hochschule_Zentralschweiz
//Zug   Fliegender_Hamburger
//Zug   Doppelstock-Stromlinien-Wendezug_der_LBE
//Zeitschriftenverlag   Gruner_+_Jahr
//Zeitschriftenverlag   Smith,_Elder_&_Co.
//Zeitschriftenverlag   Verlag_Neue_Kritik
//Zeitschriftenverlag   Verlag_Otto_Beyer
//Zeitschriftenverlag   Verlag_Technik

Node *dissectWord(Node *subject, bool checkDuplicates) {
	autoIds = false;
	Node *original = subject;
	if (dissected[subject]) return original;
	if (!checkNode(subject, true, true, true)) return original;
	if (subject->statementCount > 1000)checkDuplicates = false;// expansive isA4 !!!!
	//  => todo dissectWord befor loading data!!!!!

	string str = replace_all(subject->name, "_", " ");
	str = replace_all(str, "-", " ");
	//    p("dissectWord");
	//    p(subject->name);
	const char *thing = str.data();
	if (contains(thing, " ") or contains(thing, "_") or contains(thing, "/") or contains(thing, ".") or
	    (endsWith(thing, "s") and !germanLabels))
		dissectParent(subject); // <<

	dissected[subject] = true;

	int len = (int) str.length();
	int type = (int) str.find(",");
	if (type >= 0 and len - type > 2) {
		//		char* t=(str.substr(type + 2) + " " + str.substr(0, type)).data();
		//		Node* word = getThe(t); //deCamel
		//		addStatement(word, Synonym, subject, true);
		Node *a = getThe((str.substr(0, type).data()));
		Node *b = getThe((str.substr(type + 2).data()));
		addStatement(a, Instance, subject, true);
		addStatement(b, Instance, subject, true);
		dissectWord(a, checkDuplicates);
		dissectWord(b, checkDuplicates);
		return original;
		//		str = word->name;
		//    subject=word;
	}
	type = (int) str.find("(");
	if (type > 0 and len - type > 2) {        // not (030)4321643 !
		int to = (int) str.find(")");
		string str2 = str.substr(type + 1, to - type - 1);
		Node *clazz = getThe(str2.data()); //,str.find(")")
		Node *word;
		if (type > 0) word = getThe(str.substr(0, type - 1).data()); //deCamel
		else word = getThe(str.substr(to + 1, len - 1).data()); //deCamel
		addStatement(word, Instance, subject, true);
		addStatement(clazz, Instance, word, true);
		//    addStatement(clazz, Member, word, true);
		addStatement(clazz, Instance, subject, true);
//		addStatement(subject, Instance, clazz, true);
		addStatement(subject, Label, word, true);
		//	  return;
		str = word->name;
		//    subject=word;
	}
	type = (int) str.find(" in ");
	if (type < 0) type = (int) str.find(" am ");
	if (type < 0) type = (int) str.find(" at ");
	if (type >= 0 and len - type > 2) {
		Node *at = the(location);
		Node *word = getThe(str.substr(0, type).data()); //deCamel
		Node *ort = getThe(str.substr(type + 4).data());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(subject, at, ort, checkDuplicates);// or property car_in_blue ! :
		addStatement(subject, Attribute, ort, checkDuplicates);
		addStatement(subject, Type, word, checkDuplicates);
		dissectParent(ort, checkDuplicates);
		return original;
	}
	type = (int) str.find(" from ");
	if (type >= 0 and len - type > 4) {
		Node *from = getThe("from");
		Node *word = getThe(str.substr(0, type).data()); //deCamel
		Node *ort = getThe(str.substr(type + 6).data());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(subject, from, ort, checkDuplicates);
	}
	type = (int) str.find(" for ");
	if (type < 0)type = (int) str.find(" für ");
	if (type >= 0 and len - type > 5) {
		Node *from = getThe("for");
		Node *word = getThe(str.substr(0, type).data()); //deCamel
		Node *obj = getThe(str.substr(type + 5).data());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(subject, from, obj, checkDuplicates);
	}
	type = (int) str.find(" bei ");
	if (type >= 0 and len - type > 2) {
		Node *in = getThe("near");
		//    check(eq(getThe("near")->name,"near"));
		Node *word = getThe(str.substr(0, type).data()); //deCamel
		Node *ort = getThe(str.substr(type + 5).data());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(subject, in, ort, checkDuplicates);
		if (original != subject) addStatement(original, in, ort, checkDuplicates);
		addStatement(subject, the(location), ort, checkDuplicates);
	}

	type = (int) str.find("'s ");
	if (type < 0) type = (int) str.find("s' ");// Oswalds' Cave ?
	if (type >= 0 and len - type > 2) {
		Node *word = getThe(str.substr(0, type).data()); //deCamel
		const char *o = str.substr(type + 4).data();
		Node *ort = getThe(o);
		addStatement(ort, Instance, subject, checkDuplicates);
		addStatement(subject, Member, ort, checkDuplicates);
		//		addStatement(word, Member, ort, checkDuplicates);
		addStatement(word, Instance, subject, checkDuplicates);
	}
	type = (int) str.find(" of ");// board of directors
	if (type < 0) type = (int) str.find(" de "); // de la Casa
	if (type < 0) type = (int) str.find(" du ");
	// della de la del des
	if (type >= 0 and len - type > 2) {
		Node *hat = Member;
		Node *word = getThe(str.substr(0, type).data()); //deCamel
		const char *o = str.substr(type + 4).data();
		Node *ort = getThe(o);
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(ort, hat, subject, checkDuplicates);
		//		addStatement(ort, hat, word, checkDuplicates);
	}
	type = (int) str.find(" der ");
	if (type < 0) type = (int) str.find(" des ");
	if (type < 0) type = (int) str.find(" del ");
	// della
	if (type >= 0 and len - type > 2) {
		Node *hat = Member;
		Node *word = getThe(str.substr(0, type).data()); //deCamel
		string so = str.substr(type + 5); // keep! dont autofree
		Node *ort = getThe(so.data());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(ort, hat, subject, checkDuplicates);
	}
	type = (int) str.find(" von ");
	if (type >= 0 and len - type > 2) {
		Node *hat = Member;
		Node *word = getThe(str.substr(0, type).data()); //deCamel
		string so = str.substr(type + 5); // keep! dont autofree
		Node *ort = getThe(so.data());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(ort, hat, subject, checkDuplicates);
	}
	type = (int) str.find(". ");
	if (type >= 0 and len - type > 2 and isNumber(str.data())) {
		Node *nr = getThe(str.substr(0, type).data()); //deCamel
		Node *word = getThe(str.substr(type + 2).data());
		addStatement(word, Instance, subject, checkDuplicates);
		addStatement(subject, Number, nr, checkDuplicates);
	}
	type = (int) str.find(" ");
	if (type < 0) type = (int) str.find("/");
	if (type >= 0 and len - type > 2) {
		const char *rest = str.substr(type + 1).data();
		if (startsWith(rest, "of "))rest += 3;// ...
		Node *word = getThe(rest);
		addStatement(word, Instance, subject, checkDuplicates);
	}
	return original;
	// todo: zu (ort/name) der (nicht:name) bei von auf der auf am (Angriff )gegen (Schlacht )um...
	//  free(str);
}

bool abstractsLoaded = true;

Node *getNew(const char *thing, Node *type) {
	if (type == Abstract)return getAbstract(thing);// or type==Singleton
	if (type < node_root or type > &node_root[maxNodes]) type = Object;// default parameter hickup through JNA
//	N n=add(getAbstract(thing)->name, type->id);
	N n = add(thing, type->id);
	return n;
}



Node *getSingleton(const char *thing, Node *type, bool addType/*true*/) {
	N there = hasWord(thing);
	if (!there) {
		there = add(thing, _singleton);
		insertAbstractHash(there);
		if (type) {
			if (addType)addStatement(there, get(_Type), type);
			else there->kind = type->id;// danger, fucks up abstract mechanism!
//			check(hasWord(thing)); // ^^^^^^
		}

	}
	return there;
}

Node *getThe(string thing, Node *type) {
	return getThe(thing.data(), type);
}

Node *getThe(const char *thing, Node *type) {//, bool dissect) {
	if (thing == 0 or thing[0] == 0) {
		bad();
		return 0;
	}
	if (autoIds and type != Number and isInteger(thing))return get(atoi(thing));// WAH, not here! --
	Node *rel = getRelation(thing); // not here! doch
	if (rel)return rel;
//  replaceChar((char*)thing,'_',' ');// NOT HERE!
	Node *abstract = getAbstract(thing);
	Node *insta = getThe(abstract, type); // todo: best?
	if (insta and !insta->value.number and atof(thing)) {//!=0 and eq(itoa(atof(thing)),thing)){
		insta->value.number = atof(thing);// hack, shouldn't be here!
		if (!type)insta->kind = _number;
	}
	if (insta) return insta;
	if (type == More)type = 0;
	if (type) insta = add(thing, type->id);
	else insta = add(thing, Object->kind);
	if (insta == 0) {
		p("add node failed!!");
		ps(thing);
		return 0;
	}
	// else if(atoi(thing)!=0)// and eq(itoa(atoi(thing)),thing)){
	//   insta->value.number=atoi(thing);
	//   if(!type)insta->kind=_integer;
	// }
//    object=value(thing,atof(thing),Number);

	//	if (dissect) dissectWord(insta);
	// ^^^^^^^^^^^^^^^^ dont remove! doch! do it elsewo. nee doch hier: einmal für alle! oder am Ende

	return insta;
}



// only for relationships!
// void forceAbstract(Node* n){
//	long h = hash(n->name);
//	Ahash* found = &abstracts[abs(h) % maxNodes]; // TODO: abstract=first word!!! (with new 'next' ptr!)
//	found->abstract=n;
//	found->next=0;
//}

Node *getSeo(const char *thingy) {
//	const char* thingyBug=editable(thingy);
//	N ok= hasWord(thingyBug,true);
	N ok = hasWord(thingy, true);
//	free(&thingyBug); don't free? double free or corruption WHY?
	return ok;
}

bool hasNode(const char *thingy) {
	return hasWord(thingy);
}

// merge with getAbstract bool create
Node *hasWord(const char *thingy, bool seo/*=false*/) {
	if (!thingy or thingy[0] == 0) return 0;
	//	if (thingy[0] == ' ' or thingy[0] == '_' or thingy[0] == '"') // get rid of "'" leading spaces etc!
	//  char* fixed=editable(thingy); // free!!!
	//	thingy=(const char*) fixQuotesAndTrim(fixed);// NOT HERE!
	int h = wordHash(thingy);
	long pos = abs(h) % maxNodes;
	Ahash *found = &abstracts[pos]; // TODO: abstract=first word!!! (with new 'next' ptr!)
	Node *first = 0;
	if (found and found->abstract > 0 and (first = get(found->abstract))) {
		//		if (contains(found->abstract->name, thingy))// get rid of "'" leading spaces etc!
		//			return found->abstract;
		if(first->name > &context->nodeNames[context->currentNameSlot] || first->name<=context->nodeNames){
//			p("BAD NAME");// deleted
			bad(); // todo
			return 0;
		}
		if (eq(first->name, thingy, true))    // tolower
			return first;
		if (seo and first->name) {
			if (eq(generateSEOUrl(first->name), thingy))
				return first;
		}
	}
	int tries = 0; // cycle bugs

	//  	map<Node*, bool> visited;
//#ifdef DEBUG
//	map<int, bool> visited;// relatively EXPENSIVE!!
//#endif
	//	map<Ahash*, bool> visited;
	// ./clear-shared-memory.sh After changing anything here!!
	while (found >= abstracts and found < &extrahash[maxNodes]) {
		if (tries++ > 1000) {
//			p("cycle bug");
			bad();
			break;
		}
//#ifdef DEBUG
//		if (visited[found->abstract] == 1) {// Only happens after messing with full memory
//			debugAhash(h);
//			p("visited[found] == 1 How the hell can that even be??? "); clear
//			p(found->abstract);
//			return 0;
//		}
//		visited[found->abstract]=1;
//#endif
		if (found and checkNode(found->abstract)) {
			//			if (contains(found->abstract->name, thingy))//contains enough ?? 0%Likelihood of mismatch?
			//				return found->abstract;
			N a = get(found->abstract);
			if(!checkNode(a,0,0,1))continue;// check name
			if (eq(a->name, thingy, true, true))            //teuer? n��, if 1.letter differs
				return a;
			if (seo and a->name) {
				if (eq(generateSEOUrl(a->name), thingy))
					return a;
			}
		}
		//		if (get(found->next) == found) {
		//    debugAhash(h);
		//			p("found->next == found How the hell can that even be? ");
		//			break;
		//		}
		if (!found or !found->next)break;
		if (found->next <= 0 or found->next > maxNodes * 2 or found == &abstracts[found->next])break;
		found = &abstracts[found->next];
	}
//	p("/////////////");
	return 0;
}

Node *number(int nr) {
	bool tmp = autoIds;
	autoIds = false;
	N n = getSingleton(itoa(nr).data());
	autoIds = tmp;
	return n;
}

extern "C" int nodeCount() {
	return context->nodeCount;
}extern "C" int statementCount() {
	return context->statementCount;
}
extern "C" int nextId() {
	return context->nodeCount++;
}

Node *dateValue(const char *val) {
	Node *n = getAbstract(val);// getThe(val);
	n->kind = Date->id;
//	n->value = getdate(name) std:get_time ?
//	n->value == 	char *	"1732-02-22\""
	return n;
	//	return value(val, atoi(val), Date);
}

/*
 <g.11vjx3759>  <measurement_unit.dated_percentage.source>   <g.11x1gf2m6>  .
 <g.11vjx3759>  <#type> <measurement_unit.dated_percentage>   .
 <g.11vjx3759>  <measurement_unit.dated_percentage.date>    "2005-04"^^<#gYearMonth>    .
 <g.11vjx3759>  <measurement_unit.dated_percentage.rate>    4.5   .
 <g.11vjx3759>  <#type> <measurement_unit.dated_percentage>   .
 */
Node *rdfValue(char *name) {
	if (name[0] == '"') name++; // ignore quotes "33"
	char *unit0 = strstr(name, "^");
	if (!unit0 or unit0 > name + 1000 or unit0 < name) return 0;
	if (unit0[-1] == '"')unit0[-1] = 0;
	while (unit0[0] == '^') {
		unit0[0] = 0;
		unit0++;
	}
	if (name[0] == 0)return 0;
	if (unit0[0] == '<') unit0++;
	if (unit0[0] == '#') unit0++;
	if (unit0[0] == '"') unit0++;
	if (startsWith(unit0, "http"))
		unit0 = dropUrl(unit0);// km/s OK!
	const char *unit = unit0;
//	if (eq(unit, ",)")) return 0; // LOL_(^^,) BUG!
	if (eq(unit, "xsd:integer")) unit = 0; //-> number
	else if (eq(unit, "integer")) unit = 0; //-> number
	else if (eq(unit, "int")) unit = 0; //-> number
	else if (eq(unit, "double")) unit = 0; //-> number
	else if (eq(unit, "decimal")) unit = 0; //-> number return value(key, atof(key), Number);; //-> number
	else if (eq(unit, "float")) unit = 0; //-> number
	else if (eq(unit, "nonNegativeInteger")) unit = 0; //-> number
	else if (eq(unit, "yago0to100")) unit = 0;
	if (!unit) return value(name, atof(name), Number);// unit==0 means number, ignore extra chars 123\"

	if (eq(unit, "m")) unit = "Meter";
	else if (eq(unit, "%")); // OK
	else if (eq(unit, "s")) unit = "Seconds";
	else if (eq(unit, "second")) unit = "Seconds";
	else if (eq(unit, "r")) unit = "Seconds";
	else if (eq(unit, "/km")) unit = "Kilometer";
	else if (eq(unit, "km")) unit = "Kilometer";
	else if (eq(unit, "kilometre")) unit = "Kilometer";
	else if (eq(unit, "millimetre")) unit = "mm";
	else if (eq(unit, "centimetre"));
	else if (eq(unit, "meter"));
	else if (eq(unit, "tonne"));
	else if (eq(unit, "volt"));
	else if (eq(unit, "g")) unit = "Gram";
	else if (eq(unit, "gram")) unit = "Gram";
	else if (eq(unit, "kilogram")) unit = "kg";
	else if (eq(unit, "kilogramPerCubicMetre")) unit = "kg/m^3";
	else if (eq(unit, "milligram"));
	else if (eq(unit, "hectopascal"));
	else if (eq(unit, "kilowatt"));
	else if (eq(unit, "byte"));
	else if (eq(unit, "knot"));
	else if (eq(unit, "litre"));
	else if (eq(unit, "bar"));
	else if (eq(unit, "kilonewton"));
	else if (eq(unit, "megawatt"));
	else if (eq(unit, "squareMetre"))unit = "m^2";
	else if (eq(unit, "kilometrePerHour"))unit = "km/h";
	else if (eq(unit, "xsd:date")); // parse! unit = 0; //-> number
	else if (eq(unit, "kelvin")); // ignore
	else if (eq(unit, "degreeCelsius")) unit = "C"; // ignore
	else if (eq(unit, "degreeFahrenheit")) unit = "F"; // ignore
	else if (eq(unit, "degreeRankine")) unit = "R"; // ignore
	else if (eq(unit, "degrees")); // ignore
	else if (eq(unit, "dollar")); // ignore
	else if (eq(unit, "usDollar"))unit = "dollar"; // ignore
	else if (eq(unit, "euro")); // ignore
	else if (eq(unit, "squareKilometre"))unit = "km^2"; // ignore
	else if (eq(unit, "megabyte")); // ignore
	else if (eq(unit, "gramPerCubicCentimetre"))unit = "g/cm^3"; // ignore
	else if (eq(unit, "metrePerSecond"))unit = "m/s"; // ignore
	else if (eq(unit, "kilometrePerSecond"))unit = "km/s"; // ignore


	else if (eq(unit, "yagoISBN")) unit = "ISBN"; // ignore
	else if (eq(unit, "yagoTLD")) unit = "TLD"; // ???
	else if (eq(unit, "yagoMonetaryValue")) unit = "dollar";// USD $
	else if (eq(unit, "gYear")) unit = "year"; //Date;
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
	Node *unity;
	if (unit[0] == 'Q')
		unity = get(atoi(unit + 1));
	else unity = getThe(unit);
	return value(name, atof(name), unity);
}

// Abstract nodes are necessary in cases where it is not known whether it is the noun/verb etc.
extern "C"
Node *getAbstract(const char *thing) {            // AND CREATE! use hasWord for lookup!!!
	if (thing == 0 or strlen(thing) == 0) {// or not is
		bad();
		return 0;
	}
	while (thing[0] == ' ' or thing[0] == '"')thing++;
	if (contains(thing, "^^"))// NOT HERE!
		return rdfValue(modifyConstChar(thing));
	if (autoIds and isInteger(thing))return get(atoi(thing));
	Node *abstract = hasWord(thing);
	if (abstract) {
//		if(abstract->kind!=_abstract and abstract->kind!=_singleton)
//			pf("HOW is %d not abstract?\n",abstract->id);
		return abstract;
	}
	abstract = add(thing, _abstract, _abstract); // abstract context !!
	if (!abstract) {
		p("out of memory!");
		//		exit(0);
		//		throw "out of memory exception";
		return 0;
	}
	Ahash *ok = insertAbstractHash(wordHash(thing), abstract);
	//	if (ok == 0) insertAbstractHash(wordHash(thing), abstract);		// debug
	if (ok == 0){bad();return Error;}// full!
	if (autoDissectAbstracts and (contains(thing, "_") or contains(thing, " ") or contains(thing, ".")))
		dissectParent(abstract);// later! else MESS!?
	//	else dissectAbstracts(am Ende)
	//	collectAbstractInstances(abstract am Ende);
	return abstract;
}

Node *getAbstract(string thing) {
	return getAbstract(thing.c_str());
}

Node *getAbstract(Node *node) {
	return getAbstract(node->name);
}

void collectAbstractInstances(Node *abstract) {
	Context *c = getContext(current_context);
	for (int i = 0; i < c->nodeCount; i++) {
		Node *n = &c->nodes[i];
		char *nname = n->name;
		if (eq(abstract->name, nname))
			addStatement(abstract, Instance, n);
	}
}
//Node* getThe(const char* word){
//  Node* n=find(current_context,word,true);
//  if(n==0)n=add(word);
//  return n;
//}

void showStatement(int id) {
	showStatement(getStatement(id));
}

void showStatement(Statement *s) {
	//	if (quiet)return;
	Context *c = context;
	if (s < c->statements or s > &c->statements[maxStatements]) {
		if (quiet) return;
		p("illegal statement:");
		printf("%p", s);
		return;
	}
	if (s == null) return;
	if (s->subject == 0 and s->predicate == 0 and s->object == 0)return;//null / deleted

	if (checkNode(s->Subject(), s->subject) and checkNode(s->Predicate(), s->predicate) and
	    checkNode(s->Object(), s->object))
		//    if(s->Object->value.number)
		//      printf("%d\t%s\t\t%s\t\t%g %s\t%d\t%d\t%d\n", s->id, s->Subject->name, s->Predicate->name, s->Object->value.number,s->Object->name, s->subject, s->predicate, s->object);
		//    else
		printf("$%d :\t%s\t\t%s\t\t%s\t\t%d⇛%d⇛%d\n", s->id(), s->Subject()->name, s->Predicate()->name,
		       s->Object()->name, s->subject, s->predicate,
		       s->object);

	else printf("$%d %d->%d->%d [%p]\n", s->id(), s->subject, s->predicate, s->object, s);
	flush();
	// printf("%s->%s->%s\n",s->Subject->name,s->Predicate->name,s->Object->name);

}

//, bool showAbstract
extern "C" const char *getName(int node) {
	if (!checkNode(node, false, true))return "<ERROR>";//_IN_GET_NAME>";//
	return get(node)->name;
}

char *getLabel(Node *n) {
	Context *context = getContext(current_context);
	if (n->value.text > context->nodeNames and n->value.text < context->nodeNames + context->currentNameSlot)
		return n->value.text;
	Statement *s = findStatement(n, Label, Any, 0, false, false);
	if (s) return s->Object()->name;
	return 0;
}

void show(NodeSet &all) {
	NodeSet::iterator it;
	for (it = all.begin(); it != all.end(); it++) {
		Node *node = (Node *) *it;
		if (!checkNode(node))continue;
//		pf("%d	%s\n",node->id,node->name);
		pf("%s	Q%d\n", node->name, node->id);
	}
}

void show(vector<char *> &v) {
	for (int i = 0; i < v.size(); i++) {
		p(v.at(i));
	}
}

void show(Statement *s) {
	showStatement(s);
}

bool show(Node *n, bool showStatements) {        //=true
	//	if (quiet)return;
	if (!checkNode(n)) return 0;
	if (n->statementCount <= 1) {// x->instance->x
		//    pf("%d|",n->id);
		//    return false;// !!! HIDE!!!
	}
	// Context* c=getContext(n->context);
	// if(c != null and c->name!=null)
	// printf("Node: context:%s#%d id=%d name=%s statementCount=%d\n",c->name, c->id,n->id,n->name,n->statementCount);
	//  printf("%s (#%d)\n", n->name, n->id);
	string img = "";
	cchar *text = "";//getText(n);
//	bool showLabel=true;//false;//!debug; getLabel(n);
	//	if (showLabel and n->name) img=getImage(n->name); EVIL!!!
	//  if(n->value.number)
	//  printf("%d\t%g %s\t%s\n", n->id,n->value.number, n->name, img.data());
	//  else
	//		printf("Node#%p: context:%d id=%d name=%s statementCount=%d kind=%d\n",n,n->context,n->id,n->name,n->statementCount,n->kind);
	//		printf("%d\t%s\t%s\t%s\t(%p)\n", n->id, n->name,text, img.data(),n);
	if (!text or isAbstract(n) or isValue(n) or strlen(text) == 0)
		printf("%d\t%s\t\t(%d statements)\n", n->id, n->name, n->statementCount);//img.data(),
	else
		printf("%d\t%s\t\t%s\t(%d statements)\n", n->id, n->name, text, n->statementCount);//img.data(),
//  if(n->statementCount<=1)return false;
	//	printf("%s\t\t(#%d)\t%s\n", n->name, n->id, img.data());
	// else
	// printf("Node: id=%d name=%s statementCount=%d\n",n->id,n->name,n->statementCount);
	int i = 0;
	//	int maxShowStatements=40; //hm
	if (showStatements) {
		Statement *s = 0;
		while ((s = nextStatement(n, s))) {
			if (i++ >= resultLimit) break;
			if (checkStatement(s)) showStatement(s);
			else pf("BROKEN STATEMENT: %p\n", s);// break?
		}
		printf("-----------------------^ %s #%d (kind: %s #%d), %d statements --- %s ^---------------\n", n->name,
		       n->id, get(n->kind)->name, n->kind, n->statementCount, text);
		flush();
	}
	return 1; // daisy
}

bool showShort(Node *n) {
	return show(n, false);
}

//extern "C"
Node *showNode(Node *n) {
	show(n);
	return n;
}

Node *showNode(int id) {
	if(id<=-propertySlots || id>maxNodes)
		return bad();
	Node *n = &getContext()->nodes[id];// &context->nodes[id];
	if (!checkNode(n, id)) return 0;
	show(n);
	return n;
}

// saver than iterating through abstracts?
NodeVector *findWordsByName(int context, const char *word, bool first, bool containsWord) {    //=false
	// pi(context);
	NodeVector *all = new NodeVector();
	Context *c = getContext(context);
	for (int i = -propertySlots; i < c->nodeCount; i++) {
		Node *n = &c->nodes[i];
		if (n->id == 0 or !checkNode(n, i, true, false)) continue;
		bool good = eq(n->name, word, true);
		if (containsWord)
			good = good or contains(n->name, word, true);// 1000000000*100 comparisons!?!
		if (good) {
			all->push_back(n);
			show(n);
			Node *abstract = hasWord(word);
			if(!abstract) // & fix / associate
				insertAbstractHash(n,true); // or leave to debug!
			else
				if(eq(abstract->name,n->name))
					addStatement(abstract,Instance,n,true);
			if (first) return all;
		}
	}
	if (all->size() == 0)
		if (!quiet) printf("cant find node %s in context %d\n", word, context);
//	if(associate){
//	N a=getAbstract(word);
//	NV insts=instanceFilter(a);// expensive!!!
//	for (int i=0; i < all->size(); i++){
//		Node *n=all->at(i);
//		if(!contains(insts,n))addStatement(a,Instance,n);
//	}
//}
	return all;
}

// see findAll for recursive children of subclasses / instances
NodeVector *findAllWords(const char *word) {
	NodeVector *all = findWordsByName(current_context, word,/*first*/false,/*containsWord*/false);
	collectAbstractInstances(getAbstract(word));// woot?
	return all;
} // diff :??
NodeVector *findAllMatches(const char *word) {
	return findWordsByName(current_context, word, false, true);
}

Statement *findStatement(int subject, int predicate, int object, int recurse, bool semantic, bool symmetric,
                         bool semanticPredicate, bool matchName) {
	return findStatement(get(subject), get(predicate), get(object), recurse, semantic, symmetric, semanticPredicate,
	                     matchName);
}

bool _trace= false;
void trace(){
	_trace=true;
}

// DO  NOT	TOUCH	A	SINGLE	LINE	IN	THIS	ALGORITHM	!!!!!!!!!!!!!!!!!!!!
Statement *findStatement(Node *subject, Node *predicate, Node *object,
                         int recurse, bool semantic, bool symmetric, bool semanticPredicate, bool matchName,
                         int limit) {
	// DO  NOT	TOUCH	A	SINGLE	LINE	IN	THIS	ALGORITHM	!!!!!!!!!!!!!!!!!!!!
	if(!predicate)return 0;//predicate=Any;// 12/2018  ... or return 0?
	if (recurse > 0) recurse++;
	else recurse = maxRecursions;
	if (recurse > maxRecursions or subject == 0) return 0;
	if (limit<=0)limit=queryLimit;

	Statement *s = 0;
	map<Statement *, bool> visited;
	int lookup = 0;
	bool stopAtInstances = false;// predicate != Instance; too Fragile for ordering!
	while ((s = nextStatement(subject, s, stopAtInstances))) { // kf predicate!=Any 24.1. REALLY??
		if (limit and lookup++ > limit)break;
		if(_trace)
			p(s);
		if (visited[s]) {// Remove in live mode if all bugs are fixed
			p("GRAPH ERROR: cyclic statement");
			p(s);
			bad();
			return 0;
		}
		visited[s] = 1;
		if (!checkStatement(s)) continue;
		if (s->context == _pattern) continue;

		//		if(s->Predicate!=Any){
		if (s->Object() == Adjective and object != Adjective) continue; // bug !!
		if (s->Predicate() == Derived) continue; // Derived bug !!
		if (s->Predicate() == get(_attribute)) continue; // bug !!
		if (s->Predicate() == get(_see_also)) continue; // see 'also' wordnet bug !!
		if (s->Predicate() == get(_DOMAIN_CATEGORY)) continue;
		if (s->Predicate() == get(_DOMAIN_BUG)) continue;
		//		}
		// ^^ todo

		if (subject == s->Predicate()) {
//      ps("NO predicate statements!");
			continue;
		}

		//    if(s->context != current_context)continue;// only queryContext
#ifdef use_instance_gap
		if (s->Predicate == subject or i > 1 and s->Predicate == Instance and predicate != Instance or i > 1 and s->Predicate == Type and predicate != Type) {
	  ps("skipping Predicate/Instance/Kind statements");
			      continue;
//			break;// todo : make sure statements are ordered!
		}
#endif

		if (s->Predicate() == Instance and predicate != Instance and predicate != Any) continue; // return 0; // DANGER!
		//		NOT COMPATIBLE WITH DISSECTED WORDS!!!!! PUT TO END!!!
		if (predicate == Any and eq(s->Subject()->name, s->Object()->name))continue;

		// DO  NOT	TOUCH	A	SINGLE	LINE	IN	THIS	ALGORITHM	!!!!!!!!!!!!!!!!!!!!
		bool subjectMatch = s->Subject() == subject or subject == Any or
		                    isA4(s->Subject(), subject, false, false); //DONT CHANGE quick
		subjectMatch = subjectMatch or (matchName and eq(s->Subject()->name, subject->name));
		bool predicateMatch = (s->Predicate() == predicate or predicate == Any);
		predicateMatch = predicateMatch or (predicate == Instance and s->Predicate() == SubClass);
		predicateMatch = predicateMatch or (predicate == SubClass and s->Predicate() == Instance);
		predicateMatch = predicateMatch or isA4(s->Predicate(), predicate, false, false);
		predicateMatch =
				predicateMatch or ((matchName or semanticPredicate) and eq(s->Predicate()->name, predicate->name));
		bool objectMatch = s->Object() == object or object == Any or object->id == 0 or
		                   isA4(s->Object(), object, false, false);// by name OK!
		objectMatch = objectMatch or (matchName and eq(s->Object()->name, object->name));

		if (subjectMatch and predicateMatch and objectMatch) {
			return s;// GOT ONE!
		}

		// READ BACKWARDS
		// OR<-PR<-SR
		bool subjectMatchReverse = subject == s->Object() or subject == Any or isA4(s->Object(), subject, false, false);
		subjectMatchReverse = subjectMatchReverse or (matchName and eq(subject->name, s->Object()->name));
		bool objectMatchReverse = object == s->Subject() or object == Any or isA4(s->Subject(), object, false, false);
		objectMatchReverse = objectMatchReverse or (matchName and eq(object->name, s->Subject()->name));
		bool predicateMatchReverse = predicate == Any; // or inverse
		symmetric = symmetric or s->Predicate() == Synonym or predicate == Synonym or s->Predicate() == Antonym or
		            predicate == Antonym;
		symmetric = symmetric and !(s->Predicate() == Instance); // todo : ^^ + more
		// todo: use inverse(predicate)
		predicateMatchReverse = predicateMatchReverse or (predicate == Instance and s->Predicate() == Type);
		predicateMatchReverse = predicateMatchReverse or (predicate == Type and s->Predicate() == Instance);
		predicateMatchReverse = predicateMatchReverse or (predicate == SuperClass and s->Predicate() == SubClass);
		predicateMatchReverse = predicateMatchReverse or (predicate == SubClass and s->Predicate() == SuperClass);
		predicateMatchReverse = predicateMatchReverse or (predicate == Antonym and s->Predicate() == Antonym);
		predicateMatchReverse = predicateMatchReverse or (predicate == Synonym and s->Predicate() == Synonym);
//		predicateMatchReverse=predicateMatchReverse or (predicate == Translation and s->Predicate() == Translation);
		predicateMatchReverse = predicateMatchReverse or predicate == Any;
		predicateMatchReverse = predicateMatchReverse or (predicateMatch and symmetric);
		// DO  NOT	TOUCH	A	SINGLE	LINE	IN	THIS	ALGORITHM	!!!!!!!!!!!!!!!!!!!!

		//		predicateMatchReverse = predicateMatchReverse or predicate == invert(s->Predicate);// invert properties ?? NAH!!
		//		predicateMatchReverse = predicateMatchReverse or invert(predicate) == s->Predicate;// invert properties ?? NAH!!
		// sick:
		//    predicateMatchReverse = predicateMatchReverse or predicate == Instance and s->Predicate == SuperClass;
		//    predicateMatchReverse = predicateMatchReverse or predicate == SuperClass and s->Predicate == Instance;
		//    predicateMatchReverse = predicateMatchReverse or predicate == SubClass and s->Predicate == Type;
		//    predicateMatchReverse = predicateMatchReverse or predicate == Type and s->Predicate == SubClass;
		if (subjectMatchReverse and predicateMatchReverse and objectMatchReverse) {
			return s;// GOT ONE!
		}

		if (!semantic) continue;
		///////////////////////// SEMANTIC /////////////////////////////
		// DO  NOT	TOUCH	A	SINGLE	LINE	IN	THIS	ALGORITHM
		subjectMatch = subjectMatch or (semantic and isA4(s->Subject(), subject, recurse, semantic));
		if (subjectMatch) objectMatch = objectMatch or (semantic and isA4(s->Object(), object, recurse, semantic));
		if ((subjectMatch and objectMatch) or symmetric) {
			if (semanticPredicate)
				predicateMatch = predicateMatch or isA4(s->Predicate(), predicate, recurse, semantic);
			else
				predicateMatch = predicateMatch or eq(s->Predicate()->name, predicate->name) or
				                 isA4(s->Predicate(), predicate, false, false);
		}
		if (subjectMatch and predicateMatch and objectMatch) {
			return s;// GOT ONE!
		}
		// DO  NOT	TOUCH	A	SINGLE	LINE	IN	THIS	ALGORITHM	!!!!!!!!!!!!!!!!!!!!
		predicateMatchReverse = predicateMatchReverse or (symmetric and predicateMatch);
		if (predicateMatchReverse) {
			subjectMatchReverse = subjectMatchReverse or isA4(s->Object(), subject, recurse, semantic);
			objectMatchReverse = objectMatchReverse or isA4(s->Subject(), object, recurse, semantic);
		}
		if (subjectMatchReverse and predicateMatchReverse and objectMatchReverse) {
			return s;// GOT ONE!
		}
		///////////////////////// END SEMANTIC /////////////////////////////
	}
	return null;
}

void removeStatement(Node *n, Statement *s) {
	if (!n or !s) return;
	Statement *last = 0;
	Statement *st = 0;
	while ((st = nextStatement(n, st))) { // FAST OK??
//	for (int i=0; i < n->statementCount; i++) { // SAVE but slow
//  	Statement* st=getStatementNr(n, i);
		if (st == s) {
			if (last == 0) {
				if (s->Subject() == n) n->firstStatement = s->nextSubjectStatement;
				if (s->Predicate() == n) n->firstStatement = s->nextPredicateStatement;
				if (s->Object() == n) n->firstStatement = s->nextObjectStatement;
			} else {
				if (s->Subject() == n) last->nextSubjectStatement = s->nextSubjectStatement;
				if (s->Predicate() == n) last->nextPredicateStatement = s->nextPredicateStatement;
				if (s->Object() == n) last->nextObjectStatement = s->nextObjectStatement;
			}
		}
		last = st;
	}
}

void deleteStatement(int id) {
	deleteStatement(getStatement(id));
}

// Does NOT delete tautological duplicates!
void deleteStatement(Statement *s) {
	pf("deleteStatement %d\n",s->id());
	p(s);
//	if (!checkStatement(s, true, false)) {
//		printf("BAD Statement:");
//		bad();
//		return;
//	}
	removeStatement(s->Subject(), s);
//	can be very expensive a->is->b 10000000 is Statements :
//	removeStatement(s->Predicate(), s);// Keep '//' in mind: not real 'bug' if Statement is broken
	removeStatement(s->Object(), s);
	s->Subject()->statementCount--;
	s->Predicate()->statementCount--;
	s->Object()->statementCount--;
	//	context->statements[s->id]=0;
	p("DELETING:");
	p(s);
	memset(s, 0, sizeof(Statement));
}

void deleteWord(const char *data, bool completely) {
	Context *context = &contexts[current_context];
	if (data[0] == '$') {
		pf("deleteStatement %s \n", data + 1);
		Statement *s = &context->statements[atoi(data + 1)];
		p(s);
		deleteStatement(s);
		return;
	}
	pf("deleteWord %s \n", data);
	int id = atoi(data);
	if (id <= 0) {
//		deleteNode(getThe(data));
		deleteNode(get(data), completely);
		if (completely) {
			NodeVector *words = findWordsByName(current_context, data, true, false); //getAbstract(data);
			for (int i = 0; i < words->size(); i++) {
				Node *word = words->at(i);
				pf("deleteNode %s \n", word->name);
				deleteNode(word, false); // DANGER!!
			}
		}
	}
	else if (checkNode(&context->nodes[id], id, false, false))
		deleteNode(&context->nodes[id], completely);
	else if (checkStatement(&context->statements[id], false, false))
		deleteStatement(&context->statements[id]);
	else ps("No such node or statement: " + string(data));
}

void deleteWord(string *s) {
	remove(s->c_str());
}

void deleteNode(int id) {
	deleteNode(get(id), false);
}

void deleteNode(Node *n, bool deleteChildren) {
// 2017-02-30 : STILL EVIL! TEST!!
	if (!checkNode(n)) return;
	if (n->kind == _abstract and deleteChildren) {
		NodeVector nv = instanceFilter(n);
		for (int i = 0; i < nv.size(); i++) {
			Node *child = nv[i];
			if (!isAbstract(child) and eq(n->name,child->name))
				deleteNode(child, false);
		}
	} else { //!!
		N a = getAbstract(n->name);
		if (a and a->value.node == n)
			a->value.node = 0;
	}
	deleteStatements(n);
	memset(n, 0, sizeof(Node)); // hole in context!  KF 2.11.17 reactived
}

void deleteStatements(Node *n) {
	Statement *s = 0;
	Statement *previous = 0;
	while ((s = nextStatement(n, s))) {
		if (previous)deleteStatement(previous);
		previous = s;
	}
	if (previous)deleteStatement(previous);// last one
	//	for (int i=0; i < minimum(n->statementCount, 10000); i++) {
	//		Statement* s=getStatementNr(n, i);
	//		deleteStatement(s);
	//	}
	p(n);
	n->statementCount = 0;
	n->firstStatement = 0;
	n->lastStatement = 0;
}

Node *parseValue(const char *aname) {
	if (contains(aname, " ")) {
		string s = (aname);
		string unit = s.substr(s.find(" ") + 1);
		return value(aname, atof(aname), unit.data());
	} else return value(aname, atof(aname), Number);//_number
}

//#include <stdlib.h>
//#include <math.h> //floor


cchar *shortName(cchar *unit) {
	if (unit == 0 or unit[0] == 0)
		return "";
	if (eq(unit, "kilometre")) unit = "km";
	else if (eq(unit, "millimetre")) unit = "mm";
	else if (eq(unit, "centimetre")) unit = "cm";
	else if (eq(unit, "meter"))unit = "m";
	else if (eq(unit, "tonne"))unit = "cm";
	else if (eq(unit, "volt"))unit = "V";
	else if (eq(unit, "gram")) unit = "g";
	else if (eq(unit, "kilogram")) unit = "kg";
	else if (eq(unit, "Contains")) unit = " ";
	else if (eq(unit, "number")) unit = " ";
	return unit;
}

int valueId(const char *aname, double v, int unit) {
	return value(aname, v, get(unit))->id;
}

Node *value(const char *aname, double v, const char *unit) {
	return value(aname, v, getThe(unit));
}

Node *value(const char *name, double v, Node *unit/*kind*/) {
	char *new_name = 0;

	if (name == 0 or strlen(name) == 0) {
		new_name = (char *) malloc(1000);// no todo: name_root done in getThe()
		if (unit == Number or unit == Integer or unit == 0) {// double / long
			sprintf(new_name, "%g", v); //Use the shorter of %e or %f 3.14 or 24E+35
		} else if (unit and unit != Bytes and unit->name) {
			if (v > 1000000000 or v < 1000)
				sprintf(new_name, "%g %s", v, shortName(unit->name)); //Use the shorter of %e or %f 3.14 or 24E+35
			else
				sprintf(new_name, "%d %s", (int) v, shortName(unit->name)); // round
		}
		name = new_name;
	}
	Node *n;
	if (!unit or (unit->id < 1000 and unit != Number))
		n = getThe(name); // 69 (year vs number!!)
	else
		n = getThe(name, unit);// 45cm != 45Volt !!!
	if (unit)n->kind = unit->id;
	else n->kind = _number;
	n->value.number = v;
//  if(unit)addStatement(unit, Instance, n);// NO, getThe !!
	//  	n->value=v;
	if (new_name)free(new_name);
	return n;
}

extern "C" void saveData(int node, void *data, int size, bool copy) {
	N n = getNode(node);
	if (!copy) {
		n->value.data = data;
		return;
	}
	void *target = &context->nodeNames[context->currentNameSlot];

	memcpy(target, data, size);
	n->value.data = target;// data
	context->currentNameSlot += size + 1;
}

extern "C" void *getData(int node) {
	N n = getNode(node);
	return n->value.data;
}

extern "C" Payload getValue(int node) {
	N n = getNode(node);
	return n->value;
}


Node *has(const char *n, const char *m) {
	return has(getAbstract(n), getAbstract(m));
}

Node *has(Node *n, string predicate, string object, int recurse, bool semantic, bool symmetric) {
	return has(n, getAbstract(predicate.data()), getAbstract(object.data()), recurse, semantic, symmetric, true);
}

Node *
has(Node *subject, Node *predicate, Node *object, int recurse, bool semantic, bool symmetric, bool predicatesemantic,
    bool matchName) {

	if (recurse > 0) recurse++;
	else recurse = maxRecursions - 1;
	if (recurse > maxRecursions)
		return 0;

	Statement *s = findStatement(subject, predicate, object, recurse, semantic, symmetric, predicatesemantic,
	                             matchName);
//	if(!s)s=findStatement(subject, predicate, object);
	if (s != null and s->Subject() == subject) return s->Object(); // a.b=*? return *
	if (s != null and s->Object() == subject) return s->Subject();

	if (recurse <= 2 and subject->kind == Abstract->id) {
		NodeVector all = instanceFilter(subject);// need big lookuplimit here? :( todo: filter onthe fly!
		for (int i = 0; i < all.size(); i++) {
			Node *insta = (Node *) all[i];
			Node *resu = has(insta, predicate, object, recurse, semantic, symmetric, predicatesemantic, matchName);
			if (resu) return resu;
		}
	} else if (recurse > 0) {
		NodeVector all = parentFilter(subject);
		for (int i = 0; i < all.size(); i++) {
			Node *insta = (Node *) all[i];
			if (yetvisited[insta]) continue;
			Node *resu = has(insta, predicate, object, recurse, semantic, symmetric, predicatesemantic, matchName);
			//			yetvisited[insta]=true;
			if (resu) return resu;
		}
	}

	//  if(recurse>maxRecursions/3)semantic = false;
	//  printf("findStatement(n %d,predicate %d,object %d,recurse %d,semantic %d,symmetric %d)\n",n,predicate,object,recurse,semantic,symmetric);

	return 0;
}

bool isEqual(Node *subject, Node *object) {
	if(!subject and !object)return true;
	if(!subject or !object)return false;
	if (subject->value.number and subject->value.number == object->value.number) return subject;
	if (atof(subject->name) > 0 and atof(subject->name) == atof(object->name)) return subject;
	if (isA4(subject, object))
		return subject;
	return 0;
}

Node *isGreater(Node *subject, Node *object) {
	//      if(subject->kind!=object->kind)return 0;
	double v = subject->value.number;
	if (!v)v = atof(subject->name);
	double w = object->value.number;
	if (!v)v = atof(object->name);
	if (subject->kind != 0 and subject->kind == object->kind and v > w) return subject;
	if (v > w) return subject;// todo !!
	return 0;
}

Node *isLess(Node *subject, Node *object) {
	//      if(subject->kind!=object->kind)return 0;
	double v = subject->value.number;
	if (!v)v = atof(subject->name);
	double w = object->value.number;
	if (!v)v = atof(object->name);
	if (subject->kind == object->kind and v < w) return subject;
	if (v and w and v < w) return subject;

	return 0;
}

Node *isAproxymately(Node *subject, Node *object) {
	if (isEqual(subject, object)) return subject;
	//  if(soundex(subject->name)==soundex(object->name))return subject;
	//  if(levin_distance(subject->name,object->name)<3)return subject;
	if (object->value.number == 0 and subject->value.number * subject->value.number > 0.1) return 0;
	float r = subject->value.number / object->value.number;
	if (r * r > 0.9 and r * r < 1.1) return subject;

	return 0;
}

// berlin (size greater 200 sqm)/*float*/

Node *has(Node *subject, Statement *match, int recurse, bool semantic, bool symmetric, bool predicatesemantic) {
	//  if(findStatement(match))
	//  if (match->Predicate == Equals)return has(subject, match->Subject, match->Object);

	//todo : iterate all property_values !!
	Node *property_value = has(subject, match->Subject(), Any, recurse, semantic, symmetric, predicatesemantic);
	//has(subject, match->Subject);
	//  if (!property_value)property_value = has(subject, match->Subject);// second try expensive!
	if (!property_value) return 0;
	if (match->Predicate() == Equals) return isEqual(property_value, match->Object())?Equals:0;
	else if (match->Predicate() == Greater) return isGreater(property_value, match->Object());
	else if (match->Predicate() == Less) return isLess(property_value, match->Object());
	else if (match->Predicate() == Circa) return isAproxymately(property_value, match->Object());
	else if (match->Predicate() == Not)
		return isEqual(property_value, match->Object()) ? False : match->Object();//True;
	else return has(subject, match->Predicate(), match->Object()); // match->Subject == Subject?

	return 0;
	//  if(match->Predicate==Range) isGreater and isLess
}

int recursions = 0;

bool areAll(Node *a, Node *b) {

	return isA4(a, b, true, true); // greedy isA4
}

// xpath  cities[population>100000, location=europe]
//Statement* toStatement(Node* n)
//Statement* getStatement(Node* n)

Statement *isStatement(Node *n) {
	if (n and n->kind == _statement) return n->value.statement;
	return 0;
}

NodeVector show(NodeVector all) {
	return showNodes(all);
}

NodeVector showWrap(Node *n) {
	show(n);
	NodeVector r;
	r.push_back(n);
	return r;
}

NodeVector wrap(Node *n) {
	NodeVector r;
	r.push_back(n);
	return r;
}

NodeSet show(NodeSet all, bool showStatements, bool showRelation, bool showAbstracts) {
	NodeSet::iterator it;
	for (it = all.begin(); it != all.end(); ++it) {
		Node *node = (Node *) *it;
		show(node, showStatements);
	}
	return all;
}

NodeVector showNodes(NodeVector all, bool showStatements, bool showRelation, bool showAbstracts) {
	int size = (int) all.size();
	ps("+++++++++++++++++++++++++++++++++++");
	for (int i = 0; i < size and i < resultLimit; i++) {
		Node *node = (Node *) all[i];

		if (i > 0 and showRelation) {
			S r = findRelations(all[i - 1], node);
			N n = findRelation(all[i - 1], node);
			if (!r or !n) p("? -> ? \n");
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
int getStatementId(long pointer) {
	return (int) ((pointer - (long) statement_root) / sizeof(Statement));
}
//NodeVector find_english(string& data) {
//}

Node *first(NodeVector rows) {
	if (rows.size() > 0) {
		Node *n = (Node *) rows[0];

		return n;
	}
	return 0;
}

Node *last(NodeVector rows) {
	int s = (int) rows.size();
	if (s > 0) {
		Node *n = (Node *) rows[s - 1];

		return n;
	}
	return 0;
}

void initUnits() {
	//  printf("Abstracts %p\n", abstracts);
	printf("Abstracts %p\n", abstracts);
	Ahash *ah = &abstracts[wordHash("meter") % maxNodes];//???;
	if (ah < abstracts or ah > extrahash /**2*/) {
		ps("abstracts kaputt");
		//		collectAbstracts();
	}
	Node *u = getThe("meter", Unit);
	addStatement(getAbstract("length"), has_the("unit"), u);

	//  addSynonyms(u,"m");
	getThe("m^2", Unit);
	getThe("km^2", Unit);
	getThe("millisecond", Unit);
	getThe("mile", Unit);
	getThe("km", Unit);
	//  u=add("phone number",String);// normalize!
	//  addSynonyms(u,"m^2","sqm");
}

Statement *error_statement = 0;

// SAME as evaluate!!

void loadConfig();

extern "C"
//Statement * learn(string& sentence0) {
Statement *learn(const char *sentence0) {
	if (!error_statement)error_statement = addStatement(Error, Error, Error);
	p("OK");
	p(sentence0);
	string sentence = sentence0;
	ps("Learning " + sentence);
	Statement *s;
	if (contains(sentence, "="))
		s = evaluate(sentence, true);
	else
		s = parseSentence(sentence, true);
	if (checkStatement(s)) {
		showStatement(s);
		return s;
	} else {
		ps("not a valid statement:");
		ps(sentence);
		return error_statement;
	}
}

/*
 int collectAbstracts3() {
 Context* c = context;
 Node* found = 0;
 int _abstract = Abstract->id;
 int max = c->nodeCount; // maxNodes;

 // collect Abstracts
 for (int i = 0; i < max; i++) {
 Node* n = &c->nodes[i];
 if (n == null or n->name == null or n->id == 0 or n->context == 0)
 continue;
 if (n->kind == _abstract)
 insertAbstractHash(n);
 }
 for (int i = 0; i < max; i++) {
 Node* n = &c->nodes[i];
 if (n == null or n->name == null or n->id == 0 or n->context == 0) {
 if (i > 1000) {
 ps("collectAbstracts : bad node:");
 pi(i);
 break;
 }
 continue;
 }
 Node* abstract = hasWord(n->name);
 if (abstract) {
 if (n->kind == _abstract)
 continue;
 else
 addStatement(abstract, Instance, n);//what?? no possible
 } else {
 if (n->kind == _abstract) {
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
//  add(name);
//}
//69 (Q30203): year
//69 (Q713048): natural number
//void cleanAbstracts(Context* c){
Node *getThe(Node *abstract, Node *type,bool create /*true!*/) {// first instance, TODO
	if (!abstract or !abstract->name) return 0;
	if (abstract->kind == _singleton)return abstract;
	if (abstract->kind == _entity)return abstract;// hack! first _entity wikidata is best? see importWikiLabels
	if (getRelation(abstract->name)) // not here! doch
		return getRelation(abstract->name);
	if (type < node_root or type > &node_root[maxNodes]) type = 0;// default parameter hickup through JNA
	if (abstract->value.node and checkNode(abstract->value.node, 0, false, true, false) and
	    eq(abstract->value.node->name, abstract->name))
		if (type == 0 or type == Any or type->id == abstract->value.node->kind)
			return abstract->value.node; // abstract->value.node as cache for THE instance
	if (type == 0) {
		// CAREFUL: ONLY ALLOW INSTANCES FOR ABSTRACTS!!!
		Statement *s = 0;
		Node *best = 0;
		int lookups=1000;// instance relation must be at beginning!
		while ((s = nextStatement(abstract, s))and lookups-->0)
			if (s->Predicate() == Instance and s->object != 0) {        // CAN NOT ASSUME RIGHT ORDER!
				if (!best || best->statementCount < s->Object()->statementCount)
					best = s->Object();
			}
		if (best) {
			abstract->value.node = best;
			return best;
		}
		if(create){
			N first = add(abstract->name, 0); // NO SUCH!! CREATE!?
			if (!atoi(abstract->name))
				abstract->value.node = first; // CACHE! -> DON't store numbers in abstract->value (69: year, natural number, ...)
			return first;
		}return abstract; // or 0 !
	}
	if (type->id == _abstract or type->id == _singleton)
		return getAbstract(abstract->name); // safe

	Statement *s = 0;
	Node *best = 0;
//	map<Statement*, int> visited; // You don't have to do anything. Just let the variable go out of scope.
	while ((s = nextStatement(abstract, s))) {
//		if (visited[s]) return 0;
//		visited[s]=1;
		if (!checkStatement(s, true, false)) continue;
		bool subjectMatch = (s->Subject() == abstract or abstract == Any);
		bool predicateMatch = (s->Predicate() == Instance);
		N object = s->Object();
		bool typeMatch = (type == 0 or object == type or object->kind == type->id);
		typeMatch = typeMatch or type == More or isA4(object, type, maxRecursions, true); //semantic, depth 0
		if (subjectMatch and predicateMatch and typeMatch) {
			if (type != More)
				return object;
			if (!best or object->statementCount > best->statementCount)
				best = object;
		}
		bool subjectMatchReverse = object == abstract;
		bool predicateMatchReverse = s->Predicate() == Type; // or inverse
		bool typeMatchReverse = (type == 0 or s->Subject() == type); // isA4(s->Object, type)

		if (type != More and subjectMatchReverse and predicateMatchReverse and typeMatchReverse) {
			best = s->Subject();
		}
	}
	if (!best) {
		best = add(abstract->name, type->id, 0);
		if (type->id > 0) addStatement(best, Type, type, false);
		addStatement(abstract, Instance, best, false, true);
	}
	return best;
}

extern "C"
bool isA(Node *fro, Node *to) {
//	show(fro);
//	show(to);
	if (isA4(fro, to, 0, 0)) return true;
	newQuery();
	Statement *s = 0;
	while ((s = nextStatement(fro, s))) {// 'quick' check
		if (s->Object() == fro and isA4(s->Predicate(), to))
			return true;// x.son=milan => milan is_a son
		if (s->Subject() == fro and s->Object() == to) {
			if (s->Predicate() == SuperClass)return true;
			if (s->Predicate() == Type)return true;
			if (s->Predicate() == Instance)return true;
			if (s->Predicate() == Synonym)return true;
		}
		if (s->Subject() == to and s->Object() == fro) {
			if (s->Predicate() == SubClass)return true;
			if (s->Predicate() == Type)return true;
			if (s->Predicate() == Instance)return true;
			if (s->Predicate() == Synonym)return true;
		}
	}
	return !findPath(fro, to, parentFilter).empty();
}

// all mountains higher than Krakatao
// todo:wordnet bugs: vulgar

void setValue(Node *node, Node *property, Node *value) {
	Statement *s = findStatement(node, property, value);
	if (s) {
		if (!eq(s->Object(), value)) {
			printf("value already set %s.%s=%s ... replacing with %s", node->name, property->name, s->Object()->name,
			       value->name);
			removeStatement(node, s); // really?? save history?
		} else return; //Wert schon da => nix?
	}
	addStatement(node, property, value, false);
}

void setName(int node, cchar *label) {
	p(node);
	p(label);
	if (node and label) setLabel(get(node), label, false, false);
}

void setLabel(Node *n, cchar *label, bool addInstance, bool renameInstances) {
//  if(addInstance and n!=get(n->id))n=save(n);// HOW!?! WHAT?
//	if(label[0]=='<')
//		bad(); "<span dbpedia parser fuckup etc
	if (label[0] == '"')label++;
	int len = (int) strlen(label);
	Context *c = context;
	char *newLabel = name_root + c->currentNameSlot;
//  if(!n->name or !strlen(n->name)) n->name=newLabel;// prepare to write
//  else
	bool hasName = n->name and n->name >= c->nodeNames and n->name < c->nodeNames+maxChars;
	if (hasName and eq(n->name, label, false))return;
	if (hasName and strlen(n->name) >= len) {// reuse! NOT when sharing char*s !!
		strcpy(n->name, label);
		n->name[len] = 0;
	} else if(label >= c->nodeNames && label < &c->nodeNames[c->currentNameSlot]){
		n->name= const_cast<char *>(label);
	} else {
		strcpy(newLabel, label);
		int len = (int) strlen(label);
		newLabel[len] = 0;// be sure!
		n->name = newLabel;//
		c->currentNameSlot += len + 1;
	}
	if (n->kind == _internal)return;
	if (n->kind == _abstract) {
		if (!renameInstances)return;
		NV all = instanceFilter(n);
		for (int i = 0; i < all.size(); i++)
			setLabel(all[i], label, false);
		if (!hasWord(label))
			insertAbstractHash(n);// not here!
		//    else
		//      mergeNode(getAbstract(label),n);
	} else {
		if (addInstance) {
			Node *a = getAbstract(label);
			addStatement(a, Instance, n);
		} else
			n = n;// all good
	}
	//  p(n);
	//  return n->name;
}

bool checkParams(int argc, char *argv[], const char *p) {
	string minus = "-";
	string colon = ":";
	string slash = "/";
	string equals = "=";
	for (int i = 1; i <= argc; i++) {
		if (eq(argv[i], p)) return true;
//		if (eq(argv[i], (slash+p).c_str()))return true;
		if (eq(argv[i], (colon + p[0]).c_str()))return true; // import
		if (eq(argv[i], (colon + p).c_str())) return true; // :server
		if (eq(argv[i], (minus + p).c_str())) return true;
		if (eq(argv[i], (minus + minus + p).c_str()))return true;
	}
	return false;
}

// description
char *getText(Node *n) {
	if (isAbstract(n))return NO_TEXT;
	context = getContext(current_context);
	if (n->value.text >= context->nodeNames and n->value.text <= &context->nodeNames[context->currentNameSlot]) {
		return n->value.text;
	}
	return NO_TEXT;
}


string formatImage(Node *image, int size, bool thumb) {
	if (!image or !checkNode(image) or !image->name) return "";
	char *name = replaceChar(image->name, ' ', '_');
	replace_all(name, "%20", "_", true);
	char *start = strstr(name, "File:");
	if (start) name = start + 5;
	if (startsWith(name, "http"))return name;
	string hash = md5(name);// undefined reference? make clear ! why??
	string base = "http://upload.wikimedia.org/wikipedia/commons/";
	if (!thumb) return base + hash[0] + "/" + hash[0] + hash[1] + "/" + name;
	char ssize[12];
	sprintf(ssize, "%d", size);
	return base + "thumb/" + hash[0] + "/" + hash[0] + hash[1] + "/" + name + "/" + ssize + "px-" + name;
}

string getImage(cchar *a, int size, bool thumb) {
	return getImage(getAbstract(a), size, thumb);
}

string getImage(Node *a, int size, bool thumb) {
	if (!a or !checkNode(a))return 0;
	Node *i = 0;
	if (!i)i = findProperty(a, get(-10018), true, 10000);// todo: insert at top!

//	TOO EXPENSIVE:
	if (!i)i = findProperty(a, "image", true, 1000);// Amazon
	if (!i)i = findProperty(a, "Bild", false, 1000);// P18 !
	if (!i)i = findProperty(a, "wiki_image", false, 100);
	if (!i)i = findProperty(a, "product_image_url", false, 20);
	if (!i)i = findProperty(a, "Wappen", false, 100);
	if (!i)i = findProperty(a, "Positionskarte", false, 100);
	if (!i)i = findProperty(a, "Lagekarte", false, 100);
//Flagge (Abbildung)
	// *.svg , jpg, bmp, jpeg , png !!! TODO
//	Wikivoyage-Banner
	if (!i)return "";
//	if (!i or !checkNode(i)){
//		N ab=getAbstract(a->name);
//		if(!ab or ab==Error or !(isAbstract(ab)))return "";
//		if(ab!a)==Error
////		if(ab!=a)return getImage(ab,size,thumb); LOOP DANGER BUG
//	}
	return formatImage(i, size, thumb);
}

Node *mergeAll(cchar *target) {
	Node *node = getAbstract(target);
	NV all = instanceFilter(node);
	for (int i = 0; i < all.size(); i++)
		mergeNode(node, all[i]);
	return node;
}

void setKind(int id, int kind) {
	get(id)->kind = kind;
}

extern "C" Node *save(Node *copy) {
	p("SAVING");
	p(copy);
	Node *node = get(copy->id);
	if (node == copy)return node;
	memcpy(node, copy, nodeSize);
	return node;
}

extern "C" void save2(Node n) {
	if (get(n.id) == &n)return;
	memcpy(get(n.id), &n, nodeSize);
}

int test2() {
	return 12345;
}        // RUBY/ JNA !!

void replay(const char *file) {
	char *line = (char *) malloc(MAX_CHARS_PER_LINE);
//	while(readFile("logs/query.log", line)){
//	while(readFile("logs/commands.log", line)){
	while (readFile(file, line)) {
		char *data = line;
		if (data[strlen(data) - 1] == '\n')data[strlen(data) - 1] = 0;
		if (data[strlen(data) - 1] == '\r')data[strlen(data) - 1] = 0;
		if (contains(line, ":del"))parse(line, false, false);
		else if (contains(line, ":learn"))parse(line, false, false);
		else if (contains(line, ":label"))parse(line, false, false);
		else handle(concat("/ee/",line));// chaos monkey
	}
}

Context *currentContext() {
	return getContext(current_context);
}

void stripName(Node *n) {
	int l = (int) strlen(n->name);
	if (l > 1 and n->name and n->name[l - 1] == ' ') {
		if (debug)p(n->name);
//		continue;
		N o = hasWord(n->name);
		if (o) {
			if (o != n)p(o->name);
//			if(!checkNode(o))continue;
			int l = (int) strlen(o->name);
			if (l > 1 and n->name and o->name[l - 1] != ' ') {
				if (debug)p(o->name);
				//					n->name[l-1]=0;
			}
		}
	}
}

void addSeo(Node *n0) {
	Node *n = n0;
	if (n->kind != _abstract and n->kind != _singleton)
		n = getAbstract(n->name);
	if (!checkNode(n, 0, 0, 1, 0) or len(n->name) < 2)return;
	string see = generateSEOUrl(n->name);
	if (see == n->name)return;
	cchar *seo = see.data();
	N old = hasWord(seo, true);
	if (old) {
		if (old->statementCount >= n0->statementCount)return;// ok
		if (old == n)return;// ok, abstract
		if (debug)pf("addSeo FORCE %s	->	%s\n", n->name, seo);
		insertAbstractHash(wordHash(seo), n, true, true);
		return;
	}
//	N ss=getAbstract(seo);
//	addStatement(n, Label, ss);
//	pf("addSeo %s	->	%s\n",n->name,seo);
//	insertAbstractHash(wordHash(seo),n,false,false);// old writing? makes no sense
	insertAbstractHash(wordHash(seo), n, false, true);
}

// :build-seo
void buildSeoIndex() {
	p("Building SEO index. This can take some minutes.");
//	importTest();
	context = currentContext();
	debug = false;
//	addSeo(get(10506175));
//	return;
	N bad=getThe("Amazon product");
	for (int i = 1; i < maxNodes; i++) {
		if (i % 10000 == 0) {
			pf("%d\r", i);
			fflush(stdout);
		}
		Node *n = get(i);
		if (!checkNode(n, i, 0, 1, debug) or !n->name)continue;
		if(n->kind==bad->kind)continue;
		addSeo(n);
		stripName(n);
		//			2/2-Wege Direktgesteuertes Ventil 184684 230 V/AC G 1/2 Muffe Nennweite 8 mm Gehäusematerial Messing Dichtungsma HOW??
//				show(hasWord(n->name));
//			deleteNode(n);
	}
}

void fixBrokenStatement() {
	for (int i = 1; i < maxStatements; i++) {
		if (i % 10000 == 0) {
			pf("%d\r", i);
			fflush(stdout);
		}
		Statement *s = getStatement(i);
		if (s->subject < -propertySlots or s->subject > maxNodes)s->subject = _error;
		if (s->predicate < -propertySlots or s->predicate > maxNodes)s->predicate = _error;
		if (s->object < -propertySlots or s->object > maxNodes)s->object = _error;
	}
}


void fixInstances() {
	autoIds = false;
	for (int i = 1; i < maxNodes - propertySlots; i++) {
		if (!checkNode(i))continue;
		N n = get(i);
		if (empty(n->name))continue;
		N a = getAbstract(n);
		if (a == n)continue;
		if (isAbstract(n)) {
//			n->kind = _entity;// was singleton!
			n->kind = _abstract;
			continue;
		}
		else
			addStatement(a, Instance, n, true, true);
	}
}

void fixPostleitzahlen() {
	N a=get(-10281);
	Statement* s=0;
	while ((s=nextStatement(a, s))){
		addStatement(s->Object(),Type,a);
	}
}



void fixICD10() {
	N a=get(-10494);
	Statement* s=0;
//	N typ=getThe("")
	autoIds= false;
	while ((s=nextStatement(a, s))){
		if (s->predicate==-10494 && s->object>0&&s->object<10000)
			s->object=getThe("Q"+itoa(s->object))->id;
		if (s->predicate==-10494 && s->object<-10000&&s->object<-20000)
			s->object=getThe("P"+itoa(s->object))->id;
//		addStatement(s->Subject(),s->Predicate(),getThe("Q"+itoa(s->object)));
	}
}

void cacheTopic() {
	autoIds = false;
	Node *topic = getThe("topic");
	int already = 0;
	for (int i = 1; i < maxNodes - propertySlots; i++) {
		if(i%100000==0 or already%10000==0)pf("%d\t%d\r",i,already);
		if (!checkNode(i))continue;
		N n = get(i);
		if(n->kind>0)already++;
		if (isAbstract(n))continue;
		if (empty(n->name))continue;
		N top = getTopic(n);
		if(top == n || !checkNode(top)||eq(n->name,top->name))
			top=getClass(n);
		if(top == n || !checkNode(top)||eq(n->name,top->name))
			continue;
		addStatement(n, topic, top, true, true);
	}
}


void fixThe() {
	int already=0;
	for (int i = 1; i < maxNodes - propertySlots; i++) {
		if(i%100000==0)pf("%d\t%d\r",i,already);
		if (!checkNode(i))continue;
		N n = get(i);
		if (isAbstract(n))continue;
		if (empty(n->name))continue;
		if(checkNode(n->value.node)){
			already++;
			continue;
		}
		getAbstract(n)->value.node=n;
	}
}

void fixCurrent() {
	importCsv("Telekom/entities.ee.csv",0,0,0,"name,topic",0);
//	context->nodeCount=context->lastNode;
//	context->lastNode=wikidata_limit;// fill all empty slots!
//	p(*findAllWords("Spanien"));
//	fixThe();
//	fixInstances();
//	cacheTopic();
//	replay();
//	importBilliger();
//	buildSeoIndex();
//	context->lastNode=1;// RADICAL: fill all empty slots!
//	context->lastNode =60000000;// as of 5/2018  vs  https://www.wikidata.org/wiki/Q50000000
//	context->lastNode = (int) maxNodes / 2;
//	fixBrokenStatement();
//	importRemaining();
//	add_force(current_context, 415898, "Telekom", _singleton);
}


//	import("billiger.de/TOI_Suggest_Export_Products.csv");
//	replay();
//	N a=getThe("Amazon dvd product");
//	Statement* s=0;
//	while ((s=nextStatement(a, s))){
//		if(wordCount(s->Subject()->name)==1)
//			deleteNode(s->Subject());
//	}
//}

Node *mergeNode(Node *target, Node *node) {
	addStatementToNodeDirect(target, node->firstStatement);
	Statement *s = getStatement(node->firstStatement);
	Statement *next;
	while (s) {
		next = nextStatement(node, s);
		if (s->Predicate() == Instance and s->Subject() == target)
			deleteStatement(s);
		else {
			if (s->Subject() == node)s->subject = target->id;
			if (s->Predicate() == node)s->predicate = target->id;
			if (s->Object() == node)s->object = target->id;
//			addStatementToNodeDirect(target,s->id());// no: already linked
		}
		s = next;
	}
	addStatementToNodeDirect(target, node->firstStatement);
	target->statementCount += node->statementCount;
	node->lastStatement = node->lastStatement;
	node->statementCount = 0;// keep rest
//	for (int i=0; i<context->statementCount; i++) {
//		s=&context->statements[i];
//		if(s->Subject()==node)s->subject=target->id;
//		if(s->Predicate()==node)s->predicate=target->id;
//		if(s->Object()==node)s->object=target->id;
//	}
//  deleteNode(node);
	return target;
}


//#include <csignal> // or signal.h if C code // Generate an interrupt
//void SIGINT_handler(int x){
//	shutdown_webserver();
//}
//#define _MAIN_
int main(int argc, char *argv[]) {
	char *data = getenv("QUERY_STRING");
	if (data) {// ^^ CGI mode
		printf("Content-Type: text/plain;charset=us-ascii\n\n");
		printf("got QUERY_STRING %s", data);
		initSharedMemory();
		show(parse(data, false, false));
		//	start_server();
	}

	loadConfig();
	//	signal(SIGSEGV, handler); // only when fully debugged!
	//	signal(SIGINT, SIGINT_handler); // only when fully debugged! messes with console!
	//  setjmp(loop_context);
	path = argv[0];
	path = path.substr(0, path.rfind("/") + 1);
	if (path.rfind("/dist") != -1) path = path.substr(0, path.rfind("/dist") + 1);
	system(string("cd " + path).c_str());
	data_path = path + "/data/";
	import_path = path + "import/";
	mkdir("./logs", 0777);
	//	path=sprintf("%s/data",path);

	string a;
	for (int i = 1 ; i < argc; i++) a = a + argv[i] + " ";
	cchar *query = a.data();

    // todo REDUNDANT! remove!
	if (checkParams(argc, argv, "quiet")) quiet = true;
    if(query[0]=='/')quiet = true;
	bool quit=checkParams(argc, argv, "exit")|| checkParams(argc, argv, "quit");

	if (checkParams(argc, argv, "query") or checkParams(argc, argv, "select") or checkParams(argc, argv, "all")) {
        quiet = true;
        lookupLimit=queryLimit;
        query = cut_to(cut_to(query, "query "), "select");
    }


	germanLabels = true;

	initSharedMemory(); // <<<

	if (checkParams(argc, argv, "clear"))clearMemory();
	if (checkParams(argc, argv, "de"))germanLabels = true;
	if (checkParams(argc, argv, "import")) {
		if (checkParams(argc, argv, "all") or argc < 2)
			importAll();
		else import(argv[2]); // danger netbase clear import save
		if (checkParams(argc, argv, "save")) save();
	}
	if (checkParams(argc, argv, "load")) load(true);

	printf("Warnings: %d\n", badCount);
	showContext(0);

	if (checkParams(argc, argv, "load_files")) load(true);
	else if (checkParams(argc, argv, "debug")) {
		printf("debugging\n");
		for (int i = 0; i <argc; ++i) {
			p(argv[i]);
		}
		testBrandNewStuff(); // << PUT ALL HERE!
	} else {
		bool _autoIds = autoIds;
		autoIds = true;
		// *******************************
		if(query[0]=='/')
			return handle(query);

		NodeVector results = parse(query, false, argc<2); // <<< HERE
		// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		show(results);
		autoIds = _autoIds;
	}
	if (checkParams(argc, argv, "export")) export_csv();
	if (checkParams(argc, argv, "test")) testAll();
	if (checkParams(argc, argv, "server") or checkParams(argc, argv, "daemon") or checkParams(argc, argv, "demon")) {
		printf("Starting server\n");
		for (int i = 1; i < argc; i++)
			if (atoi(argv[i]) > 0) SERVER_PORT = atoi(argv[i]);
		start_server(SERVER_PORT);
		return 0;
	}
	if (quit) exit(0);
	context = getContext();
	console();
	//  } catch (std::exception const& ex) {
}
// _MAIN_ ^^^