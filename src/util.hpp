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
bool eq(const char* x,const char* y,bool ignoreCase = false);
bool eq(string x, const char* y);
bool eq(string* x, const char* y);
Node* eq(Node* x, Node* y);
bool startsWith(const char* x,const char* y);
bool startsWith(string* x,const char* y);
bool contains(const char* x,const char* y,bool ignoreCase=false);
//bool contains(const char* x,const char* y);
//bool contains(NodeVector& all,Node* node);
bool contains(NodeVector v, Node* o);
bool contains(vector<char*>& all,char* node);
bool contains(string x,const char* y);
bool isNumber(char* buf);
bool isInteger(char* buf);
void ps(char* s);
void ps(string s);// string geht nicht!?!
void ps(string* s);
void ps(NodeVector v);
void pl(long l);
void pi(int i);
void px(void* p);// 64 bit hex
extern void p(char* s);
void p(Query& q);
void p(Node* n);
//char* next(char* data);
string next(string data);
vector<char*>& splitString(string line0,const char* separator="\t");
vector<char*>& splitString(const char* line0,const char* separator="\t");
char** splitStringC(const char* line0, const char* separator);
//int splitStringC2(char* line,char** tokens,const char* separator="\t");// BROKEN!!!
int splitStringC(char* line,char** tokens, char separator,bool safe=false);
void fixNewline(char* line);
NodeVector intersect(NodeVector a,NodeVector b);
char* tolower(char* x);
unsigned int hash(const char *str);
inline int normChar(int c);
string stem(string word);
void addRange(NodeVector& some, NodeVector more);
NodeVector mergeVectors(NodeVector some, NodeVector more);
char* substr(char* what,int from,int to);
char* match(char* input, char* pattern);
char* modifyConstChar(char* line);
#define check(assertion) ps(#assertion);if(assertion)ps("OK");else{ps("FAILED");exit(0);}