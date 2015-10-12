
#include <string>
#include <vector>
using namespace std;

// malloc, exit:
#include <cstdlib>
// strcmp
#include <string.h>
#include <regex.h>

#include "util.hpp"

bool assert(bool test, cchar* what) { // bool nix gut
	printf("----\nTEST ");
	printf("%s",what);
	if (test) {
		printf(" OK\n");
		flush();
	}
	if (!test) {
		printf(" Failed\n");
		printf(" runs : ");
		p(runs);
		flush();
		if (exitOnFailure != false) exit(1);
	}
	recursions=0; // reset!
	return test;
}


bool isInteger(const char* buf) {
	return strlen(buf) == strspn(buf, "0123456789");
}

bool isNumber(const char* buf) {
	return strlen(buf) == strspn(buf, "0123456789,.");
}

string next_word(string data) {
	int spc=(int)data.find(" ");
	if (spc >= 0) return data.substr(spc + 1);
	return data;
}

char* substr(const char* what, int from, int to) {
	if (to == -1) to=(int)strlen(what);
	if (from > to) return (char*)what; // const -> ERROR!! + danger!
	char *result=(char*) malloc(sizeof(char) * (to - from + 1));
	strncpy(result, what + from, to - from);
	result[to - from]=0; // In case of unclean malloc
	return result;
}

// todo : FREEEEEE!!!!
char* tolower(const char* x) { //const
	int len=(int)strlen(x);
	char* neu=(char*) malloc(sizeof(char) * len+1);
	for (int i=0; i < len; i++) {
		neu[i]=tolower(x[i]);
	}
	neu[len]=0;
	return neu;
}
char* downcase(char* x) {
	int i=0;
	for (; i < strlen(x); i++)
		x[i]=tolower(x[i]);
	return x;
}

template<typename T>
void empty(vector<T, std::allocator<T> > &v) {
	v.erase(v.begin(), v.end());
}

template void empty(vector<Statement*, std::allocator<Statement*> > &v);
//bool contains(vector<Node*>& all,Node* node){
//    for(int i=0;i<all.size();i++){
//      if((Node*)all[i]==node)return true;
//    }
//    return false;
//}
bool contains(const char* x, const char y){
	for (int i=0; i<strlen(x); i++) {
		if(x[i]==y)return true;
	}
	return false;
}
bool contains(const char* x, const char* y, bool ignoreCase) {
	// Sonderfall: contains("abc","")==false
	if (!x || !y) return false;
	if (x[0] == 0 || y[0] == 0) return false;
	if (strstr(x, y) >= x) return true;
	if (ignoreCase){
		char* mx=tolower(x);
		char* my=tolower(y);
		bool does_contain=strstr(mx, my)>=mx;
		free(mx);
		free(my);
		return does_contain;
	}
	return false;
}

bool contains2(NodeVector& all, Node* node) {
	auto x=std::find(all.begin(), all.end(), node);
	return x!= all.end();
}
bool contains(NodeSet& all, Node* node) {
	auto x=std::find(all.begin(), all.end(), node);
	return x!= all.end();
}
bool contains(NodeSet* all, Node* node) {
	auto x=std::find(all->begin(), all->end(), node);
	return x!= all->end();
}
bool contains(NodeVector& all, Node& node, bool fuzzy) {
//	if(!fuzzy)
		return contains2(all,&node);
//	for (int i=0; i < all.size(); i++) {
//		if ((Node*) all[i] == &node) return true;
//		if (fuzzy && eq(all[i], &node)) return true;
//	}
//	return false;
}
//
bool contains(NodeVector& all, Node* node, bool fuzzy) {
	return contains2(all,node);
//	for (int i=0; i < all.size(); i++) {
//		Node* n=(Node*) all[i];
//		if (n == node || (fuzzy && eq(n, node)))
//			return true;
//	}
//	return false;
}
//
//bool contains(NodeVector* v, Node* node,bool fuzzy) {
//    for (int i = 0; i < v->size(); i++)
//        if (v->at(i) == node)return true;
//		if(fuzzy && eq(v->at(i), node))return true;
//    return false;
//}
//bool contains(NodeVector* v, Node* o) {
//    for (int i = 0; i < v->size(); i++)
//        if (v->at(i) == o)return true;
//    return false;
//}
//bool contains(NodeVector v, Node* o) {
//    for (int i = 0; i < v.size(); i++) {
//        Node* n = (Node*) v[i];
//        if (n == o)return true;
//    }
//    return false;
//}
//bool contains(NodeVector& v, Node* o) {
//    for (int i = 0; i < v.size(); i++) {
//        Node* n = (Node*) v[i];
//        if (n == o)return true;
//    }
//    return false;
//}
//

bool contains(vector<char*>& all, char* node) {
	for (int i=0; i < all.size(); i++) {
		char* ali=all[i];
		if (eq(ali, node)) return true;
	}
	return false;
}

bool contains(string x, const char* y) {
	return x.find(string(y)) != string::npos;
}

string replace_all(string str, string what, string with) {
	int idx=0;
	while ((idx=(int)str.find(what, idx)) >= 0) {
		str.replace(idx, what.length(), with);
		idx=idx + (int)with.length() + 1;
	}
	return str;
}

cchar* cut_to(cchar* str, cchar* match){
    cchar* i=strstr(str,match);
    if(!i)return str;
    return i+strlen(match);
}

char* cut_to(char* str, cchar* match){
    char* i=strstr(str,match);
    if(!i)return str;
    if(i)i[0]=0;
    return i+strlen(match);
}

//#include <sstrings2.h> Linking with -lsstrings2
//char* strrstr(const char* haystack, const char* needle);
char* reverse_cut_to(char* str, char match){
	for (size_t i=strlen(str); i>0; --i) {
		if(str[i]==match)return str+i+1;//str[i]=0;
	}
	return str;
}

// NOT const !!
char* keep_to(char* str, cchar* match){
    char* i=(char*)strstr(str,match);
    if(i)i[0]=0;
    return str;
}



//string cut_to(string str, string what) {
//	int idx=(int)str.find(what);
//	return str.substr(idx + what.length());
//}

bool quiet=false;

//void flush(){
//   fflush(stdout);
//}

bool endsWith(const char* x, const char* y) {
	int xlen=(int)strlen(x);
	int ylen=(int)strlen(y);
	if (xlen < ylen) return false;
//	char* i=(char*)strstr(x,y);
//	if(!i)return false;
	for (int i=1; i <= ylen; i++)
		if (x[xlen - i] != y[ylen - i]) return false;
	return true;
}
bool startsWith(const char* x, const char* y) {
	short len=strlen(y);
	if (strlen(x) < len) return false;
	for (int i=0; i < len; i++) {
		if (x[i] != y[i]) return false;
	}
	return true;
}

bool startsWith(string* x, const char* y) {
	return (int)x->find(y) >= 0;
}


const char* concat(const char* a,const  char* b){
	int la=(int)strlen(a);
	int lb=(int)strlen(b);
//	char c[la+lb];
    char* c=(char*)malloc((la+lb)*sizeof(char)+1);
	strcpy(c,a);
	strcpy(&c[la],b);
    c[la+lb]=0;
	return c;
}
bool equalsFuzzy(const char* x, const char* y){
    int xl=(int)strlen(x);
	if (xl != strlen(y)) return false; // xl!=strlen(y) != !xl==strlen(y) !!!!!
	int i=0;
	for (; i < xl; i++) {
		if (y[i] == 0) return false;
		if (normChar(x[i]) == normChar(y[i])) continue;
		if (tolower(x[i]) != tolower(y[i])) return false;
	}
	//    for(int i=0;i<strlen(y);i++)y0[i]=tolower(y[i]);
	//    return(strcmp(x0,y0)==0);// free?
	return true;
}

bool eq(const char* x, const char* y, bool ignoreCase, bool ignoreUnderscore) { //
	if (!x && !y) return true; //danger better: undefined?
	if (!x || !y) return false; //danger better: undefined?
    if (x && x[0] == 0 && y == 0) return true; // 0=='' danger!
	if (y && y[0] == 0 && x == 0) return true; // 0=='' danger!
	if (x == 0 || !y) return false;
	if (strcmp(x, y) == 0) return true;
	if (!ignoreCase) return false;
    if (strcasecmp(x, y) == 0) return true;//ignoreCase
    if(!ignoreUnderscore)return false;
    return equalsFuzzy(x, y);
}

//const char* concat(const char* a,const  char* b) {
//	string c="";
//	c=a+c+b;
//	return c.data();// LOST POINTER!
//}

string join(char** argv, int argc) {
	string a;
	int i=1;
	for (; i < argc; i++) {
		a=a + argv[i] + " ";
	}
	return a;
}

bool eq(int a,int b){
	return a==b;// for assertEquals
}

Node* eq(Node* x, Node* y) {
	return isEqual(x, y);
}

bool eq(string x, const char* y) {
	return (strcmp(x.c_str(), y) == 0);
}

bool eq(string* x, const char* y) {
	return (strcmp(x->c_str(), y) == 0);
}

void ps(string s) {	// string geht nicht!?!
	if (quiet) return;
	printf("%s\n", s.c_str());
	fflush(stdout);
}

void ps(NodeVector v) {
	if (quiet) return;
	NodeVector::iterator it;
	for(it = v.begin(); it != v.end(); ++it)
		show(*it);
}
void ps(string* s) {
	if (quiet) return;
	printf("%s\n", s->c_str());
	fflush(stdout);
}

void ps(const char* s) {
	if (quiet) return;
	printf("%s\n", s);
	fflush(stdout);
}

//void p(int i) {
//	if (quiet) return;
//	printf("%i\n", i);
//	fflush(stdout);
//}
void pi(int i) {
	if (quiet) return;
	printf("%i\n", i);
	fflush(stdout);
}

void pd(double l) {
	if (quiet) return;
	printf("%f\n", l);
	fflush(stdout);
}

void p(long l) {
	if (quiet) return;
	printf("%lu\n", l);
	fflush(stdout);
}

// 64 bit hex value

void px(void* p) {
	if (quiet) return;
	printf("%p\n", p);
	fflush(stdout);
}

void p(char* s) {
	if (quiet) return;
	printf("%s\n", s);
	fflush(stdout);
}

void p(const char* s) {
	if (quiet) return;
	printf("%s\n", s);
	fflush(stdout);
}

void p(string s) {
	if (quiet) return;
	printf("%s\n", s.data());
	fflush(stdout);
}

void p(Node* n) {
	show(n);
}

void p(NodeVector& n) {
	showNodes(n);
}

void p(Statement* n) {
	showStatement(n);
	flush();
}

void p(Query& q) {
	p("Query:");
	show(q.keyword, false);
	ps(q.keywords);
}

// todo: presorted jumplists
NodeVector intersect(NodeVector a, NodeVector b) {
	NodeVector c;
	NodeVector::iterator it;
	for(it = a.begin(); it != a.end(); ++it)
		if (contains(b, *it))
//			c.insert(*it);
			c.push_back(*it);
//	for (int i=0; i < a.size(); i++) {
//		Node* n=a[0];
//		if (contains(b, n)) c.push_back(n);
//	}
    return c;
}

// AAAHH NEVER WORKS!!! use splitStringC !!!
vector<char*>& splitString(string line0, const char* separator) {
	return splitString(line0.data(), separator);
}

// AAAHH NEVER WORKS!!! use splitStringC !!!
vector<char*>& splitString(const char* line0, const char* separator) {
	char * token;
	vector<char*>& v=*new vector<char*>;
	if (line0 == 0) {
		ps("empty splitString!");
		return v;
	}
	char* line=(char*) malloc(strlen(line0) * 2 + 1); //dont free!
	strcpy(line, line0);
	token=strtok(line, separator);
	while (token != NULL) {
		v.push_back(token);
		token=strtok(NULL, separator);
	}
	//free(line);// ja? NEIN: incorrect checksum for freed object - object was probably modified after being freed.
	return v;
}

// never works! use splitStringC
char** splitStringC1(const char* line0, const char* separator, int maxRows) {
	if (line0 == 0) {
		ps("empty splitString!");
		return 0;
	}
	char * token;
	char** tokens=(char**) malloc(maxRows);
	char* line=(char*) malloc(strlen(line0) * 2 + 1); //dont free!
	strcpy(line, line0);
	token=strtok(line, separator);
	int row=0;
	while (token != NULL) {
		token=strtok(NULL, separator);
		tokens[row++]=token;
	}
	//free(line);// ja? NEIN: incorrect checksum for freed object - object was probably modified after being freed.
	return tokens;
}

char* clone(const char* line) {
	char* line0=(char*) malloc(strlen(line) * 2 + 1); //dont free!
	strcpy(line0, line);
	return line0;
}

char* modifyConstChar(const char* line){
	return editable(line);// FREE if not kept!
}
char* editable(const char* line) {
	char* line0=(char*) malloc(strlen(line) * 2 + 1); //dont free!
	strcpy(line0, line);
	return line0;// FREE if not kept!
}

// line MUST not be const! tokens->out
int splitStringC(char* line, char** tokens, char separator) {
//	if(!contains(line, separator)){
//		tokens[0]=line;
//		tokens[1]=0;
//		return 1;
//////		return -1;
//	}
	char * token;
	int row=0;
	int len=(int)strlen(line);
	int i=0;
	char* lastgood=line;
	while (i < len) {
		char c=line[i];
		if (c == separator) {
			line[i]=0;
			token=lastgood;
			lastgood=&line[i + 1];
			if (tokens) tokens[row]=token;
			//        token = strtok (NULL, separator);

			row++;
		}
		i++;
	}
	if (tokens) {
		tokens[row]=lastgood;
		tokens[row+1]=0;// clean, be sure
	}
	return row + 1; //s
}
char** splitStringC(const char* line0, const char* separator) {
	return splitStringC(editable(line0), separator[0]);
}

char** splitStringC(const char* line0, char separator) {
	return splitStringC(editable(line0), separator);
}

char** splitStringC(char* line0, char separator, int maxRows) {
	char** tokens=(char**) malloc(maxRows);
//	memset(tokens,0,maxRows);
	splitStringC(line0, tokens, separator);
	return tokens;
}
char* replaceChar(char* thing,char what,char with){
    for(int i=0;i<strlen(thing);i++)if(thing[i]==what)thing[i]=with;
    return thing;
}
inline short normChar(char c) {// 0..36 damn ;)
	if (c >= '0' && c <= '9') return c-'0'+26;
	if (c >= 'a' && c <= 'z') return c-'a'+1;// NOT 0!!!
	if (c >= 'A' && c <= 'Z') return c-'A'+1;
    return 0;// no chinese etc!
    switch (c) {
        case '"':return 0;
            break;
        case '\'':return 0;
            break;
        case '(':return 0;
            break;
        case '_':return 0;
            break;
        case ' ':return 0;
            break;
        case '-':return 0;
            break;
        default: return c;// for asian etc!
    }
}

//unsigned int hashMod=(int)abstractHashSize / ahashSize;

// ./clear-shared-memory.sh After changing anything here!!
unsigned int wordhash(const char *str) { // unsigned
	if (!str) return 0;
	unsigned int c,hash=5381; // long
	while ((c=*str++)) {
		int next=normChar(c);//a_b-c==AbC
		if (next <= 0)continue;
		hash=hash*33 + next;// ((hash << 5) + hash
		hash=hash % maxNodes;
	}
	return hash;
}


// call by object => destination unmodified!   (how) expensive?!
//void addRange(NodeVector& some, NodeVector more, bool checkDuplicates) { // bool keep destination unmodified=TRUE
//	some.insert(more.begin(),more.end());
////	for (int i=0; i < more.size(); i++) {
////		Node*n=(Node*) more[i];
////		if (!checkNode(n)) continue;
////		if (!checkDuplicates || !contains(some, n)) some.push_back(n);
////	}
//}

//NodeVector mergeVectors(NodeVector some, NodeVector more) {// bool keep destination unmodified=TRUE
//    for (int i = 0; i < more.size(); i++) {
//        if (!contains(some, (Node*) more[i],false))
//            some.push_back(more[i]);
//    }
//    return some;
//}

//#include <algorithm>

void mergeVectors(NodeSet* some, NodeSet more) {
	some->insert(more.begin(), more.end());
}
void mergeVectors(NodeSet* some, NodeVector more) {
	some->insert(more.begin(), more.end());
}
void mergeVectors(NodeVector* some, NodeVector more) { // bool keep destination unmodified=TRUE
//	return std::set_union(some,&more);
	for (int i=0; i < more.size(); i++) {
//		/		if (!contains(*some, (Node*) more[i], false))
		if (std::find(some->begin(), some->end(), more[i]) == some->end())// NOT contains x yet
            some->push_back(more[i]);
	}
}

int charCount(cchar*__, char c) {// lol
	int ___=0;
	while (*__)
		___=c == *__++ ? ___ + 1 : ___;
	return ___;
} //lol

// for non-greedy use something like <([^>]+)>
// or this: //#define MINMOD	29	/* no	Next operator is not greedy. */
// USE [a-z] instead of \\w !!!
char* match(char* input,cchar* pattern) {
	int groups=charCount(pattern, '(');
	regmatch_t matches[groups + 1]; // matches[0] == whole pattern
	regex_t regex;
	int error=regcomp/*ile*/(&regex, pattern, REG_EXTENDED);
	if (error) return 0; //perror("regcomp failed");//regerror(status,REG_NOSUB)
	error=regexec(&regex, input, groups + 1, &matches[0], 0);
	if (error) return 0; //perror("regexec failed");
	int from=(int)matches[groups].rm_so;
	int to=(int)matches[groups].rm_eo;
	if (from > to) return 0;
	char *group=substr(input, from, to);
	regfree(&regex);
	return group;
}

// std::remove(arg.begin(), arg.end(), ' ');
char* fixQuotesAndTrim(char* tmp) {
	bool quote=false;
	while (tmp[0] == ' ' || tmp[0] == '_' || tmp[0] == '"') {
		if (tmp[0] == '"') quote=true;
		tmp=tmp + 1;
	}
	int len=(int)strlen(tmp);
	for (int i=0; i < len; i++) {
		char c=tmp[i];
		if (c == '+' || c == 0x2b) tmp[i]=' ';
		if (quote && c == ' ') tmp[i]='_';
		if (quote && c == '"') {
			quote=false;
			tmp[i]=' ';
		}
	}
	len--;
	while (len >= 0 && (tmp[len] == ' ' || tmp[len] == '_' || tmp[len] == '"'))
		tmp[len--]=0;
	return tmp;
}

string itoa(int i) {
	std::stringstream ss;
	ss << i;
	return ss.str();
}


char* replace(char* data,char what,char with){
    for (int i=0; i<strlen(data); i++)
        if(data[i]==what)data[i]=with;
    return data;// chain
}

void appendFile(const char* fileName,const char* data){
	FILE *fp= fopen(fileName,"a");
	if(fp==0){pf("CANNOT APPEND to FILE %s\n",fileName); return;}
	fprintf(fp,"%s\n",data);
	fclose(fp);
}

void printlabels(){
	for (int i=0; i<statementCount(); i++) {
		Statement* s= getStatement(i);
		if(checkNode(s->predicate))
			p(s->Predicate()->name);
	}
}