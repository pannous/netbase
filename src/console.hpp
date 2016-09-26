//void console();
#include "netbase.hpp"
char* getaline();
void console();
void showHelpMessage();

extern "C" Node** execute(const char* data,int* hits=0);// and act
NodeVector parse(const char* data0,bool safeMode/*true*/);
