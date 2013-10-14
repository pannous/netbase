#pragma once
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <queue>
using namespace std;


extern bool exitOnFailure;
extern int maxRecursions;
extern bool debug; //=true;
extern bool showAbstract;
extern char* root_memory; // else: void value not ignored as it ought to be
extern int recursions;
extern int badCount;
extern int runs; // algorithm metrics
extern int current_context;

// if test or called from other object
//#define inlineName true // because of char* loss!!!!  TODO!!!
#define DONT_CHECK_DUPLICATES false
#define DONT_DISSECT false
#define NO_TYPE 0
#define CHECK_DUPLICATES true

//#define name(w) getThe(#w)
#define word(w) getAbstract(#w)
#define some(word) getAbstract(#word)
#define a(word) getAbstract(#word)
#define _(w) getThe(#w)
#define the(word) getThe(#word)
#define has_the(word) getThe(#word)// semantic bla
#define all(word) getThe(#word) //todo!
//#define a(word) Node* word=getThe(#word);
#define that(word) Node* word=getThe(#word)  ;
#define der(word) Node* word=getThe(#word)  ;
#define die(word) Node* word=getThe(#word)   ;
#define das(word) Node* word=getThe(#word)    ;
#define ein(word) Node* word=getThe(#word)     ;
#define eine(word) Node* word=getThe(#word)     ;
//#define get(word) Node* word=getThe(#word)     ;




//static int sizeOfSharedMemory= 0x0f000000;//1000*1000*400;// /* 400MB shared memory segment */
//int maxNodes();
int maxStatements();
static const bool multipleContexts = false;
extern string path;
extern string data_path;
extern string import_path;

extern bool virgin_memory;

static const int maxStatementsPerNode = 10000; // cant make dynamic if used as array[100]
static const int maxContexts = 1000;
extern map<int, int> wn_map;
extern map<int, int> wn_map2;

static const char* NIL_string = "NIL";

struct Statement;
// why not just save ints as string?
// 1) faster comparison   bottleneck factor n==nr digits or is atoi part of cpu?
// 2) memory. really?? only if value field is used >80% of the time!

typedef struct Node;
typedef union Value {
    char* text; // wiki abstracts etc
    //    char* name;
    //    int integer;
    //    float floatValue;
    long datetime; // milliseconds? Date*? HOW really?
    //    long longValue;// 8 bytes
    double number; // 8 bytes OK
    //    Statement statement;// too long
	Node* node; // THE ONE in abstracts type --- cycle !---
    Statement* statement; // overhead OK!!! 8 bytes on 64bit machines
}Value;


// NEVER USE STRUCTS WITHOUT POINTER!!
// only pointers will edit real data! otherwise you just recieve (and manipulate) a copy of a struct (when assigning s=structs[i])!
// reduce from 0x20 bytes to name,kind, firstStatement == 0x0b bytes!
// 1 nit == 4+1 words =20bytes
typedef struct Node {
    //class Node{
    //public:
    int id; //implicit -> redundant
    int context; //implicit   | short context_id or int node_id

#ifdef inlineName
    char name[100];
#else
    char* name; // see value for float etc
#endif
    int kind; // abstract,node,person,year, m^2
    //Node* kind;

    //float rank;
    int statementCount; //implicit, can be replaced with iterator KEEP!
    int firstStatement;
	int lastStatement;
    Value value; // for statements, numbers

//    int firstPredicateStatement; // 'usages' OR: sort statements!
//    Hope_Valley             kind            City

    // INDEX
    // class country{ population{property:0} capital{property:1} }
    // germany.index[0]=80Mio .index[1]=Berlin
    //        Node* index;//nur #properties+1 Nits!!
}Node ;



// norway captial oslo
// oslo population 325235
// all captials with more than 300000

// units
// beth age '29 years'
// oldest -> sort by old
typedef struct Ahash {
    Node* abstract;
    Ahash* next;
}Ahash ;

// S13425 beth likes apple
// Node3254 value=S13425
// S      karsten agrees <Node3254>
// S      Node3254/S13425 is wrong
// 1 Stit == 3 words
typedef struct Statement {
    int id; // implicit?
    int context; // implicit?  NODE!?!
    //        Node* meta;
    Node* Subject; // as function? nee as it is!! transpose on context load
    Node* Predicate;
    Node* Object;
    int nextSubjectStatement;
    int nextPredicateStatement;
    int nextObjectStatement;
    int subject; // implicit!! Subject
    int predicate;
    int object;
    // int subjectContext;//... na! nur in externer DB!
} Statement;
// manipulate via other statements (confidence, truth, author(!), ...)

// union: context->hasNode->0 , name
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
typedef vector<Facet*> FacetVector;
typedef vector<Statement*> StatementVector;

class Query {
public:

    Query() {
        depth = maxRecursions;
        start = 0;
        page = 0;
        hitsPerPage = 10; //rows=10;
        limit = 100; // was mit facetten???
        lookuplimit = 10000;// INF!
        maxFacets=10;
        autoFacet = true;
        semantic = false;
        predicatesemantic=false;
    }
    int start; //    int offset; == page* hitPerPage
    //    int end;//     == start + hitsPerPage
    int page; //    int offset;
    int hitsPerPage; // rows
    int limit;
    int lookuplimit;
    int depth; //=maxRecursions;
    bool semantic;
    bool predicatesemantic;
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
    int currentNameSlot;
    int nodeCount;
    int statementCount; //first 1000 reserved!
    // Node nodes[maxNodes];
    // Statement statements[maxStatements];
    Node* nodes;
    Statement* statements;
    //	int statementArrays[10000000];
    //	int* statementArrays;
    //	int lastStatementArray;
} Context;

//const ?
//static Context contexts[maxContexts]; save geht nicht
extern Context* contexts; //[maxContexts];
//Context contexts[maxContexts];


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
extern Node* BackLabel;
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


////////////////////////
void flush();
char* name(Node* node);
Context* getContext(int contextId);
void showContext(int nr);
void showContext(Context* cp);
void showNodes(NodeVector all, bool showStatements = false,bool showRelation=false); //
//string query2(string s,int limit=defaultLimit);
//string query(Query& q);
void initNode(Node* node, int id, const char* nodeName, int kind, int contextId);
Node* add(const char* nodeName, int kind = /*_node*/ 101, int contextId = current_context);
bool checkNode(Node* node, int nodeId = -1, bool checkStatements = false, bool checkNames = false);
bool addStatementToNode(Node* node, int statementNr);
bool addStatementToNodeDirect(Node* node, int statementNr);
bool addStatementToNodeWithInstanceGap(Node* node, int statementNr);
Statement* addStatement4(int contextId, int subjectId, int predicateId, int objectId, bool check = true);
Statement* addStatement(Node* subject, Node* predicate, Node* object, bool checkDuplicate = true);
//inline
		Node* get(const char* node);
inline Node* get(char* node);
inline Node* get(int NodeId);

//extern "C" /* <== don't mingle name! */ inline
Context* currentContext();

Node* getAbstract(const char* word);
int collectAbstracts();
unsigned int hash(const char *str); //unsigned
//unsigned long hash(const char *str); //unsigned
//Node* getThe(const char* word, Node* type = 0,bool dissect=true);
//Node* getThe(string thing, Node* type=0,bool dissect=true);
Node* getThe(const char* word, Node* type = 0,bool dissect=false);
Node* getThe(string thing, Node* type=0,bool dissect=false);
Node* getThe(Node* abstract, Node* type=0);
Node* getClass(const char* word);// ?  apple vs Apple ! same as getThe NOW
//Node* getClass(string word);
void showStatement(Statement* s);
void show(Node* n, bool showStatements = true);
Node* showNr(int context, int id);
void testBrandNewStuff();
Node* findWord(int context, const char* word, bool first = false);
//NodeVector find_all(char* name, int context = current_context, int recurse = false, int limit = defaultLimit);
Statement* findStatement(Node* subject, Node* predicate, Node* object, int recurse = false, bool semantic = false, bool symmetric = false,bool semanticPredicate=false);
Statement* findStatement(Node* n, string predicate, string object, int recurse = false, bool semantic = false, bool symmetric = false);
char* initContext(Context*);
Node* hasWord(const char* thingy);
string getImage(const char* n, int size = 300);

Node* has(const char* n, const char* m);
Node* has(Node* n, Node* m);
Node* has(Node* n, Statement* s, int recurse = true, bool semantic = true, bool symmetric = false,bool predicatesemantic=true);//naja!!!
Node* has(Node* subject, string predicate, string object, int recurse = true, bool semantic = true, bool symmetric = false);
Node* has(Node* subject, Node* predicate, Node* object, int recurse = true, bool semantic = true, bool symmetric = false,bool predicatesemantic=true);
void set(Node* node, Node* property, Node* value);
bool isA4(Node* n, string match, int recurse = false, bool semantic = false);
bool isA4(Node* n, Node* match, int recurse = false, bool semantic = false);
bool isA(Node* fro, Node* to);
Node* value(const char* name, double v,const char* unit = 0);
Node* parseValue(const char* aname);
void clearAlgorithmHash();

Statement* pattern(Node* subject, Node* predicate, Node* object);
Statement* isStatement(Node* n);// to / get Statement
Statement* nextStatement(Node* n,Statement* current,bool stopAtInstances=false);
Statement* getStatement(int id,int context_id=current_context);
Statement* getStatementNr(Node* n, int nr,bool firstInstanceGap=false);

//NodeVector& all_instances(Node* type, int recurse , int limit = defaultLimit);
//NodeVector& all_instances(Node* type);
//NodeVector all(Node* type,Node* slot,int recurse);
Node* findMember(Node* n, string match, int recurse = false, bool semantic = false);
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
Statement* learn(string data);
void deleteNode(Node* n);
void deleteStatements(Node* n);
void deleteStatement(Statement* s);
int countInstances(Node* n);
Node* isEqual(Node* subject, Node* object);
Node* isGreater(Node* subject, Node* object);
Node* isLess(Node* subject, Node* object);
Node* isAproxymately(Node* subject, Node* object);

void dissectWord(Node* subject);
Context* currentContext();
//void initRelations();
void init();
Node* add_force(int contextId, int id, const char* nodeName, int kind);

#define is Parent
#define is_a Parent
#define are Parent
#define does_have Member
#define has_a Member
#define have Member

//extern map<long, Node*>* abstracts;
extern Ahash* abstracts; // Node name hash
extern Ahash* extrahash; // for hashes that are not unique, increasing
extern map < Node*, bool> yetvisited;
extern map <double, short> yetvisitedIsA;
//Ahash* getAbstractHash(int hashkey);
//Node* getAbstractHash(const char* word);
//Ahash* insertAbstractHash(long hashkey, Node* abstract); //debug only
Ahash* insertAbstractHash(int hashkey, Node* abstract); //debug only
Ahash* insertAbstractHash(Node* abstract);

void deleteWord(string* s);
void deleteWord(const char* data,bool completely=true);
Node* reify(Statement* s);
bool checkStatement(Statement *s,bool checkSPOs=false,bool checkNamesOfSPOs=false);
//bool checkStatement(Statement *s);
void checkRootContext();
typedef Node* N;
typedef NodeVector NV;

//#pragma warnings_off
static int averageNameLength = 20; // used for malloc
static int nodeSize=sizeof(Node);
static int statementSize=sizeof(Statement);
static int ahashSize=16;
static int million=1000000;
static int billion=1000000000;
static int MB=1000000;
static int GB=1000000000;
//# sudo sysctl -w kern.sysv.shmmax=2147483648 # => 2GB !!
static int maxNodes = 9*million;
static int maxStatements0 = maxNodes*10;// 10 = crude average of Statements per Node  ; max=1000!
static int abstractHashSize = maxNodes*ahashSize; //~nodes?
static int contextOffset=0x4000;
static int abstractsOffset= contextOffset+ maxNodes*(nodeSize+averageNameLength)+maxStatements0*statementSize;// can groooow!
static int bytesPerNode=(nodeSize+averageNameLength+ahashSize*2);
static long sizeOfSharedMemory =contextOffset+ maxNodes*bytesPerNode+maxStatements0*statementSize;// 5000000000; //0x0f000000;// 0x0f000000;//1000*1000*400;// /* 400MB shared memory segment */
//static long sizeOfSharedMemory =8000000; //0x0f000000;// 0x0f000000;//1000*1000*400;// /* 400MB shared memory segment */

int test2();