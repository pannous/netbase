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
// strcmp
#include <string.h>
using namespace std;

#include "netbase.hpp"
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
//#include "tests.cpp"
//#define assert(cond) ((cond)?(void)(0): (void)(fprintf (stderr,"FAILED: %s, file %s, line %d \n",#cond,__FILE__,__LINE__), (void)abort()))


int nameBatch = 100;
char* root_memory = 0;
bool storeTypeExplicitly = true;
bool exitOnFailure = true;
bool debug = false;//true;//false;
int maxRecursions = 7;
int runs = 0;
Context* contexts; //[maxContexts];// extern
string path; // extern

//int maxNodes() {
//    return maxNodes;
//}

int maxStatements() {
    return maxStatements0;
}

//extern "C" inline
Context* currentContext() {
    return &contexts[current_context];
}

int badCount;
int current_context = wordnet;

//map<string, Node*> abstracts;
Ahash* abstracts; // Node name hash
Ahash* extrahash; // for hashes that are not unique, increasing

//map<const char*,Node*> abstracts;
map<int, int> wn_map;
map<int, int> wn_map2;

map < Node*, bool> yetvisited;
map <double, short> yetvisitedIsA;
//static map < Node*, bool> yetvisited;
//static map <double, short> yetvisitedIsA;
//static map < Node*, short> yetvisitedIsA;
//static map <double, bool> yetvisitedIsA;

// int search;// use in external maps!
// int subjectContext;//... na! nur in externer DB!


void flush() {
	fflush(stdout);
}



bool checkHash(Ahash* ah){
//    if(pos>maxNodes*2)
    if(ah<abstracts|| ah> (void*)&abstracts[maxNodes*2]){// times 2 because it can be an extraHash outside of abstracts!!
		p("ILLEGAL HASH!");
	//	pi(pos);
		px(ah);
		return false;
    }
	return true;
}

Ahash* insertAbstractHash(Node* a) {
    return insertAbstractHash(hash(a->name), a);
}

Ahash* insertAbstractHash(int pos, Node* a) {
    Ahash* ah = &abstracts[pos];
	if(!checkHash(ah))
		return 0;
//    if(pos==hash("city"))
//		p(a->name);
    int i=0;
    while (ah->next && i++<10) {
        if (ah->abstract == a || eq(ah->abstract->name,a->name))
			return ah; //schon da
        else
			ah = ah->next;
    }
    if (ah->abstract) {//schon voll
        Ahash* na = extrahash;
        extrahash++;
        ah->next = na;
        ah = na;
    }
    ah->abstract = a;
    return ah;
}

bool addStatementToNode2(Node* node, int statementNr) {
    int n = node->statementCount;
    if (n == 0)node->firstStatement = statementNr;
    else {
        Context* context = getContext(node->context);
        Statement* statement0 = getStatement(node, n - 1);
        if (statement0 == 0) {
            badCount++;
            return false;
        }
        //            Statement* statement1=&context->statements[statementNr];
        if (statement0->Subject == node) statement0-> nextSubjectStatement = statementNr; //statement1;
        if (statement0->Predicate == node) statement0-> nextPredicateStatement = statementNr; //statement1;
        if (statement0->Object == node) statement0-> nextObjectStatement = statementNr; //statement1;
    }
    node->statementCount++;
    return true;
}

bool appendLinkedListOfStatements(Statement *statement0, Node* node, int statementNr){
//    Statement* statement1=&c->statements[statementNr];
    if (statement0->Subject == node)
        statement0-> nextSubjectStatement = statementNr;
    if (statement0->Predicate == node)
        statement0-> nextPredicateStatement = statementNr;
    if (statement0->Object == node)
        statement0-> nextObjectStatement = statementNr;
}

bool addStatementToNodeWithInstanceGap(Node* node, int statementNr) {
    int n = node->statementCount;
    if (n == 0){
        node->firstStatement = statementNr;
        node->statementCount++;
        return true;
    }
    else {
        Context* context = getContext(node->context);
		Statement* statement0 = getStatement(node, n - 1,true);// using instant gap ONLY HERE
        // because of InstanceGap!! ^^^ todo O(n) -> O(1)
		if (statement0 == 0) {
            badCount++;
            return false;
        }
    Context* c = getContext(node->context);
    int statement2Nr=0;
        if (statement0->Subject == node){
            statement2Nr=statement0-> nextSubjectStatement;
            statement0-> nextSubjectStatement = statementNr;
        }
        if (statement0->Predicate == node){
            statement2Nr=statement0-> nextPredicateStatement;
            statement0-> nextPredicateStatement = statementNr;
        }
        if (statement0->Object == node){
            statement2Nr=statement0-> nextObjectStatement;
            statement0-> nextObjectStatement = statementNr;
        }
        Statement* statement1=&c->statements[statementNr];
        appendLinkedListOfStatements(statement1,node,statement2Nr);
    }
    node->statementCount++;
    return true;
}


//bool addStatementToNodeInstanceGap(Node* node, int statementNr) {
bool addStatementToNode(Node* node, int statementNr) {
    int n = node->lastStatement;
    if (n == 0){
        node->firstStatement = statementNr;
        node->lastStatement = statementNr;
        node->statementCount++;
        return true;
    }
    else {
		int statement2Nr=0;// find free empty (S|P|O)statement slot of lastStatement
		Context* context=getContext(node->context);
        Statement* statement0 = &context->statements[node->lastStatement];// last statement
        Statement* statement1= &context->statements[statementNr];// target
        if (statement0->Subject == node){
            statement2Nr=statement0-> nextSubjectStatement;// rescue old
            statement0-> nextSubjectStatement = statementNr;
        }
        if (statement0->Predicate == node){
            statement2Nr=statement0-> nextPredicateStatement;
            statement0-> nextPredicateStatement = statementNr;
        }
        if (statement0->Object == node){
            statement2Nr=statement0-> nextObjectStatement;
            statement0-> nextObjectStatement = statementNr;
        }
		// squeeze statement1 in between statement0 and statement2Nr
        appendLinkedListOfStatements(statement1,node,statement2Nr);// put target into free slot
		node->lastStatement=statementNr;
    }
    node->statementCount++;
    return true;
}

char* statementString(Statement* s){
	char name[1000];
	sprintf(name,"(%s %s %s)",s->Subject->name,s->Predicate->name,s->Object->name);
	return name;
}
Node* reify(Statement* s){
	if(!checkStatement(s))return 0;
	Node* reified=add(statementString(s),_reification,s->context);
	reified->value.statement=s;
	return reified;
}

bool checkStatement(Statement *s,bool checkSPOs,bool checkNamesOfSPOs){
    if(s==0)return false;
    if(s<contexts[current_context].statements)return false;
    if(s>=contexts[current_context].statements+maxStatements0)return false;
	if(checkSPOs||checkNamesOfSPOs)
		if (s->Subject == 0 || s->Predicate == 0 || s->Object == 0)return false;
	if(checkNamesOfSPOs)if (s->Subject->name == 0 || s->Predicate->name == 0 || s->Object->name == 0)return false;
	return true;
}

char* name(Node* node) {
    if (!node)return "NULL";
    if (node == 0)return "NULL";
    return node->name;
}

//static Context contexts[maxContexts];
// Context* contexts;

Context* getContext(int contextId) {
    if (contextId == 0) {
//        p("Context#0!");
    }
	if (!multipleContexts)
        contextId = wordnet; // just one context

    Context* context = &(contexts[contextId]); //*(Context*)malloc(sizeof(Context*));

	if (context->nodes != null && context->id==contextId) {
//		printf("Found context %d: %s\n",context->id,context->name);
//		flush();
        return context;
    }

	printf("Reset context %d: %s",context->id,context->name);
    context->id = contextId;
    context->currentNameSlot = 0; //context->nodeNames;
#ifdef statementArrays
    context->statementArrays = (int*) malloc(maxStatements());
#endif
    initContext(context);
    if (contextId == wordnet)
        context->nodeCount = 1000; //sick hack to reserve first 1000 words!
    else
        context->nodeCount = 0;
    context->statementCount = 1; //1000;
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

void showContext(Context* cp) {
    if (quiet)return;
    Context c = *cp;
	char* format="Context#%d name:%s nodes:%d, statements:%d n#%016llX nN#%016llX s#%016llX\n";
    printf(format, c.id, c.name, c.nodeCount, c.statementCount, c.nodes, c.nodeNames, c.statements);
    flush();
}

void showContext(int nr) {
    showContext((Context*) getContext(nr));
}

void checkExpand(Context* context) {
    //    if(!context->nodeNames)
    context->nodeNames = (char*) malloc(nameBatch);
    if (context->currentNameSlot % nameBatch < 5) {
        void* tmp = realloc(context->nodeNames, context->currentNameSlot + nameBatch);
        if (!tmp) {
            p("Out of memory error");
        } else if (tmp != context->nodeNames) {
            p("context->names moved!! what is with the pointers??");
            context->nodeNames = (char*) tmp; //dare it
        } else if (!quiet)
            p("context->names checkExpanded");
    }
}

bool isA4(Node* n, string match, int recurse, bool semantic) {

    if (!checkNode(n))return false;
    if (eq(n->name, match.c_str()))return true; //&& n->name==match
    if (get(n->kind) && eq(get(n->kind)->name, match.c_str()))return true;

    if (recurse > 0)recurse++;
    else recurse = maxRecursions;
    if (recurse > maxRecursions)return false;

    if (semantic && has(n, "synonym", match, false, false, true))return true;
    if (semantic && has(n, "plural", match, false, false, true))return true;
    if (semantic && has(n, "type", match, false, false, false))return true;
    if (semantic && has(n, "parent", match, false, false, false))return true;
    //    if(semantic && has(n,"kind",match,false,false,false))return true;//?
    if (semantic && has(n, "name", match, false, false, false))return true;
    if (semantic && has(n, "label", match, false, false, false))return true;
    if (semantic && has(n, "is", match, false, false, false))return true; // --

    if (semantic && n->kind == Abstract->id)//|| isA(n,List)
        for (int i = 0; i < n->statementCount; i++) {
            Statement* s = getStatement(n, i);
            if (s->Predicate == Instance)
                if (isA4(s->Object, match, recurse, semantic))
                    return true;
        }
    return false;
}
//NodeVector findPath(Node* from, Node* to){
//
//}

void initNode(Node* node, int id, const char* nodeName, int kind, int contextId) {
    Context* context = getContext(contextId);
    if (!checkNode(node, id, false, false))return;
    node->id = id;
#ifndef inlineName
    node->name = &context->nodeNames[context->currentNameSlot];
#endif
    strcpy(node->name, nodeName);// can be freed now!
    int len = strlen(nodeName);
    context->nodeNames[context->currentNameSlot + len] = 0;
    context->currentNameSlot = context->currentNameSlot + 1 + len * sizeof (char);
    //	checkExpand(context);
    //	context->currentNameSlot[-4]=id;
    node->kind = kind;
    node->context = contextId;
    node->value.number=0;
    node->statementCount = 0;

#ifdef inlineStatements
    node->statements = 0; //nextFreeStatementSlot(context,0);
#endif
}

// return false if node not ok
// remove when optimized!!!!!!!!!!
bool checkNode(Node* node, int nodeId, bool checkStatements, bool checkNames) {
    if (node == 0) {
        badCount++;
        p("null node");
        pi(nodeId);
        return false;
    }
    Context* c = currentContext(); // getContext(node->context);
    void* maxNodePointer=&c->nodes[maxNodes];
    if (node < c->nodes) {
        badCount++;
        printf("node* < maxNodes!!! %016llX < %016llX \n", node,c->nodes);
        return false;
    }
    if (node>=maxNodePointer) {
        badCount++;
        printf("node* >= maxNodes!!! %016llX >= %016llX\n", node,maxNodePointer);
//		p("OUT OF MEMORY or graph corruption");
        return false;
    }
    if (node->context < 0 || node->context > maxContexts) {
        badCount++;
        p("wrong node context");
        p("node:");
        pi(nodeId);
        p("context:");
        pi(node->context);
        return false;
    }
    if (checkNames && node->name == 0 ) {
        badCount++;
        printf("node->name == 0 %d\n", node);
        return false;
    }
    if (checkNames &&  (node->name < c->nodeNames || node->name>=&c->nodeNames[averageNameLength * maxNodes ])) {
        badCount++;
        printf("node->name out of bounds %d\n", node);
        return false;
    }
    if (nodeId > maxNodes) {
        badCount++;
        p("nodeId>maxNodes");
        pi(nodeId);
        return false;
    }
#ifdef inlineStatements
    if (checkStatements && node->statements == null) {//
        badCount++;
        p("node not loaded");
        pi(nodeId);
        // initNode(subject,subjectId,(char*)NIL_string,0,contextId);
        return false;
    }
#endif
    return true;
}

Node* add(const char* nodeName, int kind, int contextId) {//=node =current_context
#ifndef DEBUG
	if(!nodeName)return 0;
#endif
    Context* context = getContext(contextId);
    Node* node = &(context->nodes[context->nodeCount]);
    //	if(context->nodeCount>maxNodes)return 0;
    initNode(node, context->nodeCount, nodeName, kind, contextId);
    if (!checkNode(node, context->nodeCount))return 0;
    context->nodeCount++;

    if (kind != abstractId)
        addStatement(getAbstract(nodeName), Instance, node, false);
    if (storeTypeExplicitly && kind > 105)// might cause loop?
        addStatement4(contextId, node->id, Type->id, kind, false); // store type explicitly!
    //	    why? damit alle Instanzen etc 'gecached' sind und algorithmen einfacher. Beth(kind:person).
    //  kosten : Speicher*2
    return node;
}

Node* add_force(int contextId, int id, const char* nodeName, int kind) {
    Context* context = getContext(contextId);
    // pi(context.nodes);// == &context.nodes[0] !!
    if (id > maxNodes) {
        if (quiet)return 0;
        printf("int context %d, int id %d id>maxNodes", contextId, id);
        return 0;
    }

#ifdef inlineStatements
    if (context->nodes[id].statements == null)
#endif
        context->nodeCount++; // add one , otherwise : overwrite
    Node* node = &context->nodes[id];
    Node* node2 = context->nodes+id;
    Node* node3 =(Node*) (root_memory+contextOffset+id*nodeSize);
    initNode(node, id, nodeName, kind, contextId);
    return node;
}

#ifdef inlineStatements

bool addStatementToNode2(Node* node, int statement) {
    if (node->statementCount < maxStatementsPerNode) {
        node->statements[node->statementCount] = statement; //? nodeCount;//!! #statement dummy nodes ?? hmm --
        node->statementCount++;
    } else {
        if (maxStatementsPerNode == node->statementCount) {// warn once
            badCount++;
            // p("maxStatementsPerNode!!");
            // p(node->name);
        }
        return false;
    }
    return true;
}
#endif

Statement* addStatement4(int contextId, int subjectId, int predicateId, int objectId, bool check) {
    if (contextId < 0 || subjectId < 0 || predicateId < 0 || objectId < 0) {
        p("WARNING contextId<0||subjectId<0||predicateId<0||objectId<0");
        return 0;
    }
    if (contextId > maxContexts || subjectId > maxNodes || predicateId > maxNodes || objectId > maxNodes) {
        badCount++;
        p("WARNING contextId>maxContexts||subjectId>maxNodes||predicateId>maxNodes||objectId>maxNodes");
        return 0;
    }
    Context* context = getContext(contextId);
    // pi(context.nodes);// == &context.nodes[0] !!
    if (subjectId > maxNodes || predicateId > maxNodes || objectId > maxNodes) {
        if (quiet)return 0;
        printf("int context %d,subjectId %d>maxNodes || predicateId %d>maxNodes ||objectId %d>maxNodes ", contextId, subjectId, predicateId, objectId);
        return 0;
    }
    Node* subject = &context->nodes[subjectId];
    Node* predicate = &context->nodes[predicateId];
    Node* object = &context->nodes[objectId];

    if (check && !checkNode(subject, subjectId))return 0;
    if (check && !checkNode(object, objectId))return 0;
    if (check && !checkNode(predicate, predicateId))return 0;

    Statement* statement = &context->statements[context->statementCount]; // union of statement, node??? nee
    statement->id = context->statementCount;
    context->statementCount++;
    statement->Subject = subject;
    statement->Predicate = predicate;
    statement->Object = object;
    statement->subject = subjectId;
    statement->predicate = predicateId;
    statement->object = objectId;
    if (!addStatementToNode(subject, statement->id))return statement;
    if (!addStatementToNode(object, statement->id))return statement;
    if (!addStatementToNode(predicate, statement->id))return statement;

    // predicate->statementCount++;
    // context->nodeCount++;
    return statement;
}

Statement* addStatement(Node* subject, Node* predicate, Node* object, bool checkDuplicate) {
    if (!checkNode(subject))return 0;
    if (!checkNode(object))return 0;
    if (!checkNode(predicate))return 0;
    if (checkDuplicate) {//todo: add specifications but not generalizations?
        Statement* old = findStatement(subject, predicate, object,0,0,0); //,true,true,true);
        if (old)return old; // showStatement(old)
    }
    Context* context = currentContext(); //  getContext(subject->context); // todo!
    // pi(context.nodes);// == &context.nodes[0] !!
    Statement* statement = &context->statements[context->statementCount]; // union of statement, node??? nee // 3 mal f√ºr 3 contexts !!!
    Statement* s = statement;
    statement->id = context->statementCount;
    statement->context = current_context; //todo!!
    statement->Subject = subject;
    statement->Predicate = predicate;
    statement->Object = object;
    statement->subject = subject->id;
    statement->predicate = predicate->id;
    statement->object = object->id;

    addStatementToNode(subject, statement->id);
    addStatementToNode(predicate, statement->id);
    addStatementToNode(object, statement->id);
    //	subject->statements[subject->statementCount]=context->statementCount;//? nodeCount;//!! #statement dummy nodes ?? hmm --
    //	subject->statementCount++;
    //	predicate->statements[predicate->statementCount]=context->statementCount;//? nodeCount;//!! #statement dummy nodes ?? hmm --
    //	predicate->statementCount++;
    //    object->statements[object->statementCount]=context->statementCount;//? nodeCount;//!! #statement dummy nodes ?? hmm --
    //	object->statementCount++;
    context->statementCount++;
    // pi(context->statementCount);
    // context->nodeCount++;
    return statement;
}

// has to be called with i, NOT with  n->statements[i]!!
// returns first Statement BEFORE Instance predicate!
// TODO!!! what if x->y->x !?!?!
// TODO firstInstanceGap too complicated, but needed for nodes with 1000000 instances (city etc)
// TODO only used in addStatementToNodeWithInstanceGap !!
Statement* getStatement(Node* n, int nr,bool firstInstanceGap) {
    if (nr >= maxStatementsPerNode) {
        badCount++;
        return null;
    }
    if (n == null) {
        badCount++;
        return null;
    }
    //	if(n->statements==null){badCount++;return null;}
    if (nr >= n->statementCount) {
        badCount++;
        return null;
    }
    if (n->firstStatement < 0) {
        badCount++;
        return null;
    }
    Context* c = getContext(n->context);
    Statement* statement = &c->statements[n->firstStatement];
    Statement* laststatement = statement;
    for (int i = 0; i < nr; i++) {
        if (statement == 0) {
			p("CORRUPTED STATEMENTS!");
			show(n);
            badCount++;
            return null;
        }
		if(i>0&&firstInstanceGap&&statement->Predicate==Instance){
            return laststatement;
        }
        if (statement->Subject == n) {
            statement = &c->statements[statement->nextSubjectStatement];
            continue;
        }
        if (statement->Predicate == n) {
            statement = &c->statements[statement->nextPredicateStatement];
            continue;
        }
        if (statement->Object == n) {
            statement = &c->statements[statement-> nextObjectStatement];
            continue;
        }
    }
    if(firstInstanceGap&&statement->Predicate==Instance){
            return laststatement;
    }
    return statement;
}

#ifdef inlineStatements
// has to be called with i, NOT with  n->statements[i]!!

Statement* getStatement2(Node* n, int nr) {
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

inline Node* get(int NodeId) {
    //    if (NodeId > maxNodes) {
    //        if (quiet)return 0;
    //        printf("id %d id>maxNodes", NodeId);
    //        return 0;
    //    }
    return &currentContext()->nodes[NodeId];
}

void dissectParent(Node* subject) {
    if (strstr(subject->name, "("))return;
    if (strstr(subject->name, ","))return;
    if (strstr(subject->name, "von"))return;
    if (strstr(subject->name, "vor"))return;
    if (strstr(subject->name, "zu"))return;
    if (strstr(subject->name, "of"))return;
    if (strstr(subject->name, "by"))return;
    if (strstr(subject->name, "de"))return;
    if (strstr(subject->name, "am"))return;
    if (strstr(subject->name, "at"))return;
    if (strstr(subject->name, "bei"))return;
    if (strstr(subject->name, "in"))return;
    if (strstr(subject->name, "from"))return;
    if (strstr(subject->name, "for"))return;
    //if(isAName(s)ret. // noe!
    string str = replace_all(subject->name, " ", "_");
    str = replace_all(str, "-", "_");
    //        p("dissectWord");
    //        p(subject->name);
    int len = str.length();
    int type = str.find("_");
    if (type < 1)type = str.find(".");
    if (type >= 0 && len - type > 2) {
        Node* word = getThe(str.substr(type + 1).c_str());
        addStatement(word, Instance, subject, true);
    }// release str
//    str.clear();
}

// Ausnahmen:
//Zugsicherung    Zugbeeinflussungssystem_S-Bahn_Berlin
//Zugunfall       Kesselwagenexplosion_in_der_BASF
//Zugsicherung    Geschwindigkeits√ºberwachung_Neigetechnik
//Zugsicherung    Zugsicherung_mit_Linienleiter_1990
//Zuggattung      ICE_International
//Zug_(Stadt)     Hochschule_Luzern_‚Äì_Wirtschaft
//Zug_(Stadt)     Padagogische_Hochschule_Zentralschweiz
//Zug     Fliegender_Hamburger
//Zug     Doppelstock-Stromlinien-Wendezug_der_LBE
//Zeitschriftenverlag     Gruner_+_Jahr
//Zeitschriftenverlag     Smith,_Elder_&_Co.
//Zeitschriftenverlag     Verlag_Neue_Kritik
//Zeitschriftenverlag     Verlag_Otto_Beyer
//Zeitschriftenverlag     Verlag_Technik

void dissectWord(Node* subject) {
    string str = replace_all(subject->name, " ", "_");
    str = replace_all(str, "-", "_");
    //        p("dissectWord");
    //        p(subject->name);
    int len = str.length();
    int type = str.find(",");
    if (type >= 0 && len - type > 2) {
        Node* word = getThe((str.substr(type + 2) + "_" + str.substr(0, type)).c_str()); //deCamel
        addStatement(word, Synonym, subject, true);
        Node* a = getThe((str.substr(0, type).c_str()));
        Node* b = getThe((str.substr(type + 2).c_str()));
        addStatement(a, Instance, subject, true);
        addStatement(b, Instance, subject, true);
        str = word->name;
        //        subject=word;
    }
    type = str.find("(");
    if (type > 0 && len - type > 2) {// not (030)4321643 !
        int to = str.find(")");
        string str2 = str.substr(type + 1, to - type - 1);
        Node* clazz = getThe(str2.c_str()); //,str.find(")")
        Node* word;
        if (type > 0)
            word = getThe(str.substr(0, type - 1).c_str()); //deCamel
        else
            word = getThe(str.substr(to + 1, len - 1).c_str()); //deCamel
        addStatement(word, Instance, subject, true);
        addStatement(clazz, Instance, word, true);
        //        addStatement(clazz, Member, word, true);
        addStatement(subject, Instance, clazz, true);
        //	    return;
        str = word->name;
        //        subject=word;
    }
    type = str.find("_in_");
    if (type >= 0 && len - type > 2) {
        Node* in = getThe("location");
        Node* word = getThe(str.substr(0, type).c_str()); //deCamel
        Node* ort = getThe(str.substr(type + 4).c_str());
        addStatement(word, Instance, subject, true);
        addStatement(subject, in, ort, true);
    }
    type = str.find("_at_");
    type = str.find("_am_");
    if (type >= 0 && len - type > 2) {
        Node* at = getThe("at");
        Node* word = getThe(str.substr(0, type).c_str()); //deCamel
        Node* ort = getThe(str.substr(type + 4).c_str());
        addStatement(word, Instance, subject, true);
        addStatement(subject, at, ort, true);
    }
    type = str.find("_from_");
    if (type >= 0 && len - type > 2) {
        Node* from = getThe("from");
        Node* word = getThe(str.substr(0, type).c_str()); //deCamel
        Node* ort = getThe(str.substr(type + 6).c_str());
        addStatement(word, Instance, subject, true);
        addStatement(subject, from, ort, true);
    }
    type = str.find("_for_");
    type = str.find("_für_");
    if (type >= 0 && len - type > 2) {
        Node* from = getThe("for");
        Node* word = getThe(str.substr(0, type).c_str()); //deCamel
        Node* obj = getThe(str.substr(type + 5).c_str());
        addStatement(word, Instance, subject, true);
        addStatement(subject, from, obj, true);
    }
    type = str.find("_bei_");
    if (type >= 0 && len - type > 2) {
        Node* in = getThe("near");
//        check(eq(getThe("near")->name,"near"));
        Node* word = getThe(str.substr(0, type).c_str()); //deCamel
        Node* ort = getThe(str.substr(type + 5).c_str());
        addStatement(word, Instance, subject, true);
        addStatement(subject, in, ort, true);
    }

    type = str.find("'s_");
    if (type < 0)type = str.find("s'_");
    if (type >= 0 && len - type > 2) {
        Node* hat = Member;
        Node* word = getThe(str.substr(0, type).c_str()); //deCamel
        const char* o = str.substr(type + 4).c_str();
        Node* ort = getThe(o);
        addStatement(ort, Instance, subject, true);
        addStatement(subject, hat, ort, true);
    }
    type = str.find("_of_");
    if (type < 0)type = str.find("_de_");// de_la_Casa
    if (type < 0)type = str.find("_du_");
    //_della_ de la del des
    if (type >= 0 && len - type > 2) {
        Node* hat = Member;
        Node* word = getThe(str.substr(0, type).c_str()); //deCamel
        const char* o = str.substr(type + 4).c_str();
        Node* ort = getThe(o);
        addStatement(word, Instance, subject, true);
        addStatement(ort, hat, subject, true);
    }
    type = str.find("_der_");
    if (type < 0)type = str.find("_des_");
    if (type < 0)type = str.find("_del_");
    //_della_
    if (type >= 0 && len - type > 2) {
        Node* hat = Member;
        Node* word = getThe(str.substr(0, type).c_str()); //deCamel
        string so = str.substr(type + 5); // keep! dont autofree
        Node* ort = getThe(so.data());
        addStatement(word, Instance, subject, true);
        addStatement(ort, hat, subject, true);
    }
    type = str.find("_von_");
    if (type >= 0 && len - type > 2) {
        Node* hat = Member;
        Node* word = getThe(str.substr(0, type).data()); //deCamel
        string so = str.substr(type + 5); // keep! dont autofree
        Node* ort = getThe(so.data());
        addStatement(word, Instance, subject, true);
        addStatement(ort, hat, subject, true);
    }
    type = str.find("._");
    if (type >= 0 && len - type > 2) {
        Node* number = getThe("number");
        Node* nr = getThe(str.substr(0, type).c_str()); //deCamel
        Node* word = getThe(str.substr(type + 2).c_str());
        addStatement(word, Instance, subject, true);
        addStatement(subject, number, nr, true);

    }
    type = str.find("_");
    if (type >= 0 && len - type > 2) {
        Node* word = getThe(str.substr(type + 1).c_str());
        addStatement(word, Instance, subject, true);
    }

    // todo: zu (ort/name) _der_ (nicht:name) bei von auf_der auf am (Angriff_)gegen (Schlacht_)um...
    //    free(str);
}

bool abstractsLoaded = true;

Node* getThe(string thing, Node* type,bool dissect) {
    return getThe(thing.data(),type,dissect);
}
Node* getThe(const char* thing, Node* type,bool dissect) {
	if(eq(thing,"of"))// To do: something
		return Owner;
    if (thing == 0) {
        badCount++;
        return 0;
    }
    Node* abstract = getAbstract(thing);
    Node* insta = firstInstance(abstract,type);// todo: best?
    if (insta)
		return insta;

    if (type)
        insta = add(thing, type->id);
    else
        insta = add(thing, Object->kind);
	if(insta==0){
		p("add node failed!!");
		ps(thing);
		return 0;
	}

    if(dissect)
        dissectWord(insta); // dont remove!
    return insta;
}

Node* hasWord(const char* thingy) {
    long h = hash(thingy);
    Ahash* found = &abstracts[h % maxNodes];// TODO: abstract=first word!!! (with new 'next' ptr!)
    if (found && found->next==0&& found->abstract && eq(found->abstract->name,thingy))// tolower??
		return found->abstract;
    //&&eq(found->abstract->name, thingy)  // debug ; expensive!

	int tries=0;// cycle bugs
    while (found >= abstracts && found<&extrahash[maxNodes] && found->next && tries++<100) {
//        if (eq(thingy, "near"))
//            h = h;
        if (checkNode(found->abstract) && eq(found->abstract->name, thingy))//teuer? nö, if 1.letter differs
            return found->abstract;
        if(found->next==found)break;
        found = found->next;
    }
    return 0;
}

bool doDissect = false;
Node* getAbstract(const char* thing) {// AND CREATE!
    if (thing == 0) {
        badCount++;
        return 0;
    }
    char* thingy = (char*) malloc(1000); // todo: replace \\" ...
    strcpy(thingy, thing);
    fixNewline(thingy);
    Node* abstract = hasWord(thingy);
    if (abstract)return abstract;
    abstract = add(thingy, abstractId, abstractId); // abstract context !!
    if (!abstract) {
		p("out of memory!");
        throw "out of memory exception";
		return 0;
    }
    insertAbstractHash(hash(thingy), abstract);
    if (strstr(thing, "_") || strstr(thing, " ") || strstr(thing, ".") && doDissect)
        dissectParent(abstract);

    //    for(int i=0;i<c->nodeCount;i++){
    //        Node* n=&c->nodes[i];
    //        char* nname=n->name;
    //        if(eq(name,nname))
    //            addStatement(abstract,Instance,n);
    //    }
    //    delete[] name;
//    free(thingy);// OK?
    return abstract;
}
//Node* getThe(const char* word){
//    Node* n=find(current_context,word,true);
//    if(n==0)n=add(word);
//    return n;
//}

Node* getClass(const char* word) {
    return getThe(word);
}

void showStatement(Statement* s) {
    if (quiet)return;

    Context* c = currentContext();
    if (s < c->statements || s > &c->statements[maxStatements0]) {
        p("illegal statement:");
        printf("%016llX", s);
        return;
    }
    if (s == null)return;
    if (s->id != null && checkNode(s->Subject,s->subject) && checkNode(s->Predicate,s->predicate)&& checkNode(s->Object,s->object))
//        if(s->Object->value.number)
//            printf("%d\t%s\t\t%s\t\t%g %s\t%d\t%d\t%d\n", s->id, s->Subject->name, s->Predicate->name, s->Object->value.number,s->Object->name, s->subject, s->predicate, s->object);
//        else
            printf("%d\t%s\t\t%s\t\t%s\t%d\t%d\t%d\n", s->id, s->Subject->name, s->Predicate->name, s->Object->name, s->subject, s->predicate, s->object);
    else
        printf("#%d %d->%d->%d  [%016llX]\n", s->id, s->subject, s->predicate, s->object, s);
    flush();
    // printf("%s->%s->%s\n",s->Subject->name,s->Predicate->name,s->Object->name);

}

void show(Node* n, bool showStatements) {//=true
    if (quiet)return;
    if (!checkNode(n))return;
    // Context* c=getContext(n->context);
    // if(c != null && c->name!=null)
    // printf("Node: context:%s#%d id=%d name=%s statementCount=%d\n",c->name, c->id,n->id,n->name,n->statementCount);
    //    printf("%s  (#%d)\n", n->name, n->id);
	string img = "";
	if(hasWord(n->name))
    img=getImage(n->name);
//    if(n->value.number)
//    printf("%d\t%g %s\t%s\n", n->id,n->value.number, n->name, img.data());
//    else
    printf("%d\t%s\t%s\n", n->id, n->name, img.data());
    //	printf("Node#%016llX: context:%d id=%d name=%s statementCount=%d kind=%d\n",n,n->context,n->id,n->name,n->statementCount,n->kind);
    // else
    // printf("Node: id=%d name=%s statementCount=%d\n",n->id,n->name,n->statementCount);
    int i = 0;
    int maxShowStatements=1000;//hm
    if (showStatements)
        for (; i < min(n->statementCount,maxShowStatements); i++) {
            showStatement(getStatement(n, i));
        }
    if (showStatements)
        p("--------------");
}

Node* showNr(int context, int id) {

    Context* c = getContext(context); //contexts[context];
    // showContext(context);

    if (id > maxNodes) {
        if (quiet)return 0;
        printf("int context %d, int id %d id>maxNodes", context, id);
        return 0;
    }
    Node* n = &c->nodes[id];
    if (!checkNode(n, id))return 0;
    show(n);
    return n;
}

Node* findWord(int context, const char* word, bool first) {//=false
    // pi(context);
    Context* c = getContext(context);
    Node* found = 0;
    int max = c->nodeCount; // maxNodes;
    for (int i = 0; i < max; i++) {
        Node* n = &c->nodes[i];
        if (n == null || n->name == null || word == null || n->id == 0 || n->context == 0)
            continue;
		if(!checkNode(n,n->id,true,true))
			continue;
        if (eq(n->name, word, true)) {
            found = n;
            if (!quiet)
                printf("found node %s in context %d\n", word, context);
            show(n);
            if (first)
                return n;
        }
    }
    if (found == 0) {
        if (!quiet)
            printf("cant find node %s in context %d\n", word, context);
    }
    return found;
}

bool areAll(Node* a,Node* b){
    return isA4(a,b, 0,0);
}

Statement* findStatement(Node* subject, Node* predicate, Node* object, int recurse, bool semantic, bool symmetric,bool semanticPredicate) {
    if (recurse > 0)
        recurse++;
    else recurse = maxRecursions;
    if (recurse > maxRecursions || subject == 0)
        return false;
    //    if(recurse>maxRecursions/3)semantic = false;
    for (int i = 0; i < subject->statementCount; i++) {
        Statement* s = getStatement(subject, i);
        if (s == 0) {
            badCount++;
            ps("!s!");
            return 0;
        };
//		showStatement(s);// debug!!!
//        if(s->context != current_context)continue;// only queryContext
#ifdef use_instance_gap
        if(s->Predicate==subject||i>1&&s->Predicate==Instance&&predicate!=Instance||i>1&&s->Predicate==Type&&predicate!=Type){
//            ps("skipping Predicate/Instance/Kind statements");
//            continue;
            break;// todo : make sure statements are ordered!
        }
#endif
//        showStatement(s); // to reveal 'bad' runs (first+name) ... !!!
        bool subjectMatch = (s->Subject == subject || subject == Any);
        bool predicateMatch = (s->Predicate == predicate || predicate == Any);
        predicateMatch = predicateMatch || predicate == Instance && s->Predicate == SubClass;
        predicateMatch = predicateMatch || predicate == SubClass && s->Predicate == Instance;
        bool objectMatch = (s->Object == object || object == Any);
        if (subjectMatch && predicateMatch && objectMatch)return s;

        bool subjectMatchReverse = s->Object == subject; //s->Subject == object;
        bool predicateMatchReverse = (symmetric && s->Predicate == predicate || predicate == Any); // || inverse
        predicateMatchReverse = predicateMatchReverse || predicate == Instance && s->Predicate == Type;
        predicateMatchReverse = predicateMatchReverse || predicate == Type && s->Predicate == Instance;
        predicateMatchReverse = predicateMatchReverse || predicate == SuperClass && s->Predicate == SubClass;
        predicateMatchReverse = predicateMatchReverse || predicate == SubClass && s->Predicate == SuperClass;
        // sick:
//        predicateMatchReverse = predicateMatchReverse || predicate == Instance && s->Predicate == SuperClass;
//        predicateMatchReverse = predicateMatchReverse || predicate == SuperClass && s->Predicate == Instance;
//        predicateMatchReverse = predicateMatchReverse || predicate == SubClass && s->Predicate == Type;
//        predicateMatchReverse = predicateMatchReverse || predicate == Type && s->Predicate == SubClass;
        bool objectMatchReverse = s->Subject == object; //s->Object == subject;
        if (subjectMatchReverse && predicateMatchReverse && objectMatchReverse)return s;

        if (!semantic)continue;
		///////////////////////// SEMANTIC /////////////////////////////
        subjectMatch = subjectMatch || semantic && isA4(s->Subject, subject, recurse, semantic);
        if (subjectMatch)
            objectMatch = objectMatch || semantic && isA4(s->Object, object, recurse, semantic);
        if (subjectMatch && objectMatch){
            if(semanticPredicate)
                predicateMatch = predicateMatch || semantic && areAll(s->Predicate, predicate);//isA4, recurse, semantic);
            else
                predicateMatch = predicateMatch || semantic && isA4(s->Predicate, predicate, recurse, semantic);//isA4, recurse, semantic);
        }
        if (subjectMatch && predicateMatch && objectMatch)return s;

        subjectMatchReverse = subjectMatchReverse || isA4(s->Subject, object,0,0);;//todo recurse, semantic);
        predicateMatchReverse = predicateMatchReverse|| predicate == Any;
		predicateMatchReverse = predicateMatchReverse|| (symmetric && areAll(s->Predicate, predicate) ); // || inverse //isA4 recurse, semantic)
        objectMatchReverse = objectMatchReverse || isA4(s->Object, subject,0,0);;//todo recurse, semantic);
        //			if(!semantic &&!recurse){
        if (subjectMatchReverse && predicateMatchReverse && objectMatchReverse)return s;
		///////////////////////// END SEMANTIC /////////////////////////////
    }
    return null;
}
void removeStatement(Node* n, Statement* s){
    if(!n||!s)return;
    Statement* last=0;
     for (int i = 0; i < n->statementCount; i++) {
        Statement* st = getStatement(n, i);
        if(st==s){
            if(last==0){
                if(s->Subject==n)n->firstStatement=s->nextSubjectStatement;
                if(s->Predicate==n)n->firstStatement=s->nextPredicateStatement;
                if(s->Object==n)n->firstStatement=s->nextObjectStatement;
            }
            else{
                if(s->Subject==n)last->nextSubjectStatement=s->nextSubjectStatement;
                if(s->Predicate==n)last->nextPredicateStatement=s->nextPredicateStatement;
                if(s->Object==n)last->nextObjectStatement=s->nextObjectStatement;
            }
        }
        last=st;
    }
}
void deleteStatement(Statement* s){
    removeStatement(s->Subject,s);
    removeStatement(s->Predicate,s);
    removeStatement(s->Object,s);
    memset(s,0, sizeof (Statement));
}
void remove(char* data){
	Context* context=&contexts[current_context];
	int id=atoi(data);
	if(id<0)
		deleteNode(getThe(data));
//		deleteNode(getAbstract(data));// DANGER!!
	else
		if(checkNode(&context->nodes[id],id,false,false))
			deleteNode(&context->nodes[id]);
		else if(checkStatement(&context->statements[id],id))
			deleteStatement(&context->statements[id]);
		else ps("No such node: "+string(data));
}

void remove(string* s){
	remove(s->c_str());
}

void deleteNode(Node* n){
    if(n->kind==abstractId){
        NodeVector nv=instanceFilter(n);
        for (int i = 0; i < nv.size(); i++) {
            Node* n = nv[i];
            deleteNode(n);
        }
    }
    for (int i = 0; i < n->statementCount; i++) {
        Statement* s = getStatement(n, i);
        deleteStatement(s);
    }
    memset(n,0, sizeof (Node));// hole in context!
}

void deleteStatements(Node* n){
    for (int i = 0; i < n->statementCount; i++) {
        Statement* s = getStatement(n, i);
        deleteStatement(s);
    }
}
Node* parseValue(const char* aname){
    if(contains(aname," ")){
        string s=(aname);
        string unit=s.substr(s.find(" ")+1);
        return value(aname,atof(aname),unit.data());
    }
    else return value(aname,atof(aname),0);
}

//#include <stdlib.h>
//#include <math.h> //floor
Node* value(const char* aname, double v,const char* unit) {
    char name[1000];
    if(aname)strcpy(name,aname);
    if (unit) {
        sprintf(name, "%g %s", v,unit); //Use the shorter of %e or %f  3.14 or 24E+35
    } else {
        sprintf(name, "%g", v); //Use the shorter of %e or %f  3.14 or 24E+35
    }
    Node *n=getThe(name);
    if(unit)
           n->kind=getThe(unit)->id;
    else
        n->kind=number;
    n->value.number = v;
    return n;
}

Statement* findStatement(Node* subject, string predicate, string object, int recurse, bool semantic, bool symmetric) {
    if (recurse > 0)recurse++;
    else recurse = maxRecursions;
    if (recurse > maxRecursions)return false;
    for (int i = 0; i < subject->statementCount; i++) {
        Statement* s = getStatement(subject, i);
        if (isA4(s->Predicate, predicate, recurse, semantic))
            if (s->Subject == subject)
                if (isA4(s->Object, object))return s;
                else
                    if (isA4(s->Subject, object))return s; // TODO! if Predicate^-1 !
    }/// ?? unresolved!?
    return null;
}

Node* has(Node* n, string predicate, string object, int recurse, bool semantic, bool symmetric) {
    if (recurse > 0)recurse++;
    else recurse = maxRecursions - 1;
    if (recurse > maxRecursions)return false;
    Statement* s = findStatement(n, predicate, object, recurse, semantic, symmetric);
    if (s != null && s->Subject == n) return s->Object;
    if (s != null && s->Object == n) return s->Subject;
    return 0;
}

Node* has(Node* subject, Node* predicate, Node* object, int recurse, bool semantic, bool symmetric,bool predicatesemantic) {
    if (recurse > 0)recurse++;
    else recurse = maxRecursions - 1;
    if (recurse > maxRecursions)return false;
    if (subject->kind == Abstract->id) {
        NodeVector all = instanceFilter(subject);
        for (int i = 0; i < all.size(); i++) {
            Node* insta = (Node*) all[i];
            Node* resu = has(insta, predicate, object, recurse, semantic, symmetric,predicatesemantic);
            if (resu)return resu;
        }
    }
    //    if(recurse>maxRecursions/3)semantic = false;
    //    printf("findStatement(n %d,predicate %d,object %d,recurse %d,semantic %d,symmetric %d)\n",n,predicate,object,recurse,semantic,symmetric);
    Statement* s = findStatement(subject, predicate, object, recurse, semantic, symmetric,predicatesemantic);
    if (s != null && s->Subject == subject) return s->Object;// a.b=*? return *
    if (s != null && s->Object == subject) return s->Subject;
    return 0;
}

Node* isEqual(Node* subject, Node* object) {

    if (!isA4(subject, object))return 0;
//    if (isA4(subject, object))return subject;
    //    if(subject->kind==object->kind)
    if (subject->value.number == object->value.number)return subject;
    if(atof(subject->name)>0&&atof(subject->name)>atof(object->name))return subject;
    return 0;
}

Node* isGreater(Node* subject, Node* object) {
//            if(subject->kind!=object->kind)return 0;
    if (subject->value.number > object->value.number)return subject;
    int v=atof(subject->name);
    int w=atof(object->name);
    if(v && w && v>w)return subject;
    return 0;
}

Node* isLess(Node* subject, Node* object) {
//            if(subject->kind!=object->kind)return 0;
    if (subject->value.number < object->value.number)return subject;
    int v=atof(subject->name);
    int w=atof(object->name);
    if(v && w && v<w)return subject;
    return 0;
}

Node* isAproxymately(Node* subject, Node* object) {
    if (isEqual(subject, object))return subject;
    //    if(soundex(subject->name)==soundex(object->name))return subject;
    //    if(levin_distance(subject->name,object->name)<3)return subject;
    if (object->value.number == 0 && subject->value.number * subject->value.number > 0.1)return 0;
    float r = subject->value.number / object->value.number;
    if (r * r > 0.9 && r * r < 1.1)return subject;
    return 0;
}

// berlin (size greater 200 sqm)/*float*/
Node* has(Node* subject, Statement* match, int recurse, bool semantic, bool symmetric,bool predicatesemantic) {
    //    if(findStatement(match))
//    if (match->Predicate == Equals)return has(subject, match->Subject, match->Object);

    //todo : iterate all property_values !!
    Node* property_value = has(subject, match->Subject, Any,recurse,semantic,symmetric,predicatesemantic);
    //has(subject, match->Subject);
//    if (!property_value)property_value = has(subject, match->Subject);// second try expensive!
    if (!property_value)
        return false;
    if (match->Predicate == Equals)return isEqual(property_value, match->Object);
    else if (match->Predicate == Greater)return isGreater(property_value, match->Object);
    else if (match->Predicate == Less)return isLess(property_value, match->Object);
    else if (match->Predicate == Circa)return isAproxymately(property_value, match->Object);
    else return has(subject,match->Predicate,match->Object);// match->Subject == Subject?
    return 0;
    //    if(match->Predicate==Range) isGreater and isLess
}
int recursions = 0;

bool hasValue(Node* n) {
    return (*(int*) & n->value) != 0;
}

bool isA4(Node* n, Node* match, int recurse, bool semantic) {
    if (n == match)return true;
    if (!n || !n->name || !match || !match->name)return false; //!!
    if (eq(n->name, match->name))return true;
    if (n->kind == match->id)return true; //
    if(n->id<100 && match->id<100)return false;// danger!
    if (get(n->kind) && eq(get(n->kind)->name, match->name, true))// danger: instance, noun
        return true;
    if (n->kind == match->id)return true;
    if (eq(n->name, match->name, true))return true; //&& n->name==match
    if (n->id == match->id)return true; // how so??? "Type" overwritten by "kind" !!!!
    long badHash = n->id + match->id * 10000000;
    if (yetvisitedIsA[badHash] == 1)return true;
    if (yetvisitedIsA[badHash] == -1)return false;
    //        else if (yetvisitedIsA[n]==null)yetvisitedIsA[n]=true;
    //        else yetvisitedIsA[n]++;

    if (recurse > 0)recurse++;
    //    else recurse=maxRecursions;
    if (recurse > maxRecursions)
        return false;
    runs++;

    if (hasValue(n)) {// false else?
        //        if(isA4(n->kind,match->kind))
        if (n->kind == match->kind) {
            if (n->value.number == match->value.number)return true;
            if (n->value.datetime == match->value.datetime)return true;
            if (n->value.statement == match->value.statement)return true;
            else return false;
        }
    }
//        if (recurse > maxRecursions / 3)
//            semantic = false;
    bool semantic2 = false;
    if (semantic && has(n, Synonym, match, false, false, true)) {
        yetvisitedIsA[badHash] = true;
        return true;
    } // todo:semantic true (level1)
    //    if(semantic && has(n,Plural,match,false,false,true))return true;
    if (semantic && has(n, SuperClass, match, recurse, semantic2, false)) {
        yetvisitedIsA[badHash] = true;
        return true;
    }
    if (semantic && has(n, Type, match, recurse, semantic2, false)) {
        yetvisitedIsA[badHash] = true;
        return true;
    }
    if (semantic && has(match, Instance, n, recurse, semantic2, false)) {
        yetvisitedIsA[badHash] = true;
        return true;
    }
    if (semantic && has(match, SubClass, n, recurse, semantic2, false)) {
        yetvisitedIsA[badHash] = true;
        return true;
    }
    if (semantic && has(n, Label, match, false, false, false)) {
        yetvisitedIsA[badHash] = true;
        return true;
    }
    if (semantic && recurse>0 && findStatement(n, Plural, match, maxRecursions - 1, semantic2)) {
        yetvisitedIsA[badHash] = true;
        return true;
    } //(n,Plural,match,0,true,true))return true;
    if (semantic && recurse>0 && findStatement(match, Plural, n, maxRecursions - 1, semantic2)) {
        yetvisitedIsA[badHash] = true;
        return true;
    } //(n,Plural,match,0,true,true))return true;


    //    if(isA(n,match->name,false,false))return true;// compare by name
    if (n->kind == Abstract->id && recurse < 3) {//|| isA(n,List)
        for (int i = 0; i < n->statementCount; i++) {
            Statement* s = getStatement(n, i);
            if (s == 0)continue; //todo: why!!??
            if (s->Predicate == Instance)
                if (recurse && isA4(s->Object, match, recurse, semantic)) {
                    yetvisitedIsA[badHash] = true;
                    return true;
                }
        }
    }
    yetvisitedIsA[badHash] = -1;
    return false;
}

// todo: a.b=c findMember(b,b) error

Node* findMember(Node* n, string match, int recurse, bool semantic) {
    if (!n)return 0;
    if (recurse > 0)recurse++;
    if (recurse > maxRecursions)return false;
    if (debug)show(n);
    for (int i = 0; i < n->statementCount; i++) {
        Statement* s = getStatement(n, i);// Not using instant gap
        if(!s){badCount++;continue;}
        if (debug)showStatement(s);
        if (isA4(s->Predicate, match, recurse, semantic))
            if (s->Subject == n)return s->Object;
            else return s->Subject;
        if (isA4(s->Object, match, recurse, semantic))return s->Object;
        if (isA4(s->Subject, match, recurse, semantic))return s->Subject;
    }
    return null;
}

// xpath    cities[population>100000, location=europe]
//Statement* toStatement(Node* n)
//Statement* getStatement(Node* n)
Statement* isStatement(Node* n) {
    if (n && n->kind == _statement)
        return n->value.statement;
    return 0;
}




Node* has(const char* n, const char* m) {
    return has(getAbstract(n), getAbstract(m));
}

Node* has(Node* n, Node* m) {
    Node *no=0;
    Node* save = n; // heap data loss !?!
    if (!no)no = has(n, m, Any); // TODO: test
//    findPath(n,m,hasFilter);// Todo new algoritym!

    if (!no)no = has(n, Member, m);
    if (!no)no = has(n, Attribute, m);
    if (!no)no = has(n, Substance, m);
    if (!no)no = has(n, Part, m);
    //    if(!n)n=has(n,Predicate,m);// TODO!
    if (!no)no = has(save, Any, m); //TODO: really?
    return no;
}

void showNodes(NodeVector all, bool showStatements) {
    ps("+++++++++++++++++++++++++++++++++++");
    for (int i = 0; i < all.size(); i++) {
        Node* node = (Node*) all[i];
        show(node, showStatements);
    }
    ps("++++++++++ Hits : +++++++++++++++++");
    pi(all.size());
}
//NodeVector match_all(string data){
//}

//NodeVector find_english(string& data) {
//}

Node* first(NodeVector rows) {
    if (rows.size() > 0) {
        Node* n = (Node *) rows[0];
        return n;
    }
    return 0;
}

Node* last(NodeVector rows) {
    int s = rows.size();
    if (s > 0) {
        Node* n = (Node *) rows[s - 1];
        return n;
    }
    return 0;
}

void initUnits() {
//    printf("Abstracts %016llX\n", abstracts);
    printf("Abstracts %016llX\n", abstracts);
    Ahash* ah = &abstracts[hash("meter") % abstractHashSize];
    if ((char*) ah < root_memory || ah > extrahash) {
        ps("abstracts kaputt");
        collectAbstracts();
    }
    Node* u = getThe("meter", Unit);
    addStatement(a(length), has_the("unit"), u);

    //    addSynonyms(u,"m");
    u = getThe("m^2", Unit);
    u = getThe("km^2", Unit);
    u = getThe("millisecond", Unit);
    u = getThe("mile", Unit);
    u = getThe("km", Unit);
    //    u=add("phone number",String);// normalize!
    //    addSynonyms(u,"m^2","sqm");
}

Statement* learn(string sentence){
	ps("learning "+sentence);
	Statement* s=evaluate(sentence);
	if(checkStatement(s)){
		return s;
	}else{
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


//void cleanAbstracts(Context* c){

Node* firstInstance(Node* abstract, Node* type) {
	if(type==0){
		Statement* last=&(currentContext()->statements[abstract->lastStatement]);
		if(last->Predicate==Instance)return last->Object;
		else return 0;// NO SUCH!!
	}
    for (int i = 0; i < abstract->statementCount; i++) {
        Statement* s = getStatement(abstract, i);
//		showStatement(s);
        if (s == 0) {
			ps("CORRUPTED Statements for");
			showStatement(s);
            badCount++;
            break;
        };
        bool subjectMatch = (s->Subject == abstract || abstract == Any);
        bool predicateMatch = (s->Predicate == Instance);
        bool typeMatch=(type == 0 || s->Object==type || s->Object->kind== type->id);
		typeMatch = typeMatch|| isA4(s->Object, type,maxRecursions,true);//semantic, depth 0
        if (subjectMatch && predicateMatch && typeMatch)return s->Object;

        bool subjectMatchReverse = s->Object == abstract;
        bool predicateMatchReverse = s->Predicate == Type; // || inverse
        bool typeMatchReverse=(type == 0 || s->Subject==type);// isA4(s->Object, type)
        if (subjectMatchReverse && predicateMatchReverse &&  typeMatchReverse)return s->Subject;
    }
    return 0;
}


NodeVector anyFilter(Node* subject) {
     NodeVector all;
for (int i = 0; i < subject->statementCount; i++) {
        Statement* s = getStatement(subject, i);
        if (s == 0) {
            badCount++;
            continue;
        };
        bool subjectMatch = (s->Subject == subject || subject == Any);
        bool subjectMatchReverse = s->Object == subject;

        if (subjectMatch )all.push_back(s->Object);
        if (subjectMatchReverse )all.push_back(s->Subject);
    }
    return all;
}

NodeVector instanceFilter(Node* subject) {
    NodeVector all;
    for (int i = 0; i < subject->statementCount; i++) {
        Statement* s = getStatement(subject, i);
        if (s == 0) {
            badCount++;
            continue;
        };
        bool subjectMatch = (s->Subject == subject || subject == Any);
        bool predicateMatch = (s->Predicate == Instance);

        bool subjectMatchReverse = s->Object == subject;
        bool predicateMatchReverse = s->Predicate == Type; // || inverse

        if (subjectMatch && predicateMatch)all.push_back(s->Object);
        if (subjectMatchReverse && predicateMatchReverse)all.push_back(s->Subject);
    }
    return all;
}

NodeVector hasFilter(Node* subject) {
     NodeVector all;
    for (int i = 0; i < subject->statementCount; i++) {
        Statement* s = getStatement(subject, i);
        if (!checkStatement(s)) {
            badCount++;
            continue;
        };
        bool subjectMatch = (s->Subject == subject || subject == Any);
        bool predicateMatch = (s->Predicate == Member);
        predicateMatch = predicateMatch || s->Predicate == Part;
        predicateMatch = predicateMatch || s->Predicate == Attribute;
        predicateMatch = predicateMatch || s->Predicate == Substance;
        predicateMatch = predicateMatch || s->Predicate == Active;
        predicateMatch = predicateMatch || s->Predicate == To;
        predicateMatch = predicateMatch || s->Predicate == For;
        bool subjectMatchReverse = s->Object == subject;
        bool predicateMatchReverse = s->Predicate == Owner; // || inverse
        predicateMatchReverse = predicateMatchReverse || s->Predicate == By;
        predicateMatchReverse = predicateMatchReverse || s->Predicate == From;

        if (subjectMatch && predicateMatch)all.push_back(s->Object);
        if (subjectMatchReverse && predicateMatchReverse)all.push_back(s->Subject);
//        if (subjectMatch && s->Predicate>1000)all.push_back(s->Object);
     }
    return all;
}

NodeVector parentFilter(Node* subject) {
    NodeVector all;
    for (int i = 0; i < subject->statementCount; i++) {
        Statement* s = getStatement(subject, i);
        if (s == 0) {
            badCount++;
            continue;
        };
        bool subjectMatch = (s->Subject == subject || subject == Any);
        bool predicateMatch = (s->Predicate == Type);
        predicateMatch = predicateMatch || s->Predicate == SuperClass;
        predicateMatch = predicateMatch || s->Predicate == Synonym;
        predicateMatch = predicateMatch || s->Predicate == Plural;

        bool subjectMatchReverse = s->Object == subject;
        bool predicateMatchReverse = s->Predicate == Instance; // || inverse
        predicateMatchReverse = predicateMatchReverse || s->Predicate == Plural;
        predicateMatchReverse = predicateMatchReverse || s->Predicate == Synonym;
        predicateMatchReverse = predicateMatchReverse || s->Predicate == SubClass;

        if (subjectMatch && predicateMatch)all.push_back(s->Object);
        if (subjectMatchReverse && predicateMatchReverse)all.push_back(s->Subject);
        //        if (!semantic)continue;
        //
        //        subjectMatch = subjectMatch || semantic && isA(s->Subject, subject, recurse, semantic);
        //        if (subjectMatch)
        //            objectMatch = objectMatch || semantic && isA(s->Object, object, recurse, semantic);
        //        if (subjectMatch && objectMatch)
        //            predicateMatch = predicateMatch || semantic && isA(s->Predicate, predicate, recurse, semantic);
        //        if (subjectMatch && predicateMatch && objectMatch)return s;
        //
        //        subjectMatchReverse = subjectMatchReverse || isA(s->Subject, object, recurse, semantic);
        //        predicateMatchReverse = predicateMatchReverse || (symmetric && isA(s->Predicate, predicate, recurse, semantic) || predicate == Any); // || inverse
        //        objectMatchReverse = objectMatchReverse || isA(s->Object, subject, recurse, semantic);
        //        //			if(!semantic &&!recurse){
        //        if (subjectMatchReverse && predicateMatchReverse && objectMatchReverse)return s;
    }
    return all;
}



NodeVector shortestPath(Node* from,Node* to ){
	findPath(from,to,anyFilter);
}


int* enqueued;// 'parents'
NodeVector reconstructPath(Node* from,Node* to){
	Node* current=to;
	NodeVector all;
	while(true){
		all.push_back(current);
		int id=enqueued[current->id];
		current=get(id);
		if(!current || id ==0)break;
		if(contains(all,current)){
			ps("LOOOOOP!");
			break;
		}
		show(current,false);
	}
	all.push_back(from);// done
	return all;
}

// todo : memory LEAK? why?
Node* findPath(Node* fro, Node* to, NodeVector (*edgeFilter)(Node*)) {
    //    map<Node*, Node*>enqueued;
    enqueued = (int*) malloc(currentContext()->nodeCount * sizeof (int));
    queue<Node*> q;
    if (enqueued == 0) {
		p("out of memory for findPath");
        throw "out of memory for findPath";
    }
    memset(enqueued, 0, currentContext()->nodeCount * sizeof (int));

    q.push(fro);
    Node* current;

    while (current = q.front()) {
        q.pop();
        if (to == current){
			NodeVector all=reconstructPath(fro,to);
			showNodes(all,false);
			free(enqueued);
			free(q);
			return all;
		}
        if (!checkNode(current, 0, true))
            continue;
        NodeVector all = edgeFilter(current);
        for (int i = 0; i < all.size(); i++) {
            Node* d = (Node*) all[i];
            if (enqueued[d->id])continue;
            if (isA4(d, to)){free(enqueued);return d;}
			if(enqueued[d->id]==0)
				enqueued[d->id] = current->id;
            q.push(d);
        }
    }
    free(enqueued);
    return false;
}

bool isA(Node* fro, Node* to) {
    if (fro->kind == Abstract->id) {
        NodeVector all = instanceFilter(fro);
        for (int i = 0; i < all.size(); i++) {
            Node* d = (Node*) all[i];
            if (findPath(d, to, parentFilter))return true;
        }
    }
    return findPath(fro, to, parentFilter);
}


// all mountains higher than Krakatao
// todo:wordnet bugs: vulgar

void pathFinder() {
    //calloc() initializes the allocated memory to ZERO.
}
//char*


void set(Node* node, Node* property, Node* value){
    Statement *s=findStatement(node,property,value);
    if(s){
        if(!eq(s->Object,value)){
            printf("value already set %s.%s=%s ... replacing with %s",node->name,property->name,s->Object,value->name);
            removeStatement(node,s);// really?? save history?
        }else return;//Wert schon da => nix?
    }
    addStatement(node,property,value,false);
}




bool checkParams(int argc, char *argv[], char* p) {
    string minus = "-";
    string colon = ":";
    string equals = "=";
    for (int i = 1; i <= argc; i++) {
        if (eq(argv[i], p))
            return true;
        if (eq(argv[i], (colon + p[0]).c_str()))// import
            return true;
        if (eq(argv[i], (minus + p).c_str()))
            return true;
        if (eq(argv[i], (minus + minus + p).c_str()))
            return true;
    }
    return false;
}

string getImage(const char* n, int size) {
	Node* a = getAbstract(n);
    Node* i = findMember(a, "wiki_image", false, false);
    if (!i)return "";
    string image = i->name;
    string hash = md5(i->name);
    string base = "http://upload.wikimedia.org/wikipedia/commons/thumb/";
    char ssize[12];
    sprintf(ssize, "%d", size);
    return base + hash[0] + "/" + hash[0] + hash[1] + "/" + image + "/" + ssize + "px-" + image;
}

int main(int argc, char *argv[]) {
    char* data = getenv("QUERY_STRING");
    if(data){
    printf("Content-Type: text/plain;charset=us-ascii\n\n");
    printf("got QUERY_STRING %s",data);
	parse(data);
//	start_server();
	}

    //    signal(SIGSEGV, handler); // only when fully debugged!
    //    signal(SIGINT, SIGINT_handler); // only when fully debugged!
    //    setjmp(loop_context);
    path = argv[0];
    path = path.substr(0, path.rfind("/") + 1);
    if (path.rfind("/dist") != -1) {
        path = path.substr(0, path.rfind("/dist") + 1);
    }
    system(string("cd " + path).c_str());

    if (checkParams(argc, argv, "quiet"))
        quiet = true;
    if (checkParams(argc, argv, "quit"))
        exit(0);
    if (checkParams(argc, argv, "exit"))
        exit(0);

    init();
    //    import();
    if (checkParams(argc, argv, "query")) {
        load();
        string q = replace_all(join(argv, argc), "query ", "");
        parse(q.c_str());
        exit(0);
        //        import();
    }
    if (checkParams(argc, argv, "export"))
        export_csv();
    if (checkParams(argc, argv, "load"))
        load(false);
    if (checkParams(argc, argv, "load_files"))
        load(true);
    if (checkParams(argc, argv, "test")) {
//        load();
//		clean();

//		testBrandNewStuff();
//		if(!hasWord("city")){
//			importAll();
//			collectAbstracts();
//		}
//		if(!hasWord("city")){
//			p("BROKEN");
//			collectAbstracts();
//		}
//        tests();
		p("TEST OK!");
    }

    if (checkParams(argc, argv, "import")) {
        import(argv[2], path.c_str()); // netbase import save
        if (checkParams(argc, argv, "save"))
            save(); // danger
    }
    parse(join(argv, argc).c_str());
    printf("Warnings: %d\n", badCount);
	testBrandNewStuff();
    console();
    //    } catch (std::exception const& ex) {
}

void testBrandNewStuff() {
    ps("test brand new stuff");
//	importWordnet();
	shortestPath(a(bug),a(frog));
//	importCsv("import/wins.csv");
//	if(!hasWord("zip"))
//	importXml("/Users/me/data/base/geo/geolocations/Orte_und_GeopIps_mit_PLZ.xml","city","ort");
	console();
}
