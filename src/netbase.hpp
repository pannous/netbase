#pragma once
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <set> // faster vector!
#include <queue>
using namespace std;

extern bool useSemantics;
extern bool germanLabels;
extern bool exitOnFailure;
extern int maxRecursions;
extern bool debug; //=true;
extern bool showAbstract;
extern bool doDissectAbstracts;
extern int recursions;
extern int badCount;
extern int runs; // algorithm metrics
extern int current_context;
extern bool useYetvisitedIsA;
extern bool autoIds;
extern bool testing;// don't implicitly init relations

extern int resultLimit;
extern int defaultLookupLimit;
extern int lookupLimit;// set per query :( todo : param! todo: filter while iterating 1000000 cities!!

// if test or called from other object

//#define inlineName true // because of char* loss!!!!  TODO!!!

#define CHECK_DUPLICATES true
#define DONT_DISSECT false
#define NO_TYPE 0

//#define name(w) getThe(#w)
#define word(w) getAbstract(#w)
#define some(word) getAbstract(#word)
#define a(word) getAbstract(#word)
//#define a(word) getThe(#word)
#define all(word) getAbstract(#word)
//#define all(word) getThe(#word) //todo!
#define _(w) getThe(#w)
#define the(word) getThe(#word)
#define has_the(word) getThe(#word)// semantic bla
#define have_the(word) getThe(#word)// semantic bla
//#define a(word) Node* word=getThe(#word);
#define that(word) Node* word=getThe(#word)  
#define der(word) Node* word=getThe(#word)
#define die(word) Node* word=getThe(#word)
#define das(word) Node* word=getThe(#word)
#define ein(word) Node* word=getThe(#word)
#define eine(word) Node* word=getThe(#word)
//#define get(word) Node* word=getThe(#word)     ;




//static int sizeOfSharedMemory= 0x0f000000;//1000*1000*400;// /* 400MB shared memory segment */
static const bool multipleContexts = false;
extern string path;
extern string data_path;
extern string import_path;

extern bool virgin_memory;

//static const int maxStatementsPerNode = 10000; // cant make dynamic if used as array[100]
static const int maxContexts = 1000;
extern map<int, int> wn_map;
extern map<int, int> wn_map2;

struct Statement;

struct Node;
//class Node;
typedef union Value {
        // why not just save values+text in 'name' as string??

    char* text; // wiki abstracts etc
    void* data; // byte[], same as ^^
    //    char* name;
    //    int integer;
    //    float floatValue;
    long datetime; // milliseconds? Date*? HOW really?
    //    long longValue;// 8 bytes

    double number; // 8 bytes OK
    // why not just save ints as string?
    // 1) faster comparison   bottleneck factor n==nr digits or is atoi part of cpu?
    // 2) memory. really?? only if value field is used >80% of the time!

	Node* node; // THE ONE in abstracts type --- cycle !---
    Statement* statement; // overhead OK!!! 8 bytes on 64bit machines  (Statement too long, pointer OK)
}Value;

extern "C" const char* getName(int node);

// NEVER USE STRUCTS WITHOUT POINTER!!
// only pointers will edit real data! otherwise you just recieve (and manipulate) a copy of a struct (when assigning s=structs[i])!
// reduce from 0x20 bytes to name,kind, firstStatement == 0x0b bytes!
// 1 nit == 4+1 words =20bytes
extern "C"
typedef struct Node {
//    class Node{
    public:
    int id; //implicit -> redundant
//    long name; // see value for float etc
    char* name;//(){return getName(id);}
    int kind; // abstract,node,person,year, m^2     // via first slot? nah
    //int context; //implicit   | short context_id or int node_id
    //float rank;
    int statementCount; //explicit, can be made implicit and replaced with iterator
    int firstStatement;
	int lastStatement;// remove
    Value value; // for statements, numbers  WASTE!!! remove
//	bool operator<(const Node *rhs) const {
//		return statementCount > rhs->statementCount;
//	}
//	bool operator() (Node* a, Node* b) const
//	{
//		return a->statementCount > b->statementCount;
//	}
//	Node* type(){
//		Statement* s=findStatement(this, getThe("type"), 0);
//		return s->Object();
//	}
    // INDEX
    // Node* index;//nur #properties+1 Nits!!
    // class country{ population{property:0} capital{property:1} }
	// germany.index[0]=80Mio .index[1]=Berlin

//	virtual int key() const { return statementCount; }
}Node ;

// norway captial oslo
// oslo population 325235
// all captials with more than 30

// units
// beth age '29 years'
// oldest -> sort by old
typedef struct Ahash {
    int abstract;
    int next;
//    Node* abstract;
//    Ahash* next;
}Ahash ;

// safe:

//typedef struct Context;
//Context* getContext(int contextId);



// S13425 beth likes apple
// Node3254 value=S13425
// S      karsten agrees <Node3254>
// S      Node3254/S13425 is wrong
// 1 Stit == 3 words

extern "C" Node* get(int NodeId);
extern "C" int getStatementId(long pointer);

typedef class Statement {
//class Statement { //OK!!
//typedef struct Statement {
public:
//    int id; // implicit?
    int id(){ // implicit
    		return getStatementId((long)this);
    }
//#ifdef useContext
    int context; //    needed for pattern() -> find...     todo: via :NODE!?!
//#endif
#ifdef explicitNodes
    Node* Subject; // as function? nee as it is!! transpose on context load
    Node* Predicate;
    Node* Object;
#else
    Node* Subject(){return get(subject);}
    Node* Predicate(){return get(predicate);}
    Node* Object(){return get(object);}
#endif
//    REORDER NEEDS NEW INDEX ON ALL SERVERS +JAVA!
    int subject; // implicit if using explicitNodes: Subject->id
    int predicate;
    int object;

    int nextSubjectStatement;
    int nextPredicateStatement;
    int nextObjectStatement;

    //  Node* meta; for reification, too expensive, how else now??
    // int subjectContext;//... na! nur in externer DB!
} Statement;
// manipulate via other statements (confidence, truth, author(!), ...)

// union: context->hasNode->0 , name
//typedef vector<Node*> NodeVector;
//std::set O(log n) time - much more efficient with large numbers of elements.
//std::set also guarantees all the added elements are unique
typedef set<Node*> NodeSet;
typedef vector<Node*> NodeVector;
typedef queue<Node*> NodeQueue;
typedef Node** NodeList;

//typedef (NodeVector (*edgeFilter)(Node*)) NodeFilter;
//typedef Node* NodeList;

class Facet {
public:

    Facet() {
        values = new map<Node*, int>();
        //        In this case, the pointer variable must be explicitly deallocated withthe delete operator.
        views = 0;
        hits = 0;
        maxHits = 0;
        rank = 0;
    }

    ~Facet() {
        printf("~Facet\n");
        //        when an object variable goes out of scope, its destructor is called automatically.
    }
    Node* field;
    map<Node*, int> *values; //count
    int views;
    int hits;
    int maxHits;
    float rank;
    Node* type; //node,string,date,int,geo,float
    //    Pair[] ranges;// how?
};

//You cannot create an array of references (or anything similar) in C++.
//typedef vector<Facet&> FacetVector;

typedef vector<Statement*> StatementVector;
typedef vector<Facet*> FacetVector;
enum QueryType{nodeQuery,xmlQuery,sqlQuery};
class Query {
public:

    Query() {
        queryType=nodeQuery;
        depth = maxRecursions;
		recursion=0;
        start = 0;
        page = 0;
        hitsPerPage = 10; //rows=10;
        limit = 100; // was mit facetten???
        lookuplimit = 10000;// defaultLookupLimit;// INF!
        maxFacets=20;
//		recursions=0;
        autoFacet = true;
        semantic = true;
        predicatesemantic=true;
        matchNames = true;
    }
    enum QueryType queryType;
    int start; //    int offset; == page* hitPerPage
    //    int end;//     == start + hitsPerPage
    int page; //    int offset;
    int hitsPerPage; // rows
    int limit;
    int lookuplimit;
    int depth; //=maxRecursions;
    bool semantic;
    bool predicatesemantic;
    bool matchNames;
    bool autoFacet; //=true;
    int maxFacets; //=10;//first come? get 30 cut 20?
    NodeVector keywords; //and
    Node* keyword;
    NodeVector fields;
    StatementVector filters;
    FacetVector facets;
    map<Node*, Facet*> facetMap;

    // found
    // filled by algorithm:
    int recursion; // depth step!
    //    NodeList classes;// filled by algorithm
    //    NodeList instances;// car1 car2 car3
    NodeVector classes; // filled by algorithm
    NodeVector instances; // car1 car2 car3
    Node* outputType;
    int resultCount; // == instances.count? <= rows?
    int totalResults; // == instances.count?
    char* result;
    int runs; // for stats: calls per search
    map<Node*, NodeList> values; //number_plate=> number_plate1(@car1) number_plate2...
};

typedef struct Context {
    int id;
    char name[100];
    char* nodeNames;
    long currentNameSlot;
    int nodeCount;
    int lastNode;
    int extrahashNr;
    int statementCount; //first 1000 reserved!
    bool use_logic;
    // Node nodes[maxNodes];
    // Statement statements[maxStatements];
    Node* nodes;
    Statement* statements;
    //	int statementArrays[10000000];
    //	int* statementArrays;
    //	int lastStatementArray;
} Context;

extern "C" int nodeCount();
extern "C" int statementCount();
extern "C" int nextId();
// Relations
extern Node* Unknown;
extern Node* Antonym;
extern Node* Parent; //=
extern Node* SuperClass;
extern Node* SubClass;
extern Node* Owner;
extern Node* Member;
extern Node* Synonym;
extern Node* Weight;
extern Node* Type; // object->class   class->class see SuperClass
extern Node* Instance;
extern Node* Active;
extern Node* Passive;
extern Node* Tag;
extern Node* Label;
extern Node* Labeled;
extern Node* Labels;
extern Node* LabeledNode;
extern Node* Category; // SuperClass or Type
extern Node* SubContext;
extern Node* SuperContext;

//Node* Attribute;     // ld:  duplicate symbol
//extern Node* Attribute;// BAD: each source file has own value!
extern Node* Attribute; //ok, if Node* Attribute declared in netbase.cpp


// basic types:
extern Node* Internal;
extern Node* _Node;
extern Node* Thing;
extern Node* Item;
extern Node* Abstract; // Thing
extern Node* Class;
extern Node* Object;
extern Node* Relation;
extern Node* Reification;// Statement: (I like you)
extern Node* Pattern;// temporary Reification

// semantic types:
extern Node* Person;
extern Node* Noun;
extern Node* Verb;
extern Node* Adjective;
extern Node* Adverb;
extern Node* Number;
extern Node* Unit;
extern Node* String;
extern Node* Float;
extern Node* Date;
//
extern Node* Translation;
extern Node* Plural;
extern Node* And;
extern Node* Or;
extern Node* Not;
extern Node* Any; // wildcard

//
extern Node* Equals;
extern Node* Greater;
extern Node* Less; //smaller
extern Node* Between;
extern Node* Circa;
extern Node* Much;
extern Node* Very;
extern Node* Contains;
extern Node* StartsWith;
extern Node* EndsWith;

typedef Node* N;
typedef Context* C;
typedef Statement* S;
typedef NodeVector Ns;
typedef NodeVector NV;// ok no &NodeVector needed
typedef NodeSet NS;
typedef vector<char*> VC;
typedef vector<char*> CV;
typedef vector<char*> VCC;
typedef vector<char*> CCV;



////////////////////////
void flush();
char* name(Node* node);
extern "C" Node* getNode(int node);
extern "C" Node* getNodeP(int node);
extern "C" Node* getNodeS(int node);
extern "C" int getId(char* node);
//extern "C" Node* getNode(char* node);

Context* getContext(int contextId=0/*wordnet*/);
void showContext(int nr);
void showContext(Context* cp);
void showNodes(NodeSet all, bool showStatements = false,bool showRelation=false,bool showAbstracts=false);
NodeVector showNodes(NodeVector all, bool showStatements = false,bool showRelation=false,bool showAbstracts=false); // bool justTitles==showStatements
NodeVector show(NodeVector all);// alias
NodeVector showWrap(Node* n);
//string query2(string s,int limit=defaultLimit);
//string query(Query& q);
Node* initNode(Node* node, int id, const char* nodeName, int kind, int contextId);
extern "C" Node* add(const char* nodeName, int kind = /*_node*/ -101, int contextId = current_context);
//bool checkNode(int nodeId, bool checkStatements= false, bool checkNames = false);
//bool checkNode(Node* node, int nodeId = -1, bool checkStatements = false, bool checkNames = false);
bool checkNode(int nodeId, bool checkStatements= false, bool checkNames = false,bool report=true);
bool checkNode(Node* node, int nodeId = -1, bool checkStatements = false, bool checkNames = false,bool report=true);
bool addStatementToNode(Node* node, int statementNr,bool force_insert_at_start);
bool addStatementToNodeDirect(Node* node, int statementNr);
bool addStatementToNodeWithInstanceGap(Node* node, int statementNr);

extern "C" Statement* addStatement4(int contextId, int subjectId, int predicateId, int objectId, bool check = true);
extern "C" Statement* addStatement(Node* subject, Node* predicate, Node* object, bool checkDuplicate = true,bool force_insert_at_start=false);
//inline
		Node* get(const char* node);
//inline
		Node* get(char* node);
//inline
Node* get(int NodeId);

//extern "C" /* <== don't mingle name! */ inline
#ifndef _context
static Context* context;
#endif
extern "C" Node* getAbstract(const char* word);
Node* getAbstract(string thing);
int collectAbstracts(bool clear=false);
int collectInstances();
unsigned int hash(const char *str); //unsigned
//unsigned long hash(const char *str); //unsigned
//Node* getThe(const char* word, Node* type = 0,bool dissect=true);
//Node* getThe(string thing, Node* type=0,bool dissect=true);
Node* getThe(Node* abstract, Node* type=0);
extern "C" Node* getThe(const char* word, Node* type = 0);//,bool dissect=false);
Node* getThe(string thing, Node* type=0);//,bool dissect=false);
extern "C" Node* getNew(const char* thing, Node* type=0);//, bool dissect=false);
Node* getClass(const char* word, Node* hint=0);// ?  apple vs Apple ! same as getThe NOW
Node* getSingleton(const char* thing, Node* type=0,bool addType=true) ;
//Node* getClass(string word);
//extern "C"
void showStatement(Statement* s);
extern "C" void showStatement(int id);
bool show(Node* n, bool showStatements = true);
void show(Statement * s);
//extern "C"
Node * showNode(Node* n);
extern "C" Node* showNode(int id);
void testBrandNewStuff();

char* initContext(Context*);
Node* hasWord(const char* thingy,bool seo=false);
extern "C" bool hasNode(const char* thingy);
string getImage(const char* n, int size = 150,bool thumb=false);
string getImage(Node* a, int size=150,bool thumb=false);


Node* value(const char* name, double v,const char* unit);
Node * value(const char* aname, double v, Node* unit = 0);
extern "C" void saveData(int node,void* data,int size,bool copy);
extern "C" void* getData(int node);
extern "C" Value getValue(int node);
extern "C" int valueId(const char* aname, double v, int unit = 0);
//Node * value(const char* aname, Value v, Node* unit = 0);
Node* parseValue(const char* aname);



extern "C" void setLabel(Node* n, const char* label,bool addInstance=true,bool renameInstances=false);
extern "C" void setName(int node, const char* label);
Statement* pattern(Node* subject, Node* predicate, Node* object);
Statement* isStatement(Node* n);// to / get Statement


extern "C"
Statement * nextStatement(int node,Statement* current);
Statement* nextStatement(Node* n,Statement* current,bool stopAtInstances=false);
extern "C" Statement* getStatement(int id,int context_id=current_context);
Statement* getStatementNr(Node* n, int nr,bool firstInstanceGap=false);

//NodeVector& all_instances(Node* type, int recurse , int limit = defaultLimit);
//NodeVector& all_instances(Node* type);
//NodeVector all(Node* type,Node* slot,int recurse);
Node* findMember(Node* n, string match, int recurse = false, bool semantic = useSemantics);
//Node* findMatch(Node* n, const char* match);
//NodeVector filter(Query& q, Node* _filter);
//NodeVector filter(NodeVector all, char* matches);
//NodeVector evaluate_sql(string data, int limit = defaultLimit);
//NodeVector query(string data, int limit = defaultLimit);
//Query parseQuery(string s,int limit=100);
//Node* evaluate(string data);
Statement* evaluate(string data);
Node* match(string data);
NodeVector match_all(string data);
NodeVector find_english(string& data);
Node* first(NodeVector rows);
Node* last(NodeVector rows);
//Node* learn(string data);
//extern "C" Statement* learn(string& data);
extern "C" Statement* learn(const char* data);
extern "C" void deleteNode(int id);
void deleteNode(Node* n);
void deleteStatements(Node* n);
extern "C" void deleteStatement(int id);
void deleteStatement(Statement* s);
int countInstances(Node* n);
Node* isEqual(Node* subject, Node* object);
Node* isGreater(Node* subject, Node* object);
Node* isLess(Node* subject, Node* object);
Node* isAproxymately(Node* subject, Node* object);


extern "C" Node* has(Node* n, Node* m);
Node* has(const char* n, const char* m);
Node* has(Node* n, Statement* s, int recurse = true, bool semantic = true, bool symmetric = false,bool predicatesemantic=true);
Node* has(Node* subject, string predicate, string object, int recurse = true, bool semantic = true, bool symmetric = false);
Node* has(Node* subject, Node* predicate, Node* object, int recurse = true, bool semantic = true, bool symmetric = false,bool predicatesemantic=true, bool matchName=false);

//void initRelations();
Node* add_force(int contextId, int id, const char* nodeName, int kind);

#define is Parent
#define is_a Parent
#define are Parent
#define does_have Member
#define has_a Member
#define have Member

//extern map<long, Node*>* abstracts;
extern Ahash* abstracts; // Node name hash
extern Ahash* extrahash; // for hashes that are not unique, increasing!!!
extern map < Node*, bool> yetvisited;
extern map <double, short> yetvisitedIsA;
//Ahash* getAbstractHash(int hashkey);
//Node* getAbstractHash(const char* word);
//Ahash* insertAbstractHash(long hashkey, Node* abstract); //debug only
Ahash* insertAbstractHash(unsigned int hashkey, Node* abstract,bool overwrite=false,bool seo=false); //debug only
Ahash* insertAbstractHash(Node* abstract,bool overwrite=false);

void deleteWord(string* s);
void deleteWord(const char* data,bool completely=true);
Node* reify(Statement* s);
void show(vector<char*>& v);
bool checkStatement(Statement *s,bool checkSPOs=false,bool checkNamesOfSPOs=false);
//bool checkStatement(Statement *s);
void checkRootContext();
Node* number(int n);
extern "C" Node* getProperty(Node* n,const char* s,int limit=0);
Node* getProperty(Node* node, Node* key,int limit);
Node* getPropertyDummy(const char* id);
void dissectParent(Node* subject,bool checkDuplicates=false);
Node* dissectWord(Node* subject,bool checkDuplicates=false);
Node* mergeNode(Node* target,Node* node);
Node* mergeAll(const char* target);
void replay();
//extern "C"  C-linkage specified, but returns user-defined type 'NodeVector' (aka 'vector<Node *>') which is incompatible with C
NodeVector parse(const char* data,bool safeMode=true);// and act -> extern "C" execute
void fixCurrent();
extern "C" void setKind(int id,int kind);
extern "C" Node* save(Node* n);
extern "C" Node* getType(Node* n);// != kind!
extern "C" char* getText(Node* n);
static char* NO_TEXT="";
#define _MAIN_
int main(int argc, char *argv[]);
extern "C" int test2();// extern "C" -> don't mangle!
extern Context* contexts; //[maxContexts];
extern Statement* statement_root;
extern Context* context_root; // else: void value not ignored as it ought to be
extern Node* abstract_root;
extern Node* node_root;
extern char* name_root;
extern int* freebaseKey_root;// keyhash-> NodeId 'map'
//extern Node** keyhash_root;
void bad();
void show(NodeSet& all);
void addSeo(Node* n);
Node *getSeo(const char* thingy);
//extern
//#pragma warnings_off

static int nodeSize=sizeof(Node);// 40
static int statementSize=sizeof(Statement);// 26 after refactor !! // was 56!!
static int ahashSize=sizeof(Ahash);
static int MB=1048576;
static long GB=1024*MB;
static int million=MB;
static long billion=GB;
#if defined(__APPLE__) // TODO! #if defined use wordnet!
static int propertySlots=1000000;// PROPERTY RELATION SLOTS >-1000 internal, <-10000 wikidata <-20000 wordnet!
#else
static int propertySlots=20000;// PROPERTY RELATION SLOTS >-1000 internal, <-10000 wikidata
#endif
// 200000-317658 for wordnet + other !

//# sudo sysctl -w kern.sysv.shmmax=2147483648 # => 2GB !!

// FREEBASE: 600.000.000 Statements !!!
// todo: via getenv
#if defined(__APPLE__)
static long maxNodes /*max 32bit=4GB!*/= 20*million;// long would need a new structure!!
static long maxStatements = maxNodes*2;// *10 = crude average of Statements per Node (yago:12!!)
#else
static long maxNodes = 100*million;
static long maxStatements = maxNodes*2;// was: 1*!
#endif
//static long abstractHashSize = maxNodes*ahashSize;
static long contextOffset=0x800000;//0x10000;
static int averageNameLength =20;// for amazon! else 20
static long maxChars=maxNodes * averageNameLength;
static int bytesPerNode=(nodeSize+averageNameLength);//+ahashSize*2
static long sizeOfSharedMemory =contextOffset+ maxNodes*bytesPerNode+maxStatements*statementSize;
static long stupidCompiler=billion+ahashSize+sizeOfSharedMemory;//abstractHashSize // against unused ahashSize ...

void buildSeoIndex();
