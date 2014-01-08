#pragma once
#include <string>
#include <vector>


extern std::string basepath;
using namespace std;

void importAll();
void importAllYago() ;
void importWordnet();
void importGeoDB();
void importNames();
void importFreebase();
//void import(const char* file);
void import(const char* type,const char* filename=0);
void importList(const char* facts_file,const char* type);
void importCsv(const char* facts_file,Node* type=0,char separator=0,const char* ignoredFields=0,const char* fields=0,int nameRowNr=-1,const char* nameRow=0 );
//void importXml(const char* facts_file,Context* context=null, const char* ignoredFields=null, const char* includedFields=null);
void importXml(const char* facts_file, char* nameField=0, const char* ignoredFields=0, const char* includedFields=0);

int getFields(char* line, vector<char*>& fields, char separator = 0, int nameRowNr = -1, const char* nameRow = 0);
char* extractTagName(char* line);
char* extractTagValue(char* line);
Node* getFreebaseEntity(char* name);