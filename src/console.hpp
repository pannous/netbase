//void console();
#include "netbase.hpp"
char* getaline();
void console();
void showHelp();

extern "C" Node** execute(const char* data,int* hits=0);// and act
NodeVector parse(const char* data);// and act