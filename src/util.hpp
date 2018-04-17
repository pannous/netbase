#pragma once
#include <string>
#include "netbase.hpp"

//extern int MAX_CHARS_PER_LINE;
//#define MAX_CHARS_PER_LINE 0x100000 // amazon wtf!
#define MAX_CHARS_PER_LINE 0x10000 // debug only! against gdb more than max-value-size
#define MAX_ROWS 100
using namespace std;

#define null 0
extern bool quiet;

string replace_all(char* str, string what, string with, bool replace_inline= true);
string replace_all(string str, string what, string with);
extern void flush();


//#define check(assertion) assert(assertion,#assertion);
bool assert(bool test, const char* what);
template<typename T> void empty(vector<T,std::allocator<T> >& v);
#define cchar const char
#define po p("ok");
bool eq(const char* x,const char* y,bool ignoreCase = true,bool ignoreUnderscore=true);
bool eq(string x, const char* y);
bool eq(string* x, const char* y);
bool eq(int a,int b);
Node* eq(Node* x, Node* y);
bool startsWith(const char* x,const char* y);
bool startsWith(string* x,const char* y);
bool endsWith(const char* x, const char* y);
//string cut_to(string str, string what);
cchar* cut_to(cchar* str,cchar* match);
char* cut_to(char* str,cchar* match);
char* keep_to(char* str, cchar* match);
bool contains(const char* x, const char y);
bool contains(const char* x,const char* y,bool ignoreCase=false);
//bool contains(const char* x,const char* y);
//bool contains(NodeVector& all,Node* node);

//bool contains(NodeVector* v, Node* o,bool fuzzy=false);
//bool contains(NodeVector* v, Node* o);
//bool contains(NodeVector v, Node* o);
//bool contains(NodeVector& v, Node* o);
bool contains(NodeSet& all, Node* node);
bool contains(NodeSet* all, Node* node);
bool contains(Node* list, char* object, bool fuzzy);
bool contains(vector<char*>& all,char* node);
bool contains(vector<const char*>& all, const char* node,bool ignoreCase=false);
bool contains(vector<string>& all, char* name);
bool contains(string x,const char* y);
bool isNumber(const char* buf);
bool isInteger(const char* buf);
#define pf(x,...) if(!quiet){printf(x, __VA_ARGS__);}
void ps(const char* s);
void ps(string s);// string geht nicht!?!
void ps(string* s);
void ps(NodeVector v);
void p(long l);
void pl(long l);
void pd(double l);
//void p(int i);
void pi(int i);
void px(void* p);// 64 bit hex
void p(char* s);
void p(const char* s);
void p(Query& q);
void p(Node* n);
void p(Statement* n);
void p(NodeVector& n);
void p(string s);
//char* next(char* data);
string next_word(string data);
char* next_word(char* data);

//vector<char*>& splitString(string line0,const char* separator="\t");
//vector<char*>& splitString(const char* line0,const char* separator="\t");
vector<string>& splitString(const char* line0,const char* separator="\t");
vector<string>& splitString(string line0,const char* separator="\t");
int splitStringC(char* line,char** tokens, char separator);
char** splitStringC(char* line0, char separator,int maxRows=20);
char** splitStringC(const char* line0, char separator);
char** splitStringC(const char* line0, const char* separator);
//int splitStringC2(char* line,char** tokens,const char* separator="\t");// BROKEN!!!

char* downcase(char* x);
//char* tolower(char* x);//call of overloaded 'tolower(const char*&)' is ambiguous
char* tolower(const char* x);
void fixNewline(char* line,bool limit=0/*0=none*/);
NodeVector intersect(NodeVector a,NodeVector b);
unsigned int wordhash(const char *str);
//inline short normChar(char c);
short normChar(char c);
string stem(string word);
//void addRange(NodeVector& some, NodeVector more,bool checkDuplicates=true);
//NodeVector mergeVectors(NodeVector some, NodeVector more);
void mergeVectors(NodeVector* some, NodeVector more);
void mergeVectors(NodeSet* some, NodeVector more);
char* substr(const char* what,int from,int to);
char* match(char* input, cchar* pattern);
char* clone(const char* line);
char* editable(const char* line);
char* modifyConstChar(const char* line);
const char* concat(const char* a,const char* b);
//const char* concat(char* a,char* b);
char* fixQuotesAndTrim(char* tmp);
char* replaceChar(char* thing,char what,char with);
char* replace(char* data,char what,char with);
//inline
bool isAbstract(Node* object);
//bool contains(NodeVector& v, Node& o,bool fuzzy=false);
bool contains(NodeVector& v, Node* o,bool fuzzy=false);
bool contains(vector<char*>& all, const char* node);

string itoa(int i);
void appendFile(const char* fileName,const char* data);

#define check(assertion) pf("TEST %s\n",#assertion);\
	if(assertion)printf("PASSED %s\n",#assertion);\
	else{printf("FAILED %s\n",#assertion);printf("%s:%d\n",__FILE__,__LINE__);exit(0);}

#define assertEquals(a,b) printf("TEST %s==%s\n",#a,#b);\
	if(eq(a,b))p("PASSED\n");else{p(a);p("!=");p(b);printf("%s:%d\n",__FILE__,__LINE__);exit(0);}

#define minimum(a,b) (a<b?a:b)
void printlabels();

FILE *open_file(const char* file,bool exitOnFailure=true);
//bool readFile(char* file,char* line);
bool readFile(const char* file,char* line);
void closeFile(const char* file);

char * fixLabel(char* label);
char* dropUrl(char* name);
void sortNodes(NodeVector& all);
bool empty(char* c);
bool empty(cchar* c);
int len(char* c);
int len(cchar* c);
int wordCount(char* c);
string generateSEOUrl(string input);
void error(string err);
