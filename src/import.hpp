#pragma once
#include <string>
#include <vector>


extern std::string basepath;
using namespace std;

void importAll();
void importAllDE();
void importAllYago() ;
void importAmazon();
void importWordnet();
void importGeoDB();
void importTelekom();
void importNames();
void importFreebase();
void importWikiData();
void testImportWiki();// Test new impart features
void importRemaining();// fix
//void import(const char* file);
extern "C" void import(const char* type,const char* filename=0);
void importImages();
void importList(const char* facts_file,const char* type);
#define AUTO_ROW -1
void importCsv(const char* facts_file,Node* type=0,char separator=0,const char* ignoredFields=0,const char* fields=0,int nameRowNr=AUTO_ROW,const char* nameRow=0 );
void importJson(const char* file, Node* type=0, const char* ignoredFields=0, const char* foldFields=0);
//void importXml(const char* facts_file,Context* context=null, const char* ignoredFields=null, const char* includedFields=null);
void importXml(const char* facts_file, char* nameField=0, const char* ignoredFields=0, const char* includedFields=0);
bool importN3(const char* file);
int getFields(char* line, vector<char*>& fields, char separator = 0, int nameRowNr = -1, const char* nameRow = 0);
char* extractTagName(const char *line);
char* extractTagValue(char* line);
Node *getEntity(char *name);
Node* rdfValue(char* name);
