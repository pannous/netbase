//#pragma once

#include <string.h> // strcmp ...
#include <cstdlib>
#include <string>
#include <map>
#ifdef sqlite3
#include "sqlite3.h"
#endif

#include "netbase.hpp"
#include "util.hpp"
#include "import.hpp"
#include "relations.hpp"// for wordnet
#include "reflection.hpp"

using namespace std;

// 64 BIT : %x -> %016llX
cchar* nodes_file="nodes.txt";
cchar* statements_file="statements.txt";
cchar* images_file="images.txt";

FILE *open_file(const char* file) {
	FILE *infile;
	if ((infile=fopen((file), "r")) != NULL) return infile;
	if (import_path.length() == 0) import_path="import/";
	if (!startsWith(file, "/")) file=(import_path + file).data(); // concat(import_path.data(), file);
	if ((infile=fopen((file), "r")) == NULL) {
		perror("Error opening file");
		printf(" %s\n", (file));
		exit(1);
	}
	return infile;
}

void norm(char* title) {
	int len=(int)strlen(title);
	for (int i=len; i >= 0; --i) {
		if (title[i] == ' ' || title[i] == '_' || title[i] == '-') {
			strcpy(&title[i], &title[i + 1]); //,len-i);
		}
	}
}

bool checkLowMemory() {
	size_t currentSize=getCurrentRSS(); //
	size_t peakSize=getPeakRSS();
	size_t free=getFreeSystemMemory();
    //	if (!free) free=5.5L * GB;// 2 GB + work
	if (!free) free=9.0L * GB; // 4 GB + work
	if (currentSize > free) {
		p("OUT OF MEMORY!");
		printf("MEMORY: %zX Peak: %zX FREE: %zX \n",  currentSize, peakSize, free);
		return true;
	}
	//		|| currentSize*1.2>sizeOfSharedMemory||
	if (currentContext()->nodeCount + 20000 > maxNodes) {
		p("OUT OF MEMORY!");
		pf("%d nodes!\n", currentContext()->nodeCount);
		return true;
	}
	if (currentContext()->currentNameSlot + 300000 > maxNodes * averageNameLength) {
		p("OUT OF MEMORY!");
		pf("%d characters!\n", currentContext()->currentNameSlot);
		return true;
	}
	if (currentContext()->statementCount + 40000 > maxStatements0) {
		p("OUT OF MEMORY!");
		pf("%d nodes!\n", currentContext()->statementCount);
		return true;
	}
	if (extrahash + 20000 > abstracts + abstractHashSize * 2) {
		p("OUT OF MEMORY!");
		pf("hashes near %p\n", extrahash);
		return true;
	}
	return false;
}

//int wordnet_synset_map[117659];
std::map<int, int> wordnet_synset_map;

int norm_wordnet_id(int synsetid) {
	if (synsetid < million) return synsetid;
	int id=wordnet_synset_map[synsetid];
	if (!id) p("BAD ID!!!");
	//	id=id+10000;// NORM!!!
	return id;
	//	return (synsetid%million)+200000;
}

void load_wordnet_synset_map() {
	if (wordnet_synset_map.size() > 0) return;
	char line[1000];
	FILE *infile=open_file("wordnet/synset_map.txt");
	int s, id;
	while (fgets(line, sizeof(line), infile) != NULL) {
		fixNewline(line);
		sscanf(line, "%d\t%d", &s, &id);
		wordnet_synset_map[s]=id;
	}
}

map<long, string> nodeNameImages;
map<long, string> nodeNameImages2; // chopped
//map<long,string> nodeNameImages3;// chopped image name

void importImages() { // 18 MILLION!   // 18496249
	p("image import starting ...");
	char line[100];
	char* lastTitle=0;
	int linecount=0;
	Node* wiki_image=getAbstract("wiki_image");
	addStatement(wiki_image, is_a, getThe("image"));
    
	/* Open the file.  If NULL is returned there was an error */
	FILE* infile=open_file((char*) images_file);
//	char tokens[1000];
	char image[1000];
	char *title=(char *) malloc(1000);
	int good=0;
//	int bad=0;
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 10000 == 0) {
			pf("importImages %d    \r",linecount);
            fflush(stdout);
		};
		sscanf(line, "%s %*s %s", title, image);
		if (eq(lastTitle, title)) continue;
        
		lastTitle=clone(title); // only the first
		if (!hasWord(title)) norm(title); //blue -_fin ==> bluefin
		if (!hasWord(title) && !hasWord(downcase(title))) continue; // currently only import matching words.
        
		//            if(++bad%1000==0){ps("bad image (without matching word) #");pi(bad);}
		//		if (getImage(title) != "")
		//			continue; //already has one ; only one so far!
		Node* subject=getAbstract(title);
		Node* object=getAbstract(image); // getThe(image);;
		//		if(endswith(image,".ogg")||endswith(image,".mid")||endswith(image,".mp3"))
		addStatement(subject, wiki_image, object, false);
		printf("%s\n", title);
		//		if (!eq(title, downcase(title),false))
		//			addStatement(getAbstract(downcase(title)), wiki_image, object, false);
		if (++good % 10000 == 0) {
			ps("GOOD images:");
			p(good);
		}
	}
	fclose(infile);
    
	good=0;
//	Node* object=getAbstract(image); // getThe(image);;
    
	/*
	 // again, this time with word fragments
	 * MEMORY LEAK!? where??
	 infile = fopen((import_path+ images_file).data(), "r");
	 while (fgets(line, sizeof (line), infile) != NULL) {
	 if (++linecount % 1000 == 0)pi(linecount);
	 sscanf(line, "%s %*s %s", title, image);
	 //        #blue fin ==> blue and fin
	 char* word=title;
	 for (int i = 0; i < strlen(title); i++) {
	 if(title[i]==' '||title[i]=='_'||title[i]=='-'){//split
	 title[i]=0;
	 //                nodeNameImages2[hash(title)]=image;
	 if(!hasWord(word))continue;
	 if(getImage(word)!="")continue;// only one so far!
	 addStatement(getAbstract(word), wiki_image, object,false);
	 if(++good%1000==0){ps("!!good");pi(good);}
     
	 word=&title[i+1];
	 }
	 }
	 }
	 */
	//    for(map<long,string>::const_iterator iter = nodeNameImages2.begin(); iter != nodeNameImages2.end(); ++iter){
	//        long key=iter->first;
	//        string value=iter->second;
	//        if(nodeNameImages.find(key)==nodeNameImages.end())// does not exist?
	//            nodeNameImages[key]=value;
	//    }
	//    for(map<long,Node*>::const_iterator iter = abstracts->begin(); iter != abstracts->end(); ++iter){
	//        long key=iter->first;
	//        Node* node=iter->second;
	//        string image=nodeNameImages[key];
	////        if(!image)key=hash2(node->name)
	//        Node* object = getAbstract(image.data());// getThe(image);;
	//        addStatement(node, predicate, object,false);
	//    }
	fclose(infile);
	p("\ndone image import\n");
}

void importNodes() {
	p("node import starting ...");
	// char fname[40]="/Users/me/data/base/netbase.sql";
	//  ('2282','Anacreon','N'),
    
	// char* fname="/Users/me/data/base/netbase/nodes.test";
	//2026896532	103	dry_out	Verb	\N	\N	11	103
    
	char line[100];
	int linecount=0;
	FILE *infile=open_file(nodes_file);
	while (fgets(line, sizeof(line), infile) != NULL) {
		char tokens[1000];
		/* Get each line from the infile */
        
		if (++linecount % 1000 == 0){
            pf("importNodes %d    \r",linecount);
            fflush(stdout);
        }
		strcpy(tokens, line);
		int x=0; // replace ' ' with '_'
		while (tokens[x]) {
			if (tokens[x] == ' ') tokens[x]='_';
			x++;
		}
		// char name[1000];
		char* name=(char*) malloc(100);
		// char kind[20];
		char contextId_s[100];
//		char deleted[1];
//		char version[1];
//		char wordKindName[100];
//		int wordKind;
		int Id;
		int kind;
		int contextId=wordnet;
//		int contextID;
		//2026896532	103	dry_out	Verb	\N	\N	11	103
		//	sscanf(tokens,"%d\t%s\t%s\t%s\t%*s\t%*s\t%*d\t%d",&Id,contextId_s,name,wordKindName,&kind);
		sscanf(tokens, "%d\t%s\t%s\t%*s\t%*s\t%*s\t%d\t%*d", &Id, contextId_s, name, &kind); // wordKind->kind !
		if (kind == 105) kind=_relation; //relation
		if (kind == 1) kind=_concept; //noun
		//        if(kind==1)continue;
		if (kind == 103) kind=noun; //noun
		if (kind == 10) kind=noun; //noun
		if (kind == 11) kind=verb; //verb
		if (kind == 12) kind=adjective; //adjective
		if (kind == 13) kind=adverb; //adverb
		//        contextId = atoi(contextId_s);
		//
		//        if (Id < 1000)contextId = wordnet; //wn
		//        if (contextId == 100 && Id > 1000)contextId = 100;
		Node* n;
		if (Id > 1000) n=add(name, kind, contextId);
		else n=add_force(contextId, Id, name, kind);
		if (n == 0) {
			p("out of memory");
			break;
		}
		wn_map[Id]=n->id;
        //		wn_map2[n->id]=Id;
	}
	fclose(infile); /* Close the file */
	p("\nnode import ok\n");
}

void importStatements2() {
    
	char line[1000];
	int linecount=0;
    
	FILE *infile=open_file(statements_file);
	while (fgets(line, sizeof(line), infile) != NULL) {
		/* Get each line from the infile */
		if (++linecount % 1000 == 0){
            pf("importStatements %d    \r",linecount);
            fflush(stdout);
        }		// p(line);
		//	strcpy(  tokens,line);
		//		int x = 0;
		//    while (tokens[x++])
		//        if (tokens[x]==' ')
		//		    tokens[x]='_';
//		int contextId;
		int subjectId;
		int predicateId;
		int objectId;
		int id; // ignore now!!
		sscanf(line, "%d\t%d\t%d\t%d", &id, &subjectId, &predicateId, &objectId); // no contextId? cause this db assumes GLOBAL id!
		subjectId=wn_map[subjectId]; //%1000000;//wn!!
        //		predicateId = predicateId; //%1000000;
		if (predicateId > 100) predicateId=wn_map[predicateId]; //%1000000;
		objectId=wn_map[objectId]; //%1000000;
		// printf("%d\t%d\t%d\n",subjectId,predicateId,objectId);
		if (subjectId < 1000 || objectId < 1000 || predicateId == 50 || predicateId == 0 || subjectId == 1043 || subjectId == 1044)
			continue;
//		Statement* s=
        addStatement4(wordnet, subjectId, predicateId, objectId);
	}
	fclose(infile); /* Close the file */
	p("\nstatements import ok");
}

#ifdef sqlite3
int maxBytes = 1000000;

void importSqlite(char* filename) {
	sqlite3* db;
	sqlite3_stmt *statement;
	const char* unused = (char*) malloc(1000);
	int status = sqlite3_open(filename, &db);
	p(status);
	status = sqlite3_prepare(db, "select * from nodes;", maxBytes, &statement, &unused);
	p(status);
    
	// http://www.sqlite.org/c3ref/step.html
	status = sqlite3_step(statement);
	p(status);
	if (status == SQLITE_DONE) {
		p("No results");
	}
	if (status == SQLITE_ROW) {
		// http://www.sqlite.org/c3ref/column_blob.html
		int id = sqlite3_column_int(statement, 0);
		const unsigned char *text = sqlite3_column_text(statement, 1);
		printf("text %s\n", text);
	}
	sqlite3_finalize(statement);
	sqlite3_close(db);
}
#endif

string deCamel(string s) {
	static string space=" ";
	for (int i=(int)s.length(); i > 1; i--) {
		char c=s[i];
		if (c > 65 && c < 91) s.replace(i, 1, space + (char) tolower(c));
		if (c == '(') s[i - 1]=0; //cut _( )
		if (c == ',') s[i]=0; //cut ,_
		if (c == ')') s[i]=0;
	}
	//    s[0]=tolower(s[0]);
	s=replace_all(s, "_", " ");
	s=replace_all(s, "  ", " ");
	return s;
}
;

// TODO :!test MEMLEAK!

const char* parseWikiTitle(char* item, int id=0, int context=current_context) {
	//    string s="wordnet_raw_material_114596700";
	static string s;
	s=string(item); //"Uncanny_Tales_(Canadian_pulp_magazine)";
	//    South_Taft,_California
	// facts/hasPopulationDensity/ArticleExtractor.txt:205483101	Sapulpa,_Oklahoma	397.1#/km^2	0.9561877303748038
    
	//    string s="wordnet_yagoActorGeo_1";
	//    wordnet_yagoActorGeo_1
	//    if(s.find("wordnet"))contextId=wordnet;
	//if(s.find("wikicategory")) kind=list / category
	s=replace_all(s, "wikicategory_", "");
	s=replace_all(s, "wordnet_(.*)_(\\d*)", "$1"); // todo
	s=replace_all(s, "yago_", "");
	s=replace_all(s, "wikicategory", "");
	s=replace_all(s, "wordnet", "");
	s=replace_all(s, "yago", "");
	int last=(int)s.rfind("_");
	int type=(int)s.find("(");
	string clazz=deCamel(s.substr(type + 1, -1));
	string word=s;
	string Id=s.substr(last + 1, -1);
	//    int
	id=atoi(Id.c_str());
	if (id > 0) {
		word=s.substr(0, last);
	}
	//    word=deCamel(word);
	//    item=word.c_str();
	//    ~word;
	return word.c_str(); // TODO!! MEMLEAK!!
}

void extractUnit() {
	string s="397.1#/km^2";
}

char* charToCharPointer(char c) {
	char separator[2]= { c, 0 };
	char* separatorx=separator;
	return separatorx;
}

char guessSeparator(char* line) {
	const char* separators=",\t;|";
	char the_separator='\t';
	int max=0;
	int size=(int)strlen(separators);
	for (int i=0; i < size; i++) {
		int nr=splitStringC(line, 0, (char) separators[i]);
		if (nr > max) {
			the_separator=separators[i];
			max=nr;
		}
	}
	return the_separator;
}

int getNameRow(char** tokens, int nameRowNr=-1, const char* nameRow=0) {
	int row=0;
	while (true) {
		char* token=tokens[row];
		if (!token) break;
		if (nameRowNr < 0) {
			if (nameRow == 0) {
				if (eq("name", token)) nameRowNr=row;
				if (eq("title", token)) nameRowNr=row;
			} else if (eq(nameRow, token)) nameRowNr=row;
		}
		row++;
	}
	return nameRowNr;
}

// BROKEN!!!??!!

int getFields(char* line, vector<char*>& fields, char separator, int nameRowNr, const char* nameRow) {
	char * token;
	cchar* separators=",;\t|";
	if (separator) separators=charToCharPointer(separator);
	//	line=modifyConstChar(line);
	token=strtok(line, separators);
	int row=0;
	while (token != NULL) {
		fields.push_back(token);
		token=strtok(NULL, separators); // BROKEN !!??!!??!!??
		row++;
	}
	for (int i=0; i < row; i++) {
		token=fields.at(i);
		if (nameRowNr < 0) {
			if (nameRow == 0) {
				if (eq("name", token)) nameRowNr=row;
				if (eq("title", token)) nameRowNr=row;
			} else if (eq(nameRow, token)) nameRowNr=row;
		}
	}
	return nameRowNr;
}

void fixNewline(char* line) {
	int len=(int)strlen(line);
	if (len == 0) return;
	if (line[len - 1] == '\n') line[--len]=0;
	if (line[len - 1] == '\r') line[--len]=0;
	if (line[len - 1] == '\t') line[--len]=0;
}

char* extractTagName(char* line) {
	return match(line, "<([^>]+)>");
}

char* extractTagValue(char* line) {
	return match(line, ">([^<]+)<");
}

bool isNameField(char* field, char* nameField) {
	if (nameField && !eq(field, nameField)) return false;
	if (eq(field, nameField)) return true;
	if (eq(field, "name")) return true;
	if (eq(field, "title")) return true;
	if (eq(field, "label")) return true;
	return false;
}

Node* namify(Node* node, char* name) {
	Context* context=currentContext();
	node->name=&context->nodeNames[context->currentNameSlot];
	strcpy(node->name, name); // can be freed now!
	int len=(int)strlen(name);
	context->nodeNames[context->currentNameSlot + len]=0;
	addStatement(getAbstract(name), Instance, node, DONT_CHECK_DUPLICATES);
	// replaceNode(subject,object);
	return node;
}

void addAttributes(Node* subject, char* line) {
	line=(char*) replace_all(line, "\"", "'").c_str();
	do {
		char* attribute=match(line, " ([^=]+)='[^']'");
		char* value=match(line, " [^=]+='([^'])'");
		if (!attribute) break;
		Node* predicate=getThe(attribute);
		Node* object=getThe(value);
		addStatement(subject, predicate, object);
	} while (attribute != 0);
}

bool hasAttribute(char* line) {
	return match(line, " ([^=]+)=");
}

//Context* context,
//Node* type,
// importXml("/Users/me/data/base/geo/geolocations/Orte_und_GeopIps_mit_PLZ.xml","city","ort");

void importXml(const char* file, char* nameField, const char* ignoredFields, const char* includedFields) {
	p("import csv start");
//	bool dissect=false;
	char line[1000];
	char* line0=(char*) malloc(sizeof(char*) * 100);
	char* field=(char*) malloc(sizeof(char*) * 100);
	char* value=(char*) malloc(sizeof(char*) * 10000000); // uiuiui!
    
	Node* root=0;
	Node* parent=0; // keep track of 1 layer
	Node* subject=0;
	Node* predicate=0;
	Node* object=0;
	vector<Node*> predicates=*new vector<Node*>();
	map<char*, char*> fields;
	queue<Node*> parents; //constructed!!
    
	//	char* objectName = (char*) malloc(100);
	//	int depth = 0;
	//	vector<char*> ignoreFields = splitString(ignoredFields, ",");
	//	vector<char*>& includeFields = splitString(includedFields, ",");
	int linecount=0;
	FILE *infile=open_file(file);
//	Node* UNKNOWN_OR_EMPTY=getThe("<unknown/>");
	//	map<Node*,Node*> fields;
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (!line) break;
		if (++linecount % 1000 == 0) {
            pf("importXml %d    \r",linecount);
            fflush(stdout);
        }fixNewline(line);
		line0=line; // readable in Debugger!
		if (contains(line, "<?xml")) continue;
		if (!subject) {
			if (root) subject=add(extractTagName(line));
			else root=add(extractTagName(line));
			continue;
		}
        
		if (startsWith(line, "</")) {
			//			for(Node* predicate:fields){
			//				object = getThe(extractTagValue(line),null, dissect);
			//				Statement *s = addStatement(subject, predicate, object, false);
			//				showStatement(s);
			//			}
			//			if(!parents.empty())
			subject=0; // parents.back(); // back????
			//			parents.pop();
			fields.clear();
			continue;
		}
		// <address> <city> <zip>12345</zip> <name>bukarest</name> </city> </address>
		if (match(line, "<[a-z]") && contains(line, "</")) {
			field=extractTagName(line);
			value=extractTagValue(line);
			if (ignoredFields && contains(ignoredFields, field)) continue;
			if (isNameField(field, nameField)) {
				//				parent=parents.front();
				// rewrite address.member=city
				//				deleteStatement(findStatement(parent, Member, subject, 0, 0, 0, 0));
				// address.CITY=bukarest
				predicate=getThe(subject->name); // CITY
				Node* object=namify(subject, value); // city.name=bukarest
				addStatement(parent, predicate, object, DONT_CHECK_DUPLICATES); // address.CITY=bukarest
				subject=object; // bukarest
				//				show(subject,true);
				continue;
			}
            
			if (!value) {				//<address> <city> ...
				//				parents.push(subject);
				parent=subject;
                
				//			if(eq(field,"zip")){
				//				value=value;
				//				showStatement(s);
				//			}
                
				if (!contains(line, "><")) {				// else empty!
					object=add(field);
					addStatement(subject, Member, object,
                                 DONT_CHECK_DUPLICATES);
					subject=object;
				} else {
					object=getThe(field);
					//					addStatement(subject, Unknown,object,DONT_CHECK_DUPLICATES);//EMPTY
					addStatement(subject, object, Unknown,
                                 DONT_CHECK_DUPLICATES); //EMPTY
					//					addStatement(subject, object,UNKNOWN_OR_EMPTY,DONT_CHECK_DUPLICATES);//EMPTY
				}
				addAttributes(subject, line);
				continue;
			}
			if (hasAttribute(line)) {
				predicate=add(field); // <zip color='green'>12345</zip>
				addAttributes(predicate, line);
			} else {
				predicate=getThe(field, NO_TYPE);
			}
			object=getThe(value, NO_TYPE);
//			Statement* s=
            addStatement(subject, predicate, object, DONT_CHECK_DUPLICATES);
            
			//			fields.insert(predicate,object);//
			continue;
		}
		if (startsWith(line, "<") && !contains(line, "</")) {
			parent=subject;
			//			parents.push(subject); // <address> <city> ...
			field=extractTagName(line);
			subject=add(field); // can be replaced by name!
			addStatement(parent, Member, subject, DONT_CHECK_DUPLICATES); // address.city
			addAttributes(subject, line);
			continue;
		}
	}
	fclose(infile); /* Close the file */
	p("import xml ok ... items imported:");
	p(linecount);
}

void importCsv(const char* file, Node* type, char separator, const char* ignoredFields, const char* includedFields, int nameRowNr,
               const char* nameRow) {
	p("import csv start");
	char line[1000];
	//	char* line=(char*)malloc(1000);// DOESNT WORK WHY !?!
	char** values=(char**) malloc(sizeof(char*) * 100);
	char* lastValue=0;
	memset(values, 0, 100);
	//    vector<char*> values;
    
	Node* subject=0;
	Node* predicate=0;
	Node* object=0;
	vector<Node*> predicates=*new vector<Node*>();
	vector<char*> ignoreFields=splitString(ignoredFields, ",");
	vector<char*>& includeFields=splitString(includedFields, ",");
	//	vector<char*>& fields = *new vector<char*>();
	int linecount=0;
	FILE *infile=open_file(file);
//	char* objectName=(char*) malloc(100);
	int fieldCount=0;
	char* columnTitles;
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (!line) break;
		fixNewline(line);
		if (linecount == 0) {
			columnTitles=line;
			if (!separator) separator=guessSeparator(modifyConstChar(line)); // would kill fieldCount
			fieldCount=splitStringC(line, values, separator);
			nameRowNr=getNameRow(values, nameRowNr, nameRow);
			for (int i=0; i < fieldCount; i++) {
				char* field=values[i];
				Node* fielt=getThe(field); // Firma		instance		Terror_Firma LOL
				dissectWord(fielt);
				predicates.push_back(fielt);
			}
			++linecount;
			continue;
		}
		if (++linecount % 1000 == 0) {
            pf("importXml %d    \r",linecount);
            fflush(stdout);
        }
        
		//        values.erase(values.begin(),values.end());
		//        ps(line);
        
		int size=splitStringC(modifyConstChar(line), values, separator);
		if (fieldCount != size) {
            //			if(debug) printf("Warning: fieldCount!=columns in line %d   (%d!=%d)\n%s\n", linecount - 1, fieldCount, size, line);
			//            ps(columnTitles); // only 1st word:
			continue;
		}
        
		if (values[nameRowNr] != lastValue) subject=getNew(values[nameRowNr], type);
        //        else //keep subject
		lastValue=values[nameRowNr];
        //		subject=getThe(values[nameRowNr]);
        
		// conflict: Neustadt.lon=x,Neustadt.lat=y,Neustadt.lon=x2,Neustadt.lon=y2
		// 2 objects, 4 relations, 1 name
        
		if (!checkNode(subject)) continue;
		if (debug && subject && type && subject->kind > 0 && subject->kind != type->id) {		// subject->kind == 0 ??? MEANING?
			pf("Found one with different type subject->kind != type->id %d,%d,%s,%s\n", subject->kind, type->id, subject->name, type->name);
            //			subject = getThe(values[nameRowNr], type, dissect); // todo : match more or less strictly? EG Hasloh
		}
		for (int i=0; i < size; i++) {
			if (i == nameRowNr) continue;
			predicate=predicates[i];
			if (predicate == null) continue;
			if (contains(ignoreFields, predicate->name)) continue;
			if (includedFields != null && !contains(includeFields, predicate->name)) continue;
			char* vali=values[i];
			if (!vali || strlen(vali) == 0) continue; //HOW *vali<100??
			object=getThe(vali);
			if (!object || object->id > maxNodes) {
				badCount++;
				if (debug) printf("ERROR %s\n", line);
				continue;
			}
//			Statement *s=
            addStatement(subject, predicate, object, false);
			//			showStatement(s);
		}
        
		if (checkLowMemory()) {
			printf("Quitting import : id > maxNodes\n");
			break;
		}
	}
	fclose(infile); /* Close the file */
	p("import csv ok ... lines imported:");
	p(linecount);
}

void importList(const char* file, const char* type) {
	p("import list start");
	char line[1000];
	Node* subject=getClass(type);
	Node* object;
	int linecount=0;
	FILE *infile=open_file(file);
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 10000 == 0) {
            pf("importList %d    \r",linecount);
            fflush(stdout);
        }
		if (linecount % 10000 == 0 && checkLowMemory()) break;
		fixNewline(line);
		object=getThe(line);
		if (!object || object->id > maxNodes) {
			badCount++;
			if (debug) printf("ERROR %s\n", line);
			continue;
		}
		addStatement(object, Type, subject, false); // This should still be skipped If approached by subject!!! not object
		//		addStatement(subject, Instance, object, false);// This would not be detected with the instant gap
	}
	fclose(infile); /* Close the file */
	p("\nimport list ok");
}

char *cut_wordnet_id(char *key) {
	for (int i=(int)strlen(key); i > 1; --i) {
		if (key[i] == '_') {
			char* id=key + i + 1;
			key[i]=0;
			return id; // <wordnet_album_106591815> ->  06591815 DROP LEADING 1 !!!
		}
	}
	return 0;
}

bool hasCamel(char* key) {
	if (contains(key, "_")) return false;
	char last=key[0];
	for (int i=0; i < strlen(key); i++) {
		char c=key[i];
		if (c > 64 && c < 91) if (last > 96 && last < 123) {
			p(i);
			return true;
		}
		last=c;
	}
	return false;
}

char* removeHead(char *key, cchar *bad) {
	if (startsWith(key, bad)) return key + strlen(bad);
	return key;
}

char *fixYagoName(char *key) {
	if (key[0] == '<') key++;
	int len=(int)strlen(key);
	if (key[len - 1] == '>') key[len - 1]=0;
	key=removeHead(key, "wikicategory_");
	key=removeHead(key, "geoclass_");
	key=removeHead(key, "wordnetDomain_");
	if (startsWith(key, "wordnet_")) {
		p("SHOULDNT BE REACHED");
//		char* id=cut_wordnet_id(key);
		key=removeHead(key, "wordnet_");
	}
	//	if(hasCamel(key)) // NOO: McBain
	//		return deCamel(key).data();
	return key;
}

Node* rdfOwl(char* name) {
	if (!name) return 0;
	if (eq(name, "rdf:type")) return Type;
	if (eq(name, "rdfs:superClassOf")) return SuperClass;
	if (eq(name, "rdfs:subClassOf")) return SubClass;
	if (eq(name, "rdfs:label")) return Label;
	if (eq(name, "skos:prefLabel")) return Label;
	if (eq(name, "rdfs:Property")) return Relation;
	if (eq(name, "rdf:Property")) return Relation;
	if (eq(name, "rdfs:subPropertyOf")) return SubClass;
	if (eq(name, "owl:SymmetricProperty")) return Relation; // symmetric!
	if (eq(name, "owl:TransitiveProperty")) return Relation; // transitive!
	if (eq(name, "rdfs:domain")) return Domain;
	if (eq(name, "rdfs:range")) return Range;
    
	if (eq(name, "rdfs:comment")) return Comment;
	if (eq(name, "rdf:Statement")) return get(_statement);
	if (eq(name, "rdfs:class")) return Class;
	if (eq(name, "rdf:Resource")) return get(_node); // no info!
	if (eq(name, "rdfs:Resource")) return get(_node); // no info!
	if (eq(name, "rdfs:Literal")) return get(_node); // no info!
	if (eq(name, "xsd:string")) return get(_node); // no info!
	if (eq(name, "owl:Thing")) return get(_node); // no info!
	if (eq(name, "xsd:date")) return Date;
	if (eq(name, "xsd:decimal")) return Number;
	if (eq(name, "xsd:integer")) return Number;
	if (eq(name, "xsd:boolean")) return getAbstract("boolean"); // !
	if (eq(name, "xsd:gYear")) return getAbstract("year"); // !
	if (eq(name, "owl:disjointWith")) return getAbstract("disjoint with"); // symmetric!
	//	if(eq(key,"xsd:string"))return Text;// no info!
	if (eq(name, "xsd:nonNegativeInteger")) return getAbstract("natural number");
	if (eq(name, "owl:FunctionalProperty")) return Label;
	if (!startsWith(name, "wiki") && contains(name, ":")) {
		name=strstr(name, ":");
		if (!name) return 0; // contains(name, ":") WTF????????
		name=name + 2;
		//		printf(" unknown key %s\n", name);
		//		return 0;
	}
	return getThe(name); //Unknown;
}

Node* dateValue(const char* val) {
	return value(val, atoi(val), Date);
}
/*
 <g.11vjx3759>   <measurement_unit.dated_percentage.source>      <g.11x1gf2m6>   .
 <g.11vjx3759>   <#type> <measurement_unit.dated_percentage>     .
 <g.11vjx3759>   <measurement_unit.dated_percentage.date>        "2005-04"^^<#gYearMonth>        .
 <g.11vjx3759>   <measurement_unit.dated_percentage.rate>        4.5     .
 <g.11vjx3759>   <#type> <measurement_unit.dated_percentage>     .
 */
Node* rdfValue(char* name) {
	char** all=splitStringC(name, '^');
	cchar* unit=all[2];
	name=all[0];
	name++; // ignore quotes "33"
	free(all);
	if (!unit || unit > name + 1000 || unit < name) return 0;
	if (unit[0] == '<') unit++;
	if (unit[0] == '#') unit++;
	if (eq(unit, ",)")) return 0; // LOL_(^^,) BUG!
	if (eq(unit, "xsd:integer")) unit=0; //-> number
	if (eq(unit, "xsd:decimal")) unit=0; //-> number return value(key, atof(key), Number);; //-> number
	if (eq(unit, "xsd:float")) unit=0; //-> number
	if (eq(unit, "xsd:nonNegativeInteger")) unit=0; //-> number
	else if (eq(unit, "yago0to100")) unit=0;
	if (!unit) return value(name, atof(name), Number);
    
	if (eq(unit, "m")) unit="Meter";
	else if (eq(unit, "%")) ; // OK
	else if (eq(unit, "s")) unit="Seconds";
	else if (eq(unit, "r")) unit="Seconds";
	else if (eq(unit, "g")) unit="Gram";
	else if (eq(unit, "/km")) unit="Kilometer";
	else if (eq(unit, "km")) unit="Kilometer";
	else if (eq(unit, "xsd:date")) ; // parse! unit = 0; //-> number
	else if (eq(unit, "degrees")) ; // ignore
	else if (eq(unit, "dollar")) ; // ignore
	else if (eq(unit, "euro")) ; // ignore
	else if (eq(unit, "yagoISBN")) unit="ISBN"; // ignore
	else if (eq(unit, "yagoTLD")) unit="TLD"; // ???
	else if (eq(unit, "yagoMonetaryValue")) unit="dollar";
	else if (eq(unit, "gYear")) unit="year"; //Date;
	else if (eq(unit, "date")) return dateValue(name);
	else if (eq(unit, "dateTime")) return dateValue(name);
	else if (eq(unit, "gYearMonth")) return dateValue(name);
	else {
		printf("UNIT %s \n", unit); // "<" => SIGSEGV !!
		return 0;
	}
	//		, current_context, getYagoConcept(unit)->id
	//	return add(name);
	Node* unity=getThe(unit); // getThe(unit);//  getYagoConcept(unit);
	return value(name, atof(name), unity);
}

Node* parseWordnetKey(char* key) {
	char* id=cut_wordnet_id(key);
	int nid=norm_wordnet_id(atoi(id));
	Node* wn=get(nid);
	if (wn) return wn;
	else return initNode(wn, nid, removeHead(key, "<wordnet_"), 0, 0);
}

Node* getYagoConcept(char* key) {
	if (startsWith(key, "<wordnet_")) return parseWordnetKey(key);
	char* name=fixYagoName(key); // Normalized instead of using similar, key POINTER not touched
	if (contains(name, ":")) return rdfOwl(key);
	if (eq(name, "isPreferredMeaningOf")) return Label;
	if (eq(name, "#label")) return Label;
	if (eq(name, "label")) return Label;
	if (eq(name, "hasGloss")) return Label;
	if (eq(name, "hasWordnetDomain")) return Domain;
	//	if(eq(key,"owl:FunctionalProperty"))return Transitive;
	if (contains(name, "^^")) return rdfValue(key);
	//	if (contains(name, ".jpg") || contains(name, ".gif") || contains(name, ".svg") || startsWith(name, "#") || startsWith(name, "<#")) {
	//		printf(" bad key %s\n", name);
	//		return 0;
	//	}
	Node *n=getThe(name); //fixYagoName(key));
	//	dissectWord(n);
    //	if(!eq(name,"MC_Zwieback")&&!eq(name,"Stefanie_Zweig")&&!eq(name,"Ca�o_Central")&&!eq(name,"Beitbridge"))
    //		dissectParent(getAbstract(name));
	return n;
}
int countRows(char* line) {
	int len=(int)strlen(line);
	int row=0;
	for (int var=0; var < len; ++var)
		if (line[var] == '\n') line[var]=' ';	// wtf!?
		else if (line[var] == '\t') row++;
	return row + 1;
}

bool importYago(const char* file) {
	p("import YAGO start");
    //	if (!contains(file, "/")) file=(((string) "/data/base/BIG/yago/") + file).data();
	if (!contains(file, "/")) file=concat("yago/", file);
	//		file = concat("/Users/me/data/base/BIG/yago/", file);
	Node* subject;
	Node* predicate;
	Node* object;
	int linecount=0;
	FILE *infile=open_file(file);
	char* line=(char*) malloc(10000);
	char* id=(char*) malloc(10000);
	char* subjectName=(char*) malloc(10000);
	char* predicateName=(char*) malloc(10000);
	char* objectName=(char*) malloc(10000);
	int rows=0;
	int leadingId=!contains(file, "Data");
	while (fgets(line, 10000, infile) != NULL) {
		fixNewline(line);
		if (line[0] == '\t') line[0]=' '; // don't line++ , else nothing left!!!
		if (linecount % 10000 == 0) {
            //			printf("MEMORY safety boarder reached\n");
			if (checkLowMemory()) //MEMORY
				return 0; //exit(0);// 4GB max
		}
		if (++linecount % 10000 == 0){
            printf("%d %s    \r", linecount, file);
            fflush(stdout);
        }
		int ok=rows=countRows(line);
		if (rows == 4 && leadingId) ok=sscanf(line, "%s\t%s\t%s\t%s", id, subjectName, predicateName, objectName);
		else if (rows == 4 && !leadingId) ok=sscanf(line, "%s\t%s\t%s\t%s", subjectName, predicateName, objectName, id); // data
		else ok=sscanf(line, "%s\t%s\t%s", subjectName, predicateName, objectName /*, &certainty*/);
		if (ok < 3) {
			printf("MISMATCH %s\n", line);
			continue; //todo
			char** all=splitStringC(line, '\t');
			if (all[2] == 0) {
				if (debug) printf("ERROR %s\n", line);
				continue;
			}
			subject=getYagoConcept(all[1]); //
			predicate=getYagoConcept(all[2]);
			object=getYagoConcept(all[3]);
			free(all);
		} else {
			if (eq(predicateName, "<hasGeonamesEntityId>")) continue;
            //			if(eq("<Tommaso_Caudera>",subjectName))
            //							p(subjectName);// subject==0 bug Disappears when debugging!!!
			subject=getYagoConcept(subjectName); //
			predicate=getYagoConcept(predicateName);
			object=getYagoConcept(objectName);
            //			if (subject == 0) subject=getYagoConcept(modifyConstChar(subjectName)); // wth ???
            //			if (object == 0) object=getYagoConcept(objectName); // wth ???
		}
		if (subject == 0 || predicate == 0 || object == 0) {
            //			if (debug) printf("ERROR %s\n", line);
            //			subject==0 bug Disappears when debugging!!!
            //			subject = getYagoConcept(subjectName); //
            //			object = getYagoConcept(objectName);
			badCount++;
			pf(">%d<", linecount);
			continue;
		}
		Statement* s;
		//		if (contains(objectName, subjectName, true))
		//			s = addStatement(subject, Member, object, false); // todo: id
		//		else
		s=addStatement(subject, predicate, object, false); // todo: id
        
		if (checkLowMemory()) {
			printf("Quitting import : id > maxNodes\n");
			break;
		}
		//		showStatement(s);
	}
	fclose(infile); /* Close the file */
	p("import facts ok");
	return true;
}

const char *fixFreebaseName(char *key) {
	key=(char *) fixYagoName(key);
	int len=(int)strlen(key);
	for (int i=len - 1; i > 0; --i)
		if (key[i] == '.') {
			key[i]=0;
			if (!eq(&key[i + 1], "topic")) return &key[i + 1];
		} else if (key[i] == '#') return &key[i + 1];
	return key;
}
long freebaseHash(cchar* x) {
	long hash=0;
	if (x[0] == '1') hash=1;
	if (x[0] == '2') hash=2;
	x++;
	char c;
	while ((c=*x++)) {
		if (c == '>') break;
		int n=normChar(c);
		if (c == '_') n=36;
		hash=hash * 64 + n;
	}
	return hash;
}
//map<string, Node*> freebaseKeys;
//map<const char*, Node*> freebaseKeys;
//static map<int, Node*> freebaseKeys=new std::map();
//map<long, Node*> freebaseKeys;
//
#ifdef use_boost
#include "shared_map.hpp"
shared_map freebaseKeys;
#else
map<long, int> freebaseKeys;
#endif

int freebaseKeysConflicts=0;


void testPrecious2() {
	long testE=freebaseHash("023gm0>");
    check(477389594==testE);
	int testI=freebaseKeys[testE];
    p(testI);
	Node* testN=get(testI);
    p(testN);
    check(startsWith( testN->name,"Christen"));
}



void testPrecious() {
	long testE=freebaseHash("01000m1>");
	Node* testA=getAbstract("Most Precious Days");
    //	Node* testA=getThe("Most Precious Days");
	int testI=freebaseKeys[testE];
	Node* testN=get(testI);
	p(testA);
	p(testN);
	check(testN == testA);
	check(freebaseKeys[testE] == testA->id);
}

bool importFreebaseLabels() { //  (Node**)malloc(1*billion*sizeof(Node*));
	char line[10000];
	char* label0=(char*) malloc(10000);
	char* label;
	char* key=(char*) malloc(10000);
	char* test=(char*) malloc(10000);
	FILE *infile=open_file("freebase.labels.en.txt");
    
	int linecount=0;
	while (fgets(line, sizeof(line), infile) != NULL) {
        //		if (linecount > 10000000) break;
#ifdef __APPLE__
        //		if (linecount > 1000000) break;
#endif
        //		if (linecount % 100 == 0 && linecount>20000)
        //			p(linecount);
		if (++linecount % 10000 == 0) {
			printf("%d freebase labels, %d duplicates     \r", linecount, freebaseKeysConflicts);
            fflush(stdout);
			if (checkLowMemory()) break;
            //			testPrecious();
		}
		sscanf(line, "%s\t%s\t%[^\t]s\t.", key, test, label0);
		label=label0;
		if (!startsWith(key, "<m.") && !startsWith(key, "<g.")) continue;
		if (!startsWith(test, "<#label")) continue;
        //        if (startsWith(label, "http"))continue;
		if (startsWith(label, "\"")) label++;
		int len=(int)strlen(label);
		if (len < 6) continue;
        if(len > 50){
            int spaces=0;
            for(int i=0;i<len;i++){
                if(label[i]==' ')spaces++;
                if(spaces==6||label[i]=='('||label[i]==':'){label[i]=label[i+1]=label[i+2]='.';label[i+3]=0;break;}
            }
            //            p(label);
        }
		label[len - 4]=0;		// "@en
		key[strlen(key) - 1]=0;
        //		try {
        //			uint h=freebaseHash(key + 3);		// skip <m. but // LEAVE THE >
        long h=freebaseHash(key + 3);		// skip <m. but // LEAVE THE >
        //			if (h < 0 || h >= 1 * billion)
        //				pf("WRONG KEY! %d %s %s", h, key, label);
        //			if(eq(key,"<m.0101rlg>"))
        //				p("K");
        
        h=freebaseHash(key + 3);
        if (h==477389594||h==28394677018){//023gm0
            p("x");
        }
        
        if (freebaseKeys[h] != 0) {
            //       freebaseKeysConflicts:2305228 not worth It
            //				if(!eq(get(freebaseKeys[h])->name, label,false))
            //					printf("freebaseKeys[h] already USED!! %s %d %s || %s\n", key + 3, h, label, get(freebaseKeys[h])->name);
            //				else
            //					printf("freebaseKeys[h] already reUSED!! %s %d %s || %s\n", key + 3, h, label, get(freebaseKeys[h])->name);
            freebaseKeysConflicts++;
        } else {
            Node* n;
            if (hasWord(label)) n=getNew(label);		//get(1);//
            else n=getAbstract(label);
            //		n->value.text=...
            if (n) {
//                freebaseKeys.insert(pair<long,int>(h,n->id));
                freebaseKeys[h]=n->id;					// idea: singleton id's !!! 1mio+hash!
            }
        }
        
        if (h==477389594||h==28394677018){//023gm0
            p("x");
            testPrecious2();
        }
    }
    
    //		} catch (...) {
    //            perror("XXX");
    //			p("XXXXXXXXXXXXXXXXXXXXXXXXX");
    //		}
    //		addStatement(getAbstract(goodKey), Instance, n,false);
    //		freebaseKeys[wordhash(goodKey)] = n;
    //		add(key,label);
    //	}
    fclose(infile); /* Close the file */
    p("freebase duplicates removed:");
    p(freebaseKeysConflicts);
    testPrecious();
    //	if (linecount > 40000000)
    //	check(freebaseKeys[freebaseHash("0c21rgr>")] != 0);
    p("import Freebase labels ok");
    return true;
}

Node *dissectFreebase(char* name) {
    if (!contains(name, ".")) {
        N a=getRelation((const char*)name);
        if(a)return a;
        a=getAbstract(name);
        if (a->lastStatement&&getStatement(a->lastStatement)->predicate==_instance)
            return getStatement(a->lastStatement)->Object();
        else
            return a;
    }
    // reuse freebaseHash for <organization.organization.parent> etc
    long h=freebaseHash(name);
    int got=freebaseKeys[h];
    if (got && get(got)->id != 0) return get(freebaseKeys[h]);
    const char* fixed=fixFreebaseName(name);
    N n=getThe(fixed);
    if(!n)return 0;// howtf "" ?
    freebaseKeys[h]=n->id;
//    freebaseKeys.insert(pair<long,int>(h,n->id));
    N o=dissectFreebase(name);
    addStatement(n, Domain, o, false);
    return n;
}

int MISSING=0;
Node* getFreebaseEntity(char* name) {
    if (name[0] == '<') name++;
    // skip <m. but  LEAVE THE >
    if (startsWith(name, "m.") || startsWith(name, "g.")) {
        long h=freebaseHash(name + 2);
        int got=freebaseKeys[h];
        if (got && get(got)->id != 0) return get(freebaseKeys[h]);
        else {
//            pf("MISSING %s\n", name);
            MISSING++;
            return 0;
//            m.0dpsmdq
//            return getThe(name);
        }
    }
    name=(char *) fixYagoName(name);
    if (contains(name, "^^")) return rdfValue(name);
    return dissectFreebase(name);
}

bool importFreebase() {
    //    return 0;
//    long x=   freebaseHash("0023gm0>");
    //    long x=   freebaseHash("023gm0>");
    //	freebaseKeys=freebaseKey_root;
    //	long x=freebaseHash("03f2bmf");
    //	long y=freebaseHash("03f27mf");
    //	check(x != y);
    //	if (!hasWord("01000m1>"))
    //	if (!freebaseKeys[freebaseHash("0c21rgr>")])		//1
    //		importFreebaseLabels();
    //    freebaseKeys.test();
//        testPrecious2();
    if (!freebaseKeys[freebaseHash("0zzxc3>")])		//1
        importFreebaseLabels();
    testPrecious2();
    //    if(hasWord("vote_value"))return true;
    pf("Current nodeCount: %d\n", currentContext()->nodeCount);
    Node* subject;
    Node* predicate;
    Node* object;
    char line[10000];
    char* objectName=(char*) malloc(10000);
    char* predicateName=(char*) malloc(10000);
    char* subjectName=(char*) malloc(10000);
    FILE *infile=open_file("freebase.data.txt");
    int linecount=0;
    while (fgets(line, sizeof(line), infile) != NULL) {
        //		if (linecount % 1000 == 0 && linecount > 140000) p(linecount);
        if (++linecount % 10000 == 0) {
            printf("\r%d freebase       ", linecount);
            fflush(stdout);
            if (checkLowMemory()) {
                printf("Quitting import : id > maxNodes\n");
                break;
            }
        }
        //		if(linecount<140000)continue;
        // printf(line);
        sscanf(line, "%s\t%s\t%s\t.", subjectName, predicateName, objectName);
        
        //		if(filterFreebase(subjectName, predicateName, objectName))continue;
        
        predicate=getFreebaseEntity(predicateName);
        
        subject=getFreebaseEntity(subjectName); //
        object=getFreebaseEntity(objectName);
        if (predicate == Instance) {
            predicate=Type;
            N t=subject;
            subject=object;
            object=t;
        }
        
        if (!subject || !predicate || !object) {
            printf("_");
            badCount++;
            //			if (debug) {
            //				p(linecount);
            //				p(line);
            //				p("ERROR");
            //			}
        } else {
            if (1147 == subject->id || 1147 == predicate->id){// || 1147 == object->id) {
                badCount++;
                p(line);
                subject=getFreebaseEntity(subjectName); //
                //				object=getFreebaseEntity(objectName);
            }
            else// Statement* s=
                addStatement(subject, predicate, object, false); // todo: id
        }
        //		showStatement(s);
    }
    fclose(infile); /* Close the file */
    p("import Freebase ok");

    pf("MISSING %d\n", MISSING);
    currentContext()->use_logic=false;
    //	freebaseKeys.clear();
    //	free(freebaseKeys);
    return true;
}

bool importFacts(const char* file, const char* predicateName="population") {
    p("import facts start");
    printf("WARNING: SETTING predicateName %s\n", predicateName);
    Node* subject;
    Node* predicate;
    Node* object;
    char line[1000];
    //    char* predicateName=(char*) malloc(100);
    predicate=getClass(predicateName);
    int linecount=0;
    FILE *infile=open_file(file);
    char* objectName=(char*) malloc(100);
    char* subjectName=(char*) malloc(100);
    while (fgets(line, sizeof(line), infile) != NULL) {
        /* Get each line from the infile */
        if (++linecount % 10000 == 0) {
            printf("%d %s    \r", linecount, file);
            fflush(stdout);
        }
        if (!eq(predicateName, "population")) sscanf(line, "%s\t%s", subjectName, objectName); // no contextId? cause this db assumes GLOBAL id!
        else sscanf(line, "%*d\t%s\t%s\t%*d", /*&id,*/subjectName, objectName /*, &certainty*/); // no contextId? cause this db assumes GLOBAL id!
        // printf(line);
        //	 printf("%d\t%d\t%d\n",subjectId,predicateId,objectId);
        //		if (contains(subjectName, "Begriffskl") || contains(subjectName, "Abkürzung") || contains(subjectName, ":") || contains(subjectName, "#"))
        //			continue;
        if (contains(objectName, "jpg") || contains(objectName, "gif") || contains(objectName, "svg") || contains(objectName, "#")
            || contains(objectName, ":")) continue;
        
        subject=getAbstract(subjectName); //
        object=getAbstract(objectName);
        //dissectWord(abstract);
        Statement* s;
        if (contains(objectName, subjectName, true)) s=addStatement(subject, Member, object, false); // todo: id
        else s=addStatement(subject, predicate, object, false); // todo: id
        if (checkLowMemory()) {
            printf("Quitting import : id > maxNodes\n");
            break;
        }
        showStatement(s);
    }
    fclose(infile); /* Close the file */
    p("import facts ok");
    return true;
}

void importNames() {
    addStatement(all(firstname), are, a(name));
    addStatement(all(firstname), Synonym, a(first_name));
    addStatement(all(male_firstname), have_the(gender), a(male));
    addStatement(all(male_firstname), are, a(firstname));
    addStatement(all(male_firstname), Owner, a(male));
    addStatement(all(female_firstname), have_the(gender), a(female));
    addStatement(all(female_firstname), are, a(firstname));
    addStatement(all(female_firstname), Owner, a(female));
    importList("FrauenVornamen.txt", "female_firstname");
    importList("MaennerVornamen.txt", "male_firstname");
}

void importAbstracts() {
    char line[1000];
    char name[1000];
    //	char* line=(char*) malloc(1000);
    //	char* name=(char*) malloc(1000);
    int linecount=0;
    int id;
    doDissectAbstracts=false;
    //	memset(abstracts, 0, abstractHashSize * 2);
    Context* c=getContext(wordnet);
    FILE *infile=open_file("wordnet/abstracts.tsv");
    while (fgets(line, sizeof(line), infile) != NULL) {
        if (++linecount % 10000 == 0){
            printf("importAbstracts %d               \r", linecount);
            fflush(stdout);
        }
        sscanf(line, "%d\t%[^\n]s", &id, name); // %[^\n]s == REST OF LINE!
        id=id + 10000; // KILL ERVERYTHING!!!
        //		for (int i = 0; i < strlen(name); i++)if(name[i]==' ')name[i]='_';
        //		printf("%s\n",line);
        if (hasWord(name)) continue;
        c->nodeCount=id; // hardcoded hack to sync ids!!!
        Node* a=getAbstract(name);
        //		a->context=wordnet;
        a->kind=abstractId;
    }
    fclose(infile); /* Close the file */
}

int synonyms=400000;

void importSenses() {
    char line[1000];
    char* name=(char*) malloc(1000);
    int linecount=0;
    int id, labelid, synsetid;
    FILE *infile=open_file("wordnet/senses.tsv");
    while (fgets(line, sizeof(line), infile) != NULL) {
        if (++linecount % 10000 == 0){ printf("importSenses %d    \r", linecount);fflush(stdout);}
        fixNewline(line);
        sscanf(line, "%d\t%d\t%d\t%*d\t%*d\t%*d\t%*d\t%s", &id, &labelid, &synsetid, /*senseid,sensenum,lexid,tags,*/
               name);
        //		if(id<1000)continue;// skip :(
        id=id + 10000; // NORM!!!
        //		if (130172 == id) p(line);
        Node* word=get(id);
        synsetid=norm_wordnet_id(synsetid);
        if (synsetid > 200000 + 117659) p(line);
        if (synsetid < 200000) continue;
        Node* sense=get(synsetid);
        for (int i=0; i < strlen(name); i++)
            if (name[i] == '%') name[i]=0;
        if (!sense->id) {
            initNode(sense, synsetid, name, 0, wordnet);
        } else if (!eq(sense->name, name)) {
            Node* syno=initNode(get(synonyms), synonyms, name, 0, wordnet);
            addStatement(syno, Synonym, sense);
            //			addStatement(word,Instance,syno);// Sense
            sense=syno;
            synonyms++;
        }
        addStatement(word, Instance, sense, false); // Sense
        //		if(!sense->name){
        //		sense->name=fixed;
        //		sense->context=wordnet;
        //		sense->id=synsetid;
        //		}
    }
    fclose(infile); /* Close the file */
}

void addLabel(Node *node, char* text) {
    int len=(int)strlen(text);
    Context* context=currentContext();
    int slot=context->currentNameSlot;
    char* label=context->nodeNames + slot;
    strcpy(label, text); // can be freed now!
    node->value.text=label;
    context->nodeNames[slot + len]=0;
    context->currentNameSlot=slot + len + 1;
    //		addStatement(n,Label,getAbstract(definition));
}

void importSynsets() {
    char line[1000];
    char definition[1000];
    int linecount=0;
    int id;
    char pos;
    FILE *infile=open_file("wordnet/synsets.tsv");
    while (fgets(line, sizeof(line), infile) != NULL) {
        if (++linecount % 1000 == 0){ printf("importSynsets %d    \r", linecount);fflush(stdout);}
        fixNewline(line);
        sscanf(line, "%d\t%c\t%*d\t%[^\n]s", &id, &pos, /*&lexdomain,*/
               definition);
        id=norm_wordnet_id(id);
        if (pos == 'n') addStatement4(wordnet, id, Type->id, noun); // get(id)->kind = noun; DEFAULT!!
        if (pos == 'v') addStatement4(wordnet, id, Type->id, verb); //get(id)->kind = verb;
        if (pos == 'a') addStatement4(wordnet, id, Type->id, adjective); //get(id)->kind = adjective;
        if (pos == 'r') addStatement4(wordnet, id, Type->id, adverb); //get(id)->kind = adverb;
        if (pos == 's') addStatement4(wordnet, id, Type->id, adjective);
        //			get(id)->kind = adjective; // satelite !?
        addLabel(get(id), definition);
    }
    fclose(infile); /* Close the file */
}

void importLables() {
    char line[1000];
    char definition[1000];
    int linecount=0;
    int id;
//    char pos;
    FILE *infile=open_file("wordnet/labels.tsv");
    while (fgets(line, sizeof(line), infile) != NULL) {
        if (++linecount % 10000 == 0){ printf("importLables %d    \r", linecount);fflush(stdout);}
        fixNewline(line);
        sscanf(line, "%*d\t%d\t%[^\n]s", &id, /*&lexdomain,*/definition);
        id=id + 10000; //  label on abstract !?!
        //		id=norm_wordnet_id(id);
        if (id >= 200000) {
            p(line);
            continue;
        }
        addLabel(get(id), definition);
        //		addStatement(get(id),Label,getAbstract(definition));
    }
    fclose(infile); /* Close the file */
}

void importLexlinks() {
    char line[1000];
    int linecount=0;
    int s, p, o;
    int ss, so;
    FILE *infile=open_file("wordnet/lexlinks.tsv");
    while (fgets(line, sizeof(line), infile) != NULL) {
        if (++linecount % 10000 == 0){ printf("importLexlinks %d    \r", linecount);fflush(stdout);}
        //			printf(line);
        fixNewline(line);
        sscanf(line, "%d\t%d\t%d\t%d\t%d", &ss, &s, &so, &o, &p);
        //		Statement* old = findStatement(subject, predicate, object, 0, 0, 0); //,true,true,true);
        //		if (old)return old; // showStatement(old)
        //
        s=s + 10000;
        o=o + 10000;
        if (p == SubClass->id) continue; // Redundant data!
        if (p == Instance->id) continue; // Redundant data!
        
        Statement* x;
        if (ss != so) x=addStatement4(wordnet, norm_wordnet_id(ss), p, norm_wordnet_id(so));
        if (debug && !x)
            pf("ERROR %s\n", line);
        //		if(s!=o)x=addStatement4(wordnet, s, p, o); not on abstracts! hmm, for antonym properties? nah!
        //		if(!x)printf("ERROR %s\n",line);
    }
    fclose(infile); /* Close the file */
}

void importStatements() {
    char line[1000];
    int linecount=0;
    int s, p, o;
    FILE *infile=open_file("wordnet/statements.tsv");
    while (fgets(line, sizeof(line), infile) != NULL) {
        if (++linecount % 1000 == 0){ printf("importStatements %d    \r", linecount);fflush(stdout);}
        fixNewline(line);
        sscanf(line, "%d\t%d\t%d", &s, &o, &p);
        //		Statement* old = findStatement(subject, predicate, object, 0, 0, 0); //,true,true,true);
        //		if (old)return old; // showStatement(old)
        //
        if (p == SubClass->id) continue; // Redundant data!
        if (p == Instance->id) continue; // Redundant data!
        addStatement4(wordnet, norm_wordnet_id(s), norm_wordnet_id(p), norm_wordnet_id(o));
    }
    fclose(infile); /* Close the file */
}

void importWordnet() {
    load_wordnet_synset_map();
    //	if(hasWord()) checkWordnet()
    importAbstracts(); // MESSES WITH ABSTRACTS!!
    importSenses();
    getContext(wordnet)->nodeCount=synonyms; //200000+117659;//WTH!
    importSynsets();
    importLables();
    importStatements();
    importLexlinks();
    addStatement(getThe("opposite"), SuperClass, Antonym); // Correct but doesn't work
    addStatement(Antonym, SuperClass, getThe("opposite")); // Correct but doesn't work
    addStatement(getThe("opposite"), Synonym, Antonym);
    addStatement(getAbstract("opposite"), Synonym, Antonym);
    addStatement(Antonym, Synonym, getThe("opposite"));
    addStatement(Antonym, Synonym, getAbstract("opposite"));
    currentContext()->use_logic=true;
}

void importWordnet2() {
    importNodes(); // FIRST! Hardlinked ids overwrite everything!!
    importStatements2();
}

void importGeoDB() {
    importCsv("cities1000.txt", getThe("city"), '\t', "alternatenames,modificationdate,geonameid",
              "latitude,longitude,population,elevation,countrycode", 2, "asciiname");
}

// IMPORTANT: needs manual collectAbstracts() afterwards (for speed reasons??)

void importAllYago() {
    //	Is
    import("yago", "yagoGeonamesData.tsv");
    load_wordnet_synset_map();
    import("yago", "yagoFacts.tsv");
    check(hasWord("Tom_Hartley"));
    import("yago", "yagoSimpleTypes.tsv");
    import("yago", "yagoLiteralFacts.tsv");
    import("yago", "yagoStatistics.tsv");
    import("yago", "yagoSchema.tsv");
    //	import("yago", "yagoGeonamesEntityIds.tsv");
    //	import("yago", "yagoGeonamesClassIds.tsv");
    import("yago", "yagoGeonamesClasses.tsv");
    import("yago", "yagoGeonamesGlosses.tsv");
    import("yago", "yagoSimpleTaxonomy.tsv");
    //import("yago","yagoWordnetIds.tsv");// hasSynsetId USELESS!!!
    //import("yago","yagoWordnetDomains.tsv");
    //import("yago","yagoMultilingualClassLabels.tsv");
    //	import("yago", "yagoTaxonomy.tsv");todo
    //import("yago","yagoDBpediaClasses.tsv");
    //import("yago","yagoDBpediaInstances.tsv");
    //import("yago","yagoMetaFacts.tsv");
    import("yago", "yagoImportantTypes.tsv");
    import("yago", "yagoLabels.tsv");
    //	dissectParent((Node *) -1);
    
    //	addStatement(Number,Synonym,getThe("xsd:decimal"));
    //	addStatement(Number,Synonym,getThe("xsd:integer"));
    //	addStatement(Date,Synonym,getThe("xsd:date"));
    //	addStatement(getThe("xsd:date"),SuperClass,Date);
    //	addStatement(getThe("xsd:decimal"),SuperClass,Number);
    //	addStatement(getThe("xsd:integer"),SuperClass,Number);
    //	addStatement(get("xsd:date"),SuperClass,Date);
    //	addStatement(get("xsd:decimal"),SuperClass,Number);
    //	addStatement(get("xsd:integer"),SuperClass,Number);
    addStatement(getAbstract("xsd:date"), SuperClass, Date);
    addStatement(getAbstract("xsd:decimal"), SuperClass, Number);
    addStatement(getAbstract("xsd:integer"), SuperClass, Number);
}

void importAll() {
    //	importFacts()
    //	importCsv("adressen.txt");
    importWordnet();
    //	doDissectAbstracts=true;// already? why not
    importNames();
    importGeoDB();
    importFreebase();
    importAllYago();
    //	if (getImage("alabama") != "" && getImage("Alabama") != "")
    //		p("image import done before ...");
    //	else
    //		importImages();
}

void importWikipedia() {
}

void import(const char* type, const char* filename) {
//    clock_t start;
//    double diff;
    //  start = clock();
    //  diff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
    if (filename == 0) filename=type;
    if (eq(type, "all")) {
        importAll();
    } else if (eq(type, "csv")) {
        importCsv(filename);
    } else if (eq(type, "wordnet")) {
        importWordnet();
    } else if (eq(type, "freebase")) {
        importFreebase();
    } else if (eq(type, "names")) {
        importNames();
    } else if (eq(type, "images")) {
        importImages();
    } else if (eq(type, "wiki")) {
        importWikipedia();
    } else if (eq(type, "topic")) {
        importWikipedia();
    } else if (eq(type, "yago")) {
        if (eq(filename, "yago")) importAllYago();
        //		else if (contains(filename, "fact"))
        //			importFacts(filename, filename);
        //		else
        importYago(filename);
    } else if (contains(filename, "txt")) {
        importCsv(filename);
    } else if (contains(filename, "csv")) {
        importCsv(filename);
    } else if (contains(filename, "tsv")) {
        importCsv(filename);
    } else if (contains(filename, "xml")) {
        importXml(filename);
    } else {
        //if (!importFacts(filename, filename))
        printf("Unsupported file type %s %s", type, filename);
        importAll();
    }
    //  cout<<"nanoseconds "<< diff <<'\n';
    
    // importSqlite(filename);
    //    importNodes();
    //    importStatements();
}
/*root@h1642655:~/netbase# l facts/
 actedIn           during              hasChild           hasISBN                hasRevenue       interestedIn    isSubstanceOf    subClassOf
 bornIn            establishedOnDate   hasCurrency        hasLabor               hasSuccessor     inTimeZone      livesIn          subPropertyOf
 bornOnDate        exports             hasDuration        hasMotto               hasTLD           inUnit          locatedIn        type
 created           familyNameOf        hasEconomicGrowth  hasNominalGDP          hasUnemployment  isAffiliatedTo  madeCoverFor     until
 createdOnDate     foundIn             hasExpenses        hasNumberOfPeople      hasUTCOffset     isCalled        means            using
 dealsWith         givenNameOf         hasExport          hasOfficialLanguage    hasValue         isCitizenOf     musicalRole      worksAt
 describes         graduatedFrom       hasGDPPPP          hasPages               hasWaterPart     isLeaderOf      originatesFrom   writtenInYear
 diedIn            happenedIn          hasGini            hasPopulation          hasWebsite       isMarriedTo     participatedIn   wrote
 diedOnDate        hasAcademicAdvisor  hasHDI             hasPopulationDensity   hasWeight        isMemberOf      politicianOf
 directed          hasArea             hasHeight          hasPoverty             hasWonPrize      isNativeNameOf  produced
 discovered        hasBudget           hasImdb            hasPredecessor         imports          isNumber        publishedOnDate
 discoveredOnDate  hasCallingCode      hasImport          hasProduct             influences       isOfGenre       range
 domain            hasCapital          hasInflation       hasProductionLanguage  inLanguage       isPartOf        since
 */
