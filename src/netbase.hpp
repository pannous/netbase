#pragma once

static char* /*increase-package-version.py src/netbase.hpp*/
version="1.6.8";

#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <set> // faster vector!
#include <queue>

using namespace std;

typedef const char * chars ;

extern "C" char* _version();
extern bool NO_TOPICS;
extern bool LIVE;
extern bool useSemantics;
extern bool useGermanLabels;
extern bool exitOnFailure;
extern bool debug; //=true;
extern bool trace;//=false
extern bool showAbstract;
extern bool autoDissectAbstracts;
extern int recursions;
extern int badCount;
extern int runs; // algorithm metrics
extern int current_context;
extern bool useYetvisitedIsA;
extern bool autoIds; // parse '1' as https://www.wikidata.org/wiki/Q1 (Universe)
extern bool testing; // don't implicitly init relations
extern bool importing; // performance state

extern int resultLimit;// 10
extern int lookupLimit;// 1000
extern int typeLimit;// 1000
extern int queryLimit;// 10000000
extern int defaultLookupLimit;
extern int maxRecursions;

extern bool out_of_memory;// delayed exit()
extern int wikidata_limit;// =60000000;
extern bool checkDuplicates;
// if test or called from other object

//#define inlineName true // because of char* loss!!!! TODO!!!

#define NO_TYPE 0

//#define name(w) getThe(#w)
#define word(w) getAbstract(#w)
#define some(word) getAbstract(#word)

//#define a(word) getThe(#word)
#define all(word) getAbstract(#word)
//#define all(word) getThe(#word) //todo!
//#define a(word) getAbstract(#word)
//#define _(w) getThe(#w)
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
//#define get(word) Node* word=getThe(#word)   ;




//static int sizeOfSharedMemory= 0x0f000000;//1000*1000*400;// /* 400MB shared memory segment */
static const bool multipleContexts = false;
extern string path;
extern string data_path;// difference to memory path: memory map constantly writes to files!
extern string import_path;
extern bool USE_MMAP;// 1 = memory map or shared memory = 0 via config

extern bool virgin_memory;

//static const int maxStatementsPerNode = 10000; // cant make dynamic if used as array[100]
static const int maxContexts = 1000;
extern map<int, int> wn_map;
extern map<int, int> wn_map2;

struct Statement;

struct Node;
//class Node;
typedef union Payload {
    // why not just save values+text in 'name' as string??
  int name;// pointer to context->names
  char* text; // wiki abstracts etc
  void* data; // byte[], same as ^^
  //  char* name;
  //  int integer;
  //  float floatValue;
  long datetime; // milliseconds? Date*? HOW really?
  //  long longValue;// 8 bytes

  double number; // 8 bytes OK
  // why not just save ints as string?
  // 1) faster comparison  bottleneck factor n==nr digits or is atoi part of cpu?
  // 2) memory. really?? only if value field is used >80% of the time!

	Node* node; // THE ONE in abstracts type --- cycle !---
  Statement* statement; // overhead OK!!! 8 bytes on 64bit machines (Statement too long, pointer OK)
}Payload;
//extern Context* context;
extern "C" const char* getName(int node);
char* resolveName(int pointer);

// NEVER USE STRUCTS WITHOUT POINTER!!
// only pointers will edit real data! otherwise you just recieve (and manipulate) a copy of a struct (when assigning s=structs[i])!
// reduce from 0x20 bytes to name,kind, firstStatement == 0x0b bytes!
// 1 nit == 4+1 words =20bytes
int getNodeId(long pointer);
extern "C"
typedef struct Node {
//  class Node{
  public:
    int id; //implicit -> redundant
//    int id(){ return getNodeId((long)this);}// implicit

	char* name;// best to debug!
//	char* english;
//  long name; // see value for float etc
//    int namePointer;// harder to debug, just adjust if context->nodeNames != context->nodeNames
//    char* name(){return context->nodeNames[namePointer]}     char* resolveName(int pointer);

    int kind; // abstract,node,person,year, m^2   // via first slot? nah
  //int context; //implicit  | short context_id or int node_id
  //float rank;
  int statementCount; //explicit, can be made implicit and replaced with iterator
  int firstStatement;
  int lastStatement;// remove
  Payload value; // for statements, numbers WASTE!!! remove / merge with name!
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
//  Node* abstract;
//  Ahash* next;
}Ahash ;

// safe:

//typedef struct Context;
//Context* getContext(int contextId);



// S13425 beth likes apple
// Node3254 value=S13425
// S   karsten agrees <Node3254>
// S   Node3254/S13425 is wrong
// 1 Stit == 3 words

extern "C" Node* get(int NodeId);
extern "C" int getStatementId(long pointer);

typedef class Statement {
//class Statement { //OK!!
//typedef struct Statement {
public:
//  int id; // implicit?
  int id(){ // implicit
  		return getStatementId((long)this);
  }
//#ifdef useContext
  int context; //  needed for pattern() -> find...   todo: via :NODE!?!
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
//  REORDER NEEDS NEW INDEX ON ALL SERVERS +JAVA!
  int subject; // implicit if using explicitNodes: Subject->id
  int predicate;
  int object;

  int nextSubjectStatement;
  int nextPredicateStatement;
  int nextObjectStatement;

  // Node* meta; for reification, too expensive, how else now??
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
    //    In this case, the pointer variable must be explicitly deallocated withthe delete operator.
    views = 0;
    hits = 0;
    maxHits = 0;
    rank = 0;
  }

  ~Facet() {
    printf("~Facet\n");
    //    when an object variable goes out of scope, its destructor is called automatically.
  }
  Node* field;
  map<Node*, int> *values; //count
  int views;
  int hits;
  int maxHits;
  float rank;
  Node* type; //node,string,date,int,geo,float
  //  Pair[] ranges;// how?
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
  int start; //  int offset; == page* hitPerPage
  //  int end;//   == start + hitsPerPage
  int page; //  int offset;
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
  //  NodeList classes;// filled by algorithm
  //  NodeList instances;// car1 car2 car3
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
extern Node* Type; // object->class  class->class see SuperClass
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

//Node* Attribute;   // ld: duplicate symbol
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

Context* getContext(int contextId=0/*wordnet*/,bool init=true);
void showContext(int nr);
void showContext(Context* cp);
void showNodes(NodeSet all, bool showStatements = false,bool showRelation=false,bool showAbstracts=false);
NodeVector showNodes(NodeVector all, bool showStatements = false,bool showRelation=false,bool showAbstracts=false); // bool justTitles==showStatements
NodeVector show(NodeVector all);// alias
NodeVector showWrap(Node* n);
NodeVector wrap(Node* n);
//string query2(string s,int limit=defaultLimit);
//string query(Query& q);
Node* initNode(Node* node, int id, const char* nodeName, int kind, int contextId);
extern "C" Node* add(const char* nodeName, int kind = /*_node*/ -101, int contextId = current_context);
//bool checkNode(int nodeId, bool checkStatements= false, bool checkNames = false);
//bool checkNode(Node* node, int nodeId = -1, bool checkStatements = false, bool checkNames = false);
bool checkNode(int nodeId, bool checkStatements= false, bool checkNames = false,bool report=true);
bool checkNode(Node* node, int nodeId = -1, bool checkStatements = false, bool checkNames = false,bool report=false);
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
Node* getAbstract(Node *node);

int collectAbstracts(bool clear=false);
int collectInstances();
unsigned int hash(const char *str); //unsigned
//unsigned long hash(const char *str); //unsigned
//Node* getThe(const char* word, Node* type = 0,bool dissect=true);
//Node* getThe(string thing, Node* type=0,bool dissect=true);
Node* getThe(Node* abstract, Node* type=0, bool create= true);
extern "C" Node* getThe(const char* word, Node* type = 0);//,bool dissect=false);
Node* getThe(string thing, Node* type=0);//,bool dissect=false);
extern "C" Node* getNew(const char* thing, Node* type=0);//, bool dissect=false);
Node* getSingleton(const char* thing, Node* type=0,bool addType=true) ;
//Node* getClass(string word);
//extern "C"
void showStatement(Statement* s);
extern "C" void showStatement(int id);
bool show(Node* n, bool showStatements = true);
bool showShort(Node* n);
void show(Statement * s);
//extern "C"
Node * showNode(Node* n, bool showStatements=true);
extern "C" Node* showNode(int id);
void testCurrent();

char* initContext(Context*);
Node* hasWord(const char* thingy,bool seo=false);
extern "C" bool hasNode(const char* thingy);
string getImage(const char* n, int size = 150,bool thumb=false);
string getImage(Node* a, int size=150,bool thumb=false);


Node* value(const char* name, double v,const char* unit);
Node * value(const char* aname, double v, Node* unit = 0);
Node *numberNode(double v);// name:"#"

extern "C" void saveData(int node,void* data,int size,bool copy);
extern "C" void* getData(int node);
extern "C" Payload getValue(int node);
extern "C" int valueId(const char* aname, double v, int unit = 0);
//Node * value(const char* aname, Value v, Node* unit = 0);
Node* parseValue(const char* aname);


extern "C" void addLabel(Node* n, const char* label);
void addLabel(const char* n, const char* label);
extern "C" void setLabel(Node* n, const char* label,bool addInstance=true,bool renameInstances=false);
extern "C" void setName(int node, const char* label);
Statement* pattern(Node* subject, Node* predicate, Node* object);
Statement* isStatement(Node* n);// to / get Statement


extern "C"
Statement * nextStatement(int node,Statement* current);
Statement* nextStatement(Node* n,Statement* current,bool stopAtInstances=false);
extern "C" Statement* getStatement(int id,int context_id=current_context);
Statement* getStatementNr(Node* n, int nr,bool firstInstanceGap=false);
void newQuery();// reset limits

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
void forget(N from, N pred, N to=0);
void forget(chars from, chars pred, chars to);
void deleteNode(Node *n, bool deleteChildren);
void deleteStatements(Node* n);
extern "C" void deleteStatement(int id);
void deleteStatement(Statement* s);
int countInstances(Node* n);
bool isEqual(Node* subject, Node* object);
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
Node *getPartner(Statement* s, Node* n, Node *m);

#define is Parent
#define is_a Parent
#define are Parent
#define does_have Member
#define has_a Member
#define have Member

//extern map<long, Node*>* abstracts;
extern Ahash* abstracts; // Node name hash
//extern Ahash* extrahash=abstracts+maxNodes; // for hashes that are not unique, increasing!!!
extern map < Node*, bool> yetvisited;
extern map <double, short> yetvisitedIsA;// very memory expensive??
//Ahash* getAbstractHash(int hashkey);
//Node* getAbstractHash(const char* word);
//Ahash* insertAbstractHash(long hashkey, Node* abstract); //debug only
Ahash* insertAbstractHash(unsigned int hashkey, Node* abstract,bool overwrite=false,bool seo=false); //debug only
Ahash* insertAbstractHash(Node* abstract,bool overwrite=false);

//void deleteWord(string* s);
void deleteWord(const char* data,bool completely=true);// hard to find instances after!
Node* reify(Statement* s);
void show(vector<char*>& v);
bool checkStatement(Statement *s,bool checkSPOs=false,bool checkNamesOfSPOs=false);
//bool checkStatement(Statement *s);
void checkRootContext();
Node* number(int n);
void dissectParent(Node* subject,bool checkDuplicates=false);
Node *dissectWord(char* word);
Node* dissectWord(Node* subject,bool checkDuplicates=false);
Node* mergeNode(Node* target,Node* node);
Node* mergeAll(const char* target);
extern bool replaying;
void replay(const char *file);
//extern "C" C-linkage specified, but returns user-defined type 'NodeVector' (aka 'vector<Node *>') which is incompatible with C
NodeVector parse(const char *data, bool safeMode, bool console);// and act -> extern "C" execute
void fixCurrent();
extern "C" void setKind(int id,int kind);
extern "C" Node* save(Node* n);
extern "C" Node* getType(Node* n);// != kind!
extern "C" char* getText(Node* n);
static char* NO_TEXT=(char*)"";
#define _MAIN_
int main(int argc, char *argv[]);
extern "C" int test2();// extern "C" -> don't mangle!
extern Context* contexts; //[maxContexts];
extern Statement* statement_root;
extern Context* context_root; // else: void value not ignored as it ought to be
extern Node* node_root;
extern char* name_root;
//extern char* english_words;// nonsense, just load whole data/ dir with correct abstracts and nodes...

extern int* freebaseKey_root;// keyhash-> NodeId 'map'

Node* bad(char* message=0);//string="");
void showReasons();

void show(NodeSet& all);
void addSeo(Node* n);
Node *getSeo(const char* thingy);
void buildSeoIndex();

static int MB=1048576;
static long GB=1024*MB;
static int million=MB;
static long billion=GB;

static int propertySlots=0x100000;//1000000;// PROPERTY RELATION SLOTS >-1000 internal, <-10000 wikidata <-200000-317658 wordnet!
static int propertyOffset=10000;// PROPERTY RELATION SLOTS >-1000 internal, <-10000 wikidata <-20000 wordnet abstracts
static int wordnetOffset=20000;//
static int synsetOffset=100000;// 200000-317658 for wordnet + other !

//# sudo sysctl -w kern.sysv.shmmax=2147483648 # => 2GB !!
/*
 * Current nodes:261609080         statements:524157200            chars:4149597486
Maximum nodes:419430400         statements:838860800            chars:8388608000
Usage  nodes:62.37%                     statements:62.48%               chars:49.47%
 */

static long contextOffset=0x800000;//0x10000;
static int averageNameLength = 16;// 10 for amazon! else 20 (cheap)
static int bytesPerNode=(sizeof(Node) + averageNameLength);//+ahashSize*2

#ifdef HARD_WIRED_MEMSIZE

#ifdef WASM
static long maxNodes = 1*million;
static long maxStatements = 1*maxNodes;
#elif ECHSE
long maxNodes = 209715200;//220*million;// enough
long maxStatements = 471859200;//2*maxNodes;
#else
//static long maxNodes = 1*million;
//static long maxStatements = 1*maxNodes;
static long maxNodes = 300*million;// Live 11.11.2018
static long maxStatements = 2*maxNodes;// why suddenly 629145600
#endif

long maxChars=maxNodes * averageNameLength;
static long sizeOfSharedMemory =contextOffset+ maxNodes*bytesPerNode+maxStatements*statementSize;
static long stupidCompiler=billion+ahashSize+sizeOfSharedMemory;//abstractHashSize // against unused ahashSize ...

#else

extern long long  maxChars;
extern long maxNodes;
extern long maxStatements;
extern long sizeOfSharedMemory;

#endif

