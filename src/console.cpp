#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <string.h>
#include <vector>
#include <string>

//#include "webserv/webserv.h"
#include "webserver.hpp"

#include "netbase.hpp"
#include "util.hpp"
#include "export.hpp"
#include "import.hpp"
#include "tests.hpp"
#include "query.hpp"
#include "init.hpp"
// for console:
#ifdef console
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#endif



using namespace std;
// static struct termios stored_settings;

static vector<char*> history;




void showHelp() {
    ps("available commands");
    ps("help :h or ?");
    ps("save :s or :w");
	ps("load :l\tfetch the graph from disk or mem");
    ps("import :i [<file>]");
    ps("load_files :lf");
//    ps("print :p");
    ps("delete :d <node|statement|id|name>");
    ps("save and exit :x");
    ps("quit :q");
    ps("clear :cl");
    ps("type words, ids, or queries:");
    ps("all animals that have feathers");
    ps("select * from dogs where terrier");
}

//bool parse(string* data) {
bool parse(const char* data) {
    if (eq(data, null)) {
        return true;
    }
    if (eq(data, "")) {
        return true;
    }

    string arg=next(string(data));
    //		scanf ( "%s", data );
    if (eq(data, "exit"))return false;
    if (eq(data, ":x")) {
        save();
		exit(1);
        return false;
    }
    if (eq(data, ":w")) {
        save();
    }
    if (eq(data, ":q")){exit(1);return false;}
    if (eq(data, "q"))return false;
    if (eq(data, "x"))return false;
    if (eq(data, "export"))export_csv();
    if (eq(data, ":cl"))clearMemory();
	if (eq(data, "clear"))clearMemory();
    if (eq(data, ":e"))export_csv();
    if (eq(data, "quit"))return false;
    if (eq(data, "quiet"))quiet = !quiet;
    if (eq(data, "help") || eq(data, "?")) {
        showHelp();
        printf("type exit or word");
        return true;
    }
    //        if (eq(data, "collect abstracts")) {
    //            collectAbstracts2();
    //            return true;
    //        }
    if (startsWith(data, ":i") || startsWith(data, "import")) {
        if(arg.length()>2) // && arg<0x7fff00000000
           import(arg.c_str(),basepath.c_str());
        else
			importAll();
        return true;
    }
    if (eq(data, "load")) {
        load(false);
        return true;
    }
    if (eq(data, "load_files")) {
        load(true);
        return true;
    }
    if (eq(data, ":l")) {
        load(false);
        return true;
    }
    if (eq(data, ":lf")) {
        load(true);
        return true;
    }
    if (eq(data, "save")) {
        save();
        return true;
    }
    if (eq(data, ":s")) {
        save();
        return true;
    }
	if (eq(data, ":c")) {
        showContext(currentContext());
        return true;
    }
    if (eq(data, ":t") || eq(data, "test") || eq(data, "tests")) {
        exitOnFailure = false;
        tests();
        return true;
    }
    if (eq(data, "debug")) {
        debug = true;
        return true;
    }
    if (eq(data, "debug on")) {
        debug = true;
        return true;
    }
    if (eq(data, "debug off")) {
        debug = true;
        return true;
    }
    if (eq(data, "no debug")) {
        debug = true;
        return true;
    }
    if (startsWith(data, ":d ")||startsWith(data, "delete ")||startsWith(data, "del ")||startsWith(data, "remove ")) {
        remove(next(data).c_str());
        return true;
    }

    if (startsWith(data, "path")) {
		vector<char*> args=splitString(data," ");
        Node* from=getAbstract(args.at(1));
        Node* to=getAbstract(args.at(2));
		shortestPath(from,to);
        return true;
    }
    if (startsWith(data, "select ")) {
        query(data);
        return true;
    }
    if (startsWith(data, "all ")) {
        query(data);
        return true;
    }
    if (startsWith(data, "these ")) {
        query(data);
        return true;
    }
    if (contains(data, "that ")) {
        query(data);
        return true;
    }
    if (contains(data, "who")) {// who loves jule
        query(data);
        return true;
    }

    if (startsWith(data, "the ")||startsWith(data, "my ")) {
        show(getThe(data));// -> query
        return true;
    }
    if (startsWith(data, "an ")) {
        query(data);
        return true;
    }
    if (startsWith(data, "a ")) {
        query(data);
        return true;
    }
    if (startsWith(data, "any ")) {
        query(data);
        return true;
    }

    //        if(startsWith(data,"is ")){  check_statement(data);return true;}
    //        if(startsWith(data,"does ")){  check_statement(data);return true;}
    if (contains(data, " in ")) {
        query(data);
        return true;
    }
    if (contains(data, " of ")) {
        query(data);
        return true;
    }
    if (contains(data, " with ")) {
        query(data);
        return true;
    }
    if (contains(data, " where ")) {
        query(data);
        return true;
    }
    if (contains(data, " from ")) {
        query(data);
        return true;
    }
    if (contains(data, " whose ")) {
        query(data);
        return true;
    }
    if (contains(data, " which ")) {
        query(data);
        return true;
    }
    if (contains(data, " without ")) {
        query(data);
        return true;
    }
    if (contains(data, "ing ")) {
        query(data);
        return true;
    }
    if (contains(data, " that ")) {
        query(data);
        return true;
    }
    if (contains(data, "who ")) {
        query(data);
        return true;
    }
    if (!isprint(data[0])) {
        return false;
    }
	if(splitString(data," ").size()>2){
		learn(data);
		return true;
	}

    //        query(string("all ")+data);// NO!
    //        query(data);// NOO!
    findWord(currentContext()->id, data);
    int i = atoi(data);
    if (i > 0) {
        if (i < 1000)showContext(i);
        showNr(currentContext()->id, i);
    }
}
// todo: all species => select * from specy


//int filter(string* word,string* filters){
//
//}
//void parseXPath(string* c){
//p=c->c_str();
//int len=strlen(p);
//for(int i=0;i<len;i++){
//if(p[i]=='.'){
//    string word=c->substr(0,i);
//    string filters=c->substr(i,len);
//    filter(word,filters);
//}
//
//}
////    c->
//}



int mygetch( ) {
  struct termios oldt,
                 newt;
  int            ch;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
// putc(c_cc[VINS]);
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}
void clearline(){
	printf("\r");
	for(int i=0;i<80;i++)printf(" ");
	printf("\r");
}
char* getaline(){//vector<char*> history
	char* line=(char*)malloc(1000);
	line[0]=0;
	int d=0;
	int pos=0;
	int size=history.size();
    if(size==0)history.push_back("");
	int nr=size;
	char* b=(char*)malloc(4);;//=(string("x")).c_str();
	b[0]=0;
	b[1]=0;
		int c;
	while(true){
	c=mygetch();
	b[0]=(char)c;
	char insert=45;
	if(c==0x0a)break;//enter
	// if(e==0x1b)

	if(c==0x7f){//  backspace
		printf("\b \b");
		pos--;
		strcpy(&line[pos],&line[pos+1]);
			line[strlen(line)]=0;
			if(line[pos])
			printf("%s  \b\b",&line[pos]);
			for(int i=0;i<strlen(&line[pos]);i++)printf("\b");
	}
			// c_cc[VINS]

	// printf("%c",insert);

	printf("%c",c);
		if(d==0x33 && c==0x7e){// del
				strcpy(&line[pos],&line[pos+1]);
				line[strlen(line)]=0;
				if(line[pos])
				printf("%s \b",&line[pos]);
				for(int i=0;i<strlen(&line[pos]);i++)printf("\b");
			// printf("%s",&line[pos]);
			// printf(" ");
		}

	if(d==0x5b){

		// 0x1b 5b 44 == right
		if(c==0x43)
			if(pos<strlen(line))
				pos++;
			else printf("\b");


		// 0x1b 5b 44 == left
		if(c==0x44)
			pos--;

		// 0x1b 5b 41 == up
		if(c==0x41 ){
			printf("\n");
			clearline();
			if(size>0){
			nr=(nr-1)%size;
			if(nr<0||nr==size)nr=size-1;
			printf(history[nr]);
			strcpy(line,history[nr]);
			pos=strlen(history[nr]);
			}
			// printf("last");
		}

		// 0x1b 5b 41 == down
		if(c==0x42){
			printf("\r");
			clearline();
			if(size>0){
			nr=(nr+1)%size;
			printf(history[nr]);
			strcpy(line,history[nr]);
			pos=strlen(history[nr]);
			}
			// printf("down");
		}
		// if(c==0x43){
			// printf(history[nr]);
		// }
		// replaceLast(line,pos,3);
	}
	else
	if(isprint(b[0]) && c!=0x1b && c!=0x5b && c!=0x7f && c!=0x7e){
		char tmp[1000];
		strcpy(tmp,&line[pos]);
		strcpy(&line[pos+1],tmp);
		line[pos++]=b[0];
		if(line[pos])
		printf("%s",&line[pos]);
	for(int i=0;i<strlen(tmp);i++)printf("\b");
}
// if(c==9){}//tab
if(d==0x1b&&c==0x7f){// alt back
	pos=0;
	line[0]=0;
	line[1]=0;
	clearline();
}

// keypad(stdscr, TRUE);
	d=c;
}
if(strlen(line)>0)
history.push_back(line);
printf("\n");
return line;
}
// all car_makes

//
// int main(){
// 	history.push_back("test0");
// 	history.push_back("test1");
// 	history.push_back("test2");
// 	while(1){
// 		printf("netbase>");
// 	char* a=getline();
// 	printf("\n{{>>>>>>>>>>>>>%s<<<<<<<<<<}}\n",a);
// 	printf("%d",history.size());
// 	}
// }



#ifdef signal
// CATCH signal and try recovery!
#include <setjmp.h>
#include <execinfo.h>
jmp_buf try_context;
jmp_buf loop_context;
jmp_buf loop_context1;

void print_trace(const char * msg) {
    void *array[100];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, 100);
    strings = backtrace_symbols(array, size);
    printf("%s Obtained %zd stack frames.\n", msg, size);
    for (i = 0; i < size; i++)
        printf("--> %s\n", strings[i]);
    free(strings);
}

void SIGINT_handler(int sig) {
    print_trace("SIGINT!\n");
    flush();
    longjmp(loop_context, 128);
}

void handler(int sig) {
    print_trace("EXC_BAD_ACCESS !\n");
    flush();
    longjmp(try_context, 128);
}
#endif

void console() {
    Node* n;
    int i;
    quiet = false;
    printf("\nNetbase C++ Version z.a");
    const char* data = (char*) malloc(1000);
    string data2;
#ifdef signal
    setjmp(try_context); //recovery point
#endif
    while (true) {
        clearAlgorithmHash();
        printf("\nnetbase> ");
        flush();
        std::getline(std::cin, data2);
        data = data2.c_str();
        parse(data);
    }
	start_server();

    //	p("farewell...");
}



char* serve(const char* data) {
	if(parse(data))return "success";// stdout.to_s
	else return "NOPE";
}
