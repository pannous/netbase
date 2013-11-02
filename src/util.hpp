#pragma once
#include <string>
#include "netbase.hpp"

using namespace std;

#define null 0
extern bool quiet;

string replace_all(string str, string what, string with);
extern void flush();


//#define check(assertion) assert(assertion,#assertion);
bool assert(bool test, char* what);
template<typename T> void empty(vector<T,std::allocator<T> >& v);

#define po p("ok");
string join(char** argv,int argc);
bool eq(const char* x,const char* y,bool ignoreCase = true,bool ignoreUnderscore=true);
bool eq(string x, const char* y);
bool eq(string* x, const char* y);
Node* eq(Node* x, Node* y);
bool startsWith(const char* x,const char* y);
bool startsWith(string* x,const char* y);
bool endsWith(const char* x, const char* y);
string cut_to(string str, string what);
bool contains(const char* x,const char* y,bool ignoreCase=true);
//bool contains(const char* x,const char* y);
//bool contains(NodeVector& all,Node* node);

//bool contains(NodeVector* v, Node* o,bool fuzzy=false);
//bool contains(NodeVector* v, Node* o);
//bool contains(NodeVector v, Node* o);
//bool contains(NodeVector& v, Node* o);
bool contains(vector<char*>& all,char* node);
bool contains(string x,const char* y);
bool isNumber(char* buf);
bool isInteger(char* buf);
#define pf(x,...) printf(x, __VA_ARGS__)
void ps(char* s);
void ps(string s);// string geht nicht!?!
void ps(string* s);
void ps(NodeVector v);
void pl(long l);
void p(int i);
void px(void* p);// 64 bit hex
extern void p(char* s);
void p(Query& q);
void p(Node* n);
void p(Statement* n);
void p(NodeVector& n);
void p(string s);
//char* next(char* data);
string next(string data);
vector<char*>& splitString(string line0,const char* separator="\t");
vector<char*>& splitString(const char* line0,const char* separator="\t");

int splitStringC(char* line,char** tokens, char separator);
char** splitStringC(char* line0, char separator,int maxRows=20);
char** splitStringC(const char* line0, char separator);
char** splitStringC(const char* line0, const char* separator);
//int splitStringC2(char* line,char** tokens,const char* separator="\t");// BROKEN!!!

char* downcase(char* x);
//char* tolower(char* x);//call of overloaded 'tolower(const char*&)' is ambiguous
char* tolower(const char* x);
void fixNewline(char* line);
NodeVector intersect(NodeVector a,NodeVector b);
unsigned int wordhash(const char *str);
inline short normChar(char c);
string stem(string word);
void addRange(NodeVector& some, NodeVector more,bool checkDuplicates=true);
//NodeVector mergeVectors(NodeVector some, NodeVector more);
void mergeVectors(NodeVector* some, NodeVector more);
char* substr(char* what,int from,int to);
char* match(char* input, char* pattern);
char* clone(const char* line);
char* modifyConstChar(const char* line);
const char* concat(const char* a,const  char* b);
char* fixQuotesAndTrim(char* tmp);
//inline
bool isAbstract(Node* object);
//bool contains(NodeVector& v, Node& o,bool fuzzy=false);
bool contains(NodeVector& v, Node* o,bool fuzzy=false);
#define check(assertion) pf("TEST %s\n",#assertion);if(assertion)pf("PASSED %s\n",#assertion);else{pf("FAILED %s\n",#assertion);exit(0);}
