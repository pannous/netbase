//#pragma once

#include <string.h> // strcmp ...
#include <cstdlib>
#include <string>
#include <map>
#include <unistd.h> //getcwd

#ifdef sqlite3
#include "sqlite3.h"


#endif

#include "utf8.hpp"
//#include "utf8.h"
#include "netbase.hpp"
#include "util.hpp"
#include "import.hpp"
#include "relations.hpp"// for wordnet
#include "reflection.hpp"
//#undef null
//#include "json.hpp"
//#define null 0

using namespace std;

cchar *statements_file = "statements.txt";
cchar *images_file = "images.txt";
cchar *images_file_de = "images_de.nt";

bool getSingletons = false;// i.e. Nationalmannschaft
bool getBest = false;// i.e. Madonna\Music | Madonna\Church
bool germanLabels = false;//true;
bool importing = false;

// Not on abstract because abstract values are 'The' entity pointers
void setText(Node *node, char *text) {
	int l = len(text);
	long slot = context->currentNameSlot;
	char *label = context->nodeNames + slot;
	strcpy(label, text); // can be freed now!
	node->value.text = label;
	context->nodeNames[slot + l] = 0;
	context->currentNameSlot = slot + l + 1;
	//		addStatement(n,Label,getAbstract(definition));
}


void normImageTitle(char *title) {// blue_fin => bluefin // what?
	int l = len(title);
	for (int i = l; i >= 0; --i) {
		if (title[i] == ' ' or title[i] == '_' or title[i] == '-') {
			strcpy(&title[i], &title[i + 1]); //,len-i);
		}
	}
}

bool checkLowMemory() {
	size_t currentSize = getCurrentRSS(); //
	size_t peakSize = getPeakRSS();
	size_t free = getFreeSystemMemory();
	//	if (!free) free=5.5L * GB;// 2 GB + work
	if (!free) free = 9.0L * GB; // 4 GB + work
	if (currentSize > free) {
		p("OUT OF MEMORY!");
		printf("MEMORY: %zX Peak: %zX FREE: %zX \n", currentSize, peakSize, free);
		return true;
	}
	//		 or currentSize*1.2>sizeOfSharedMemory or 
	if (context->nodeCount + 30000 > maxNodes) {
		p("OUT OF MEMORY!");
		pf("%d nodes!\n", context->nodeCount);
		return true;
	}
	if (context->lastNode + 30000 > maxNodes) {
		p("OUT OF MEMORY!");
		pf("%d nodes!\n", context->lastNode);
		return true;
	}
	if (context->currentNameSlot + 300000 > maxNodes * averageNameLength) {
		p("OUT OF MEMORY!");
		pf("%ld characters!\n", context->currentNameSlot);
		return true;
	}
	if (context->statementCount + 40000 > maxStatements) {
		p("OUT OF MEMORY!");
		pf("%d statements!\n", context->statementCount);
		if (importing)exit(0);
		return true;
	}
	if (extrahash + 20000 > abstracts + maxNodes * 2) {
		p("OUT OF MEMORY!");
		// 64 BIT : %x -> %016llX or %p for POINTER!
		pf("hashes near %p\n", extrahash);
		return true;
	}
	return false;
}

//int wordnet_synset_map[117659];
std::map<int, int> wordnet_synset_map;

// 100001740  -> -200000 and so on, no gaps
int norm_wordnet_id(int synsetid, bool force = false) {
	if (synsetid < 1000)return synsetid;// relations!
	if (synsetid < million and !force) return synsetid;
	int id = wordnet_synset_map[synsetid];
	if (!id and synsetid <= 10000000) id = wordnet_synset_map[synsetid + 10000000];
	if (!id and synsetid <= 20000000) id = wordnet_synset_map[synsetid + 100000000];
	//	if (!id and force)id=wordnet_synset_map[synsetid+200000000];
	//	if (!id and force)id=wordnet_synset_map[synsetid+300000000];
	if (!id) {
		p(synsetid);
		//		if(synsetid==12353431 or synsetid==112353431)
		//		p("BAD ID!!!");
	}
	id = id + synsetOffset;// NORM!!! *-1 with wikidata! war: 10000
	if (synsetOffset > propertySlots)
		throw "synsetOffset>propertySlots";
	return -id;
}

void load_wordnet_synset_map() {
	if (wordnet_synset_map.size() > 0) return;
	char line[1000];
	FILE *infile = open_file("wordnet/synset_map.txt");
	int s, id;
	while (fgets(line, sizeof(line), infile) != NULL) {
		fixNewline(line);
		sscanf(line, "%d\t%d", &s, &id);
		//		if(s==112353431 or id==266407)
		//			s=s;
		wordnet_synset_map[s] = id;
	}
}

map<long, string> nodeNameImages;
map<long, string> nodeNameImages2; // chopped
//map<long,string> nodeNameImages3;// chopped image name




void importWordnetImages(cchar *file) { // 18 MILLION!  // 18496249
	load_wordnet_synset_map();
	p("image import starting ...");
	int id;
	char line[10000];
	//	char label[1000];
	char lastTitle[10000];
	int linecount = 0;
	Node *wiki_image = getAbstract("wiki_image");
	addStatement(wiki_image, is_a, getThe("image"));
	char image[10000];
	char *title = (char *) malloc(10000);
	FILE *infile = open_file((char *) file);
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 10000 == 0) {
			if (checkLowMemory())break;
			pf("importImages %d  \r", linecount);
			fflush(stdout);
		};
		sscanf(line, "%s\t%*s\t%s\t%d", title,/*md5,*/image, &id);
		//    if(eq(title,"Uniform"))
		//      p(line);
		if (eq(lastTitle, title)) continue;
		strcpy(lastTitle, title);// only the first
		id = norm_wordnet_id(id);
		if (!id)continue;
		Node *subject = get(id);
		if (!subject and !hasWord(title)) {
			p(line);
			bad();
			continue; // currently only import matching words.
		}
		Node *object = getAbstract(image);
		if (subject and subject->id != 0)
			addStatement(subject, wiki_image, object, !CHECK_DUPLICATES);
		if (!subject or subject->id == 0 or !isAbstract(subject)) {
			if (hasWord(title))
				addStatement(getAbstract(title), wiki_image, object, !CHECK_DUPLICATES);
		}
	}
	free(title);
	fclose(infile);
}


void importImagesDE() {// dbpedia
	p("image import starting ...");
	char line[1000];
	char label[100];
	char lastTitle[10000];
	int linecount = 0;
	Node *wiki_image = getAbstract("wiki image");
	addStatement(wiki_image, is_a, getThe("image"));
	char image[1000];
	char *title = (char *) malloc(1000);
	FILE *infile = open_file((char *) images_file_de);
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 10000 == 0) {
			pf("importImages %d  \r", linecount);
			fflush(stdout);
		};
		//		sscanf(line, "<%[^>]s> <%[^>]s> <%[^>]s> .\n", title,label,image);
		sscanf(line, "<%[^>]s", title);
		sscanf(line + strlen(title) + 3, "<%[^>]s", label);
		sscanf(line + strlen(title) + 3 + strlen(label) + 3, "<%[^>]s", image);
		if (!eq(label, "depiction")) continue;
		if (eq(lastTitle, title)) continue;

		strcpy(lastTitle, title);
		//		if (!hasWord(title)) normImageTitle(title); //blue -_fin ==> bluefin
		if (!hasWord(title)) continue; // currently only import matching words.
		Node *subject = getAbstract(title);
		Node *object = getAbstract(image + 5);// ommit md5 part /9/9a/
		addStatement(subject, wiki_image, object, !CHECK_DUPLICATES);
	}
	free(title);
	fclose(infile);
}

void importImageTripels(const char *file) { // 18 MILLION!  // 18496249
	p("image import starting ...");
	char line[1000];
	char lastTitle[10000];
	int linecount = 0;
	Node *wiki_image = getAbstract("wiki image");
	addStatement(wiki_image, is_a, getThe("image"));
	char image[1000];
	char *title = (char *) malloc(1000);
	int good = 0;
	//	int bad=0;
	FILE *infile = open_file((char *) file);
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 10000 == 0) {
			pf("importImages %d  \r", linecount);
			fflush(stdout);
		};
		sscanf(line, "%s %*s %s", title, image);
		if (eq(lastTitle, title)) continue;
		strcpy(lastTitle, title);
		//		if (!hasWord(title)) normImageTitle(title); //blue -_fin ==> bluefin
		//		if (!hasWord(title)) continue; // currently only import matching words.
		//      if(++bad%1000==0){ps("bad image (without matching word) #");pi(bad);}
		//		if (getImage(title) != "")
		//			continue; //already has one ; only one so far!
		Node *subject = getAbstract(title);
		Node *object = getAbstract(image); // getThe(image);;
		//		if(endswith(image,".ogg") or endswith(image,".mid") or endswith(image,".mp3"))
		addStatement(subject, wiki_image, object, !CHECK_DUPLICATES);
		printf("%s\n", title);
		//		if (!eq(title, downcase(title),false))
		//			addStatement(getAbstract(downcase(title)), wiki_image, object, false);
		if (++good % 10000 == 0) {
			ps("GOOD images:");
			p(good);
		}
	}
	free(title);
	fclose(infile);
}

void importImagesEN() { // 18 MILLION!  // 18496249
	importImageTripels(images_file);
}


void importImages() {
	context = getContext(current_context);
	importWordnetImages("images.wn.all.csv");// via name
	if (germanLabels)
		importImagesDE();// dbpedia
	else
		importImagesEN();
	//    importWordnetImages("images.wn.csv");// via ids
}

void importNodes() {
	p("node import starting ...");
	// char fname[40]="/Users/me/data/base/netbase.sql";
	// ('2282','Anacreon','N'),

	// char* fname="/Users/me/data/base/netbase/nodes.test";
	//2026896532	103	dry_out	Verb	\N	\N	11	103

	char line[100];

	char *name0 = (char *) malloc(1000);
	int linecount = 0;
	cchar *nodes_file = "nodes.txt";
	FILE *infile = open_file(nodes_file);
	while (fgets(line, sizeof(line), infile) != NULL) {
		char tokens[1000];
		/* Get each line from the infile */

		if (++linecount % 1000 == 0) {
			pf("importNodes %d  \r", linecount);
			fflush(stdout);
		}
		strcpy(tokens, line);
		char *name = name0;
		//    replaceChar(tokens,, <#char what#>, <#char with#>)
		//		int x=0; // replace ' ' with '_'
		//		while (tokens[x]) {
		//			if (tokens[x] == ' ') tokens[x]='_';
		//			x++;
		//		}
		// char name[1000];
		// But if you are actually writing C++, then using the new [] syntax is better:
		// double *factors = new double [q-2];
		// (Note that this requires using delete[] factors instead of free(factors))
		// char kind[20];
		char contextId_s[100];
		//		char deleted[1];
		//		char version[1];
		//		char wordKindName[100];
		//		int wordKind;
		int Id;
		int kind;
		int contextId = wordnet;
		//		int contextID;
		//2026896532	103	dry_out	Verb	\N	\N	11	103
		//	sscanf(tokens,"%d\t%s\t%s\t%s\t%*s\t%*s\t%*d\t%d",&Id,contextId_s,name,wordKindName,&kind);
		sscanf(tokens, "%d\t%s\t%s\t%*s\t%*s\t%*s\t%d\t%*d", &Id, contextId_s, name, &kind); // wordKind->kind !
		if (kind == 105) kind = _relation; //relation
		if (kind == 1) kind = _concept; //noun
		//    if(kind==1)continue;
		if (kind == 103) kind = noun; //noun
		if (kind == 10) kind = noun; //noun
		if (kind == 11) kind = verb; //verb
		if (kind == 12) kind = adjective; //adjective
		if (kind == 13) kind = adverb; //adverb
		//    contextId = atoi(contextId_s);
		//
		//    if (Id < 1000)contextId = wordnet; //wn
		//    if (contextId == 100 and Id > 1000)contextId = 100;
		Node *n;
		if (Id > 1000) n = add(name, kind, contextId);
		else n = add_force(contextId, Id, name, kind);
		if (n == 0) {
			p("out of memory");
			break;
		}
		wn_map[Id] = n->id;
		//		wn_map2[n->id]=Id;
	}
	free(name0);
	fclose(infile); /* Close the file */
	p("\nnode import ok\n");
}

void importStatements2() {

	char line[1000];
	int linecount = 0;

	FILE *infile = open_file(statements_file);
	while (fgets(line, sizeof(line), infile) != NULL) {
		/* Get each line from the infile */
		if (++linecount % 1000 == 0) {
			pf("importStatements %d  \r", linecount);
			fflush(stdout);
		}        // p(line);
		//	strcpy( tokens,line);
		//		int x = 0;
		//  while (tokens[x++])
		//    if (tokens[x]==' ')
		//		  tokens[x]='_';
		//		int contextId;
		int subjectId;
		int predicateId;
		int objectId;
		int id; // ignore now!!
		sscanf(line, "%d\t%d\t%d\t%d", &id, &subjectId, &predicateId,
		       &objectId); // no contextId? cause this db assumes GLOBAL id!
		subjectId = wn_map[subjectId]; //%1000000;//wn!!
		//		predicateId = predicateId; //%1000000;
		if (predicateId > 100) predicateId = wn_map[predicateId]; //%1000000;
		objectId = wn_map[objectId]; //%1000000;
		// printf("%d\t%d\t%d\n",subjectId,predicateId,objectId);
		if (subjectId < 1000 or objectId < 1000 or predicateId == 50 or predicateId == 0 or subjectId == 1043 or
		    subjectId == 1044)
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
	static string space = " ";
	for (int i = (int) s.length(); i > 1; i--) {
		char c = s[i];
		if (c > 65 and c < 91) s.replace(i, 1, space + (char) tolower(c));
		if (c == '(') s[i - 1] = 0; //cut _( )
		if (c == ',') s[i] = 0; //cut ,_
		if (c == ')') s[i] = 0;
	}
	//  s[0]=tolower(s[0]);
	s = replace_all(s, "_", " ");
	s = replace_all(s, " ", " ");
	return s;
};

// TODO :!test MEMLEAK!

const char *parseWikiTitle(char *item, int id = 0, int context = current_context) {
	//  string s="wordnet_raw_material_114596700";
	static string s;
	s = string(item); //"Uncanny_Tales_(Canadian_pulp_magazine)";
	//  South_Taft,_California
	// facts/hasPopulationDensity/ArticleExtractor.txt:205483101	Sapulpa,_Oklahoma	397.1#/km^2	0.9561877303748038

	//  string s="wordnet_yagoActorGeo_1";
	//  wordnet_yagoActorGeo_1
	//  if(s.find("wordnet"))contextId=wordnet;
	//if(s.find("wikicategory")) kind=list / category
	s = replace_all(s, "wikicategory_", "");
	s = replace_all(s, "wordnet_(.*)_(\\d*)", "$1"); // todo
	s = replace_all(s, "yago_", "");
	s = replace_all(s, "wikicategory", "");
	s = replace_all(s, "wordnet", "");
	s = replace_all(s, "yago", "");
	int last = (int) s.rfind("_");
	int type = (int) s.find("(");
	string clazz = deCamel(s.substr(type + 1, -1));
	string word = s;
	string Id = s.substr(last + 1, -1);
	//  int
	id = atoi(Id.c_str());
	if (id > 0) {
		word = s.substr(0, last);
	}
	//  word=deCamel(word);
	//  item=word.c_str();
	//  ~word;
	return word.c_str(); // TODO!! MEMLEAK!!
}

//void extractUnit() {
//	string s="397.1#/km^2";
//}

char *charToCharPointer(char c) {
	char *separator = (char *) malloc(sizeof(char) * 2);
	separator[0] = c;
	separator[1] = 0;
	//	char separator[2]= { c, 0 };
	return separator;
}

char guessSeparator(char *line) {
	const char *separators = ",\t;|";
	char the_separator = '\t';
	int max = 0;
	int size = len(separators);
	for (int i = 0; i < size; i++) {
		int nr = splitStringC(line, 0, (char) separators[i]);
		if (nr > max) {
			the_separator = separators[i];
			max = nr;
		}
	}
	return the_separator;
}

int getNameRow(char **tokens, int nameRowNr = -1, const char *nameRow = 0) {
	int row = 0;
	if (nameRowNr >= 0)return nameRowNr;
	while (true) {
		char *token = tokens[row];
		if (!token) break;
		//		if (nameRowNr < 0) {
		if (nameRow == 0) {
			if (eq("name", token)) nameRowNr = row;
			if (contains(token, "name", true) and nameRowNr < 0) nameRowNr = row; // first come!
			if (eq("title", token)) nameRowNr = row;
		} else if (eq(nameRow, token)) nameRowNr = row;
		//		}
		row++;
	}
	if (nameRowNr < 0) return 0;
	return nameRowNr;
}

// BROKEN!!!??!!
// fields will brake once line is freed!
int getFields(char *line, vector<char *> &fields, char separator, int nameRowNr, const char *nameRow) {
	char *token;
	cchar *separators = ",;\t|";
	if (separator) separators = charToCharPointer(separator);
	//	line=editable(line);
	token = strtok(line, separators);// token will brake once line is freed!
	int row = 0;
	while (token != NULL) {
		fields.push_back(token);
		token = strtok(NULL, separators); // BROKEN !!??!!??!!??
		row++;
	}
	for (int i = 0; i < row; i++) {
		token = fields.at(i);
		if (nameRowNr < 0) {
			if (nameRow == 0) {
				if (eq("name", token)) nameRowNr = row;
				if (eq("title", token)) nameRowNr = row;
			} else if (eq(nameRow, token)) nameRowNr = row;
		}
	}
	//	if (separator)free(&separators); fuck it
	return nameRowNr;
}

void fixNewline(char *line, bool limit/*0=none*/) {
	int l = len(line) - 1;
	if (limit and l >= limit)
		line[limit - 1] = 0;
	else
		while (l >= 0) {
			if (line[l] == '\n') line[l--] = 0;
			else if (line[l] == '\r') line[l--] = 0;
			else if (line[l] == '\t') line[l--] = 0;
			else if (line[l] == '"') line[l--] = 0;
			else if (line[l] == ' ') line[l--] = 0;
			else break;
		}
}

char *extractTagName(const char *line) {
	return match(editable(line), "<([^>]+)>");
}

Node *extractType(const char *file) {
	char *typeName = keep_to(cut_to(cut_to(editable(file), "/"), "/"), ".");
	Node *type = getThe(typeName);
	return type;
}

char *extractTagValue(char *line) {
	return match(line, ">([^<]+)<");
}

bool isNameField(char *field, char *nameField) {
	if (nameField and !eq(field, nameField)) return false;
	if (eq(field, nameField)) return true;
	if (eq(field, "name")) return true;
	if (eq(field, "title")) return true;
	if (eq(field, "label")) return true;
	return false;
}

Node *namify(Node *node, char *name) {
	node->name = name_root + context->currentNameSlot;
	strcpy(node->name, name); // can be freed now!
	int l = len(name);
	context->nodeNames[context->currentNameSlot + l] = 0;
	addStatement(getAbstract(name), Instance, node, !CHECK_DUPLICATES);
	// replaceNode(subject,object);
	return node;
}

void addAttributes(Node *subject, char *line) {
	line = replaceChar(line, '"', '\''); // why?
	do {
		char *attribute = match(line, " ([^=]+)='[^']'");
		char *value = match(line, " [^=]+='([^'])'");
		if (!attribute) break;
		Node *predicate = getThe(attribute);
		Node *object = getThe(value);
		addStatement(subject, predicate, object, false);
	} while (attribute != 0);
}

bool hasAttribute(char *line) {
	return match(line, " ([^=]+)=");
}

//Context* context,
//Node* type,
// importXml("/Users/me/data/base/geo/geolocations/Orte_und_GeopIps_mit_PLZ.xml","city","ort");

void importXml(const char *file, char *nameField, const char *ignoredFields, const char *includedFields) {
	p("\nimport XML start");
	//	bool dissect=false;
	char line[1000];
	//	char* line0=(char*) malloc(sizeof(char) * 100);
	//	char* field=(char*) malloc(sizeof(char) * 100);
	char *field;
	char *value0 = (char *) malloc(sizeof(char) * 10000000); // uiuiui!
	char *value;
	Node *root = 0;
	Node *parent = 0; // keep track of 1 layer
	Node *subject = 0;
	Node *predicate = 0;
	Node *object = 0;
	vector<Node *> predicates = *new vector<Node *>();
	map<char *, char *> fields;
	queue<Node *> parents; //constructed!!

	//	char* objectName = (char*) malloc(100);
	//	int depth = 0;
	//	vector<char*> ignoreFields = splitString(ignoredFields, ",");
	//	vector<char*>& includeFields = splitString(includedFields, ",");
	int linecount = 0;
	FILE *infile = open_file(file);
	//	Node* UNKNOWN_OR_EMPTY=getThe("<unknown/>");
	//	map<Node*,Node*> fields;
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 1000 == 0) {
			pf("importXml %d  \r", linecount);
			fflush(stdout);
		}
		fixNewline(line);
		//		line0=line; // readable in Debugger!
		if (contains(line, "<?xml")) continue;
		if (!subject) {
			if (root) subject = add(extractTagName(line));
			else root = add(extractTagName(line));
			continue;
		}

		if (startsWith(line, "</")) {
			//			for(Node* predicate:fields){
			//				object = getThe(extractTagValue(line),null, dissect);
			//				Statement *s = addStatement(subject, predicate, object, false);
			//				showStatement(s);
			//			}
			//			if(!parents.empty())
			subject = 0; // parents.back(); // back????
			//			parents.pop();
			fields.clear();
			continue;
		}
		// <address> <city> <zip>12345</zip> <name>bukarest</name> </city> </address>
		if (match(line, "<[a-z]") and contains(line, "</")) {
			field = extractTagName(line);
			value = extractTagValue(line);
			if (ignoredFields and contains(ignoredFields, field)) continue;
			if (isNameField(field, nameField)) {
				//				parent=parents.front();
				// rewrite address.member=city
				//				deleteStatement(findStatement(parent, Member, subject, 0, 0, 0, 0));
				// address.CITY=bukarest
				predicate = getThe(subject->name); // CITY
				Node *object = namify(subject, value); // city.name=bukarest
				addStatement(parent, predicate, object, !CHECK_DUPLICATES); // address.CITY=bukarest
				subject = object; // bukarest
				//				show(subject,true);
				continue;
			}

			if (!value) {                //<address> <city> ...
				//				parents.push(subject);
				parent = subject;

				//			if(eq(field,"zip")){
				//				value=value;
				//				showStatement(s);
				//			}

				if (!contains(line, "><")) {                // else empty!
					object = add(field);
					addStatement(subject, Member, object, !CHECK_DUPLICATES);
					subject = object;
				} else {
					object = getThe(field);
					//					addStatement(subject, Unknown,object,!CHECK_DUPLICATES);//EMPTY
					addStatement(subject, object, Unknown, !CHECK_DUPLICATES); //EMPTY
					//					addStatement(subject, object,UNKNOWN_OR_EMPTY,!CHECK_DUPLICATES);//EMPTY
				}
				addAttributes(subject, line);
				continue;
			}
			if (hasAttribute(line)) {
				predicate = add(field); // <zip color='green'>12345</zip>
				addAttributes(predicate, line);
			} else {
				predicate = getThe(field, NO_TYPE);
			}
			object = getThe(value, NO_TYPE);
			//			Statement* s=
			addStatement(subject, predicate, object, !CHECK_DUPLICATES);

			//			fields.insert(predicate,object);//
			continue;
		}
		if (startsWith(line, "<") and !contains(line, "</")) {
			parent = subject;
			//			parents.push(subject); // <address> <city> ...
			field = extractTagName(line);
			subject = add(field); // can be replaced by name!
			addStatement(parent, Member, subject, !CHECK_DUPLICATES); // address.city
			addAttributes(subject, line);
			continue;
		}
	}
	free(value0);
	//	free(field);
	//	free(line0);
	fclose(infile); /* Close the file */
	p("import xml ok ... items imported:");
	p(linecount);
}

void fixValues(char **values, int size) {
	for (int i = 0; i < size; i++) {
		char *x = values[i];
		if (x[0] == '"') x++;
		int l = len(x);
		if (x[l - 1] == '"') x[l - 1] = 0;
		values[i] = x;
	}
}

N addSubword(char *name, N kind) {
	N old = hasWord(name);
	if (old) {
		if (old->statementCount < 3)
			addStatement(old, Type, kind);
		return old;
	}
//	if(!old){
	if (eq(name, "Kiel "))
		p("Kiel ");
	N n = getSingleton(name, kind, false);
//		addStatement(n, Type, kind);
	n->kind = kind->id;// DANGER!
//		return n;
//	}
	return 0;
}

N addSubword(char *name, int words, N kind) {
	if (wordCount(name) < words)
		return 0;
	int l = len(name);
	int i = 0;
	N found = addSubword(name, kind);// ok, only if new
	if (wordCount(name) == words)
		return found;
	while (l > 0 and i < l and i >= 0) {
		i++;
		if (name[i] == ' ') {
			words--;
			if (words == 0) {
				name[i] = 0;// cut
				N label = addSubword(name, kind);
//				N label=addSubword(name,Internal);
				name[i] = ' ';// restore
//				addStatement(found, Label, label);
//				return found;
				return label;
			}
		}
	}
	return found;
}

int toll = 0;

N addSubCategories(char *name, N kind) {
//	char* d=editable(name);
	int i = len(name);
	while (i >= 0) {
		i--;
		if (name[i] == '/') {
			name[i] = 0;// cut
//			char *kat=name+i+1;
			N label = addSubword(name, kind);
//			if(label)break;// known
			if (!label) {
				toll++;
				if (toll % 1000 == 0)printf("\ntoll :%d\n", toll);
			}
		}
	}
	addSubword(name, kind);
	return 0;
}

string str(char *c) {
	return string(c);
}

int total_bad = 0;
bool doSplitValues = true;

void splitValues(N subject, N predicate, char *values) {
	std::vector<string> &value_list = splitString(values, "|");
	for (std::vector<string>::iterator it = value_list.begin(); it != value_list.end(); ++it) {
		N object = getThe(*it);
		addStatement(subject, predicate, object, !CHECK_DUPLICATES);
	}
//	for(string s : splitString(values,"|")){// c++11 not on all systems yet:(
//		N object=getThe(s);
//		addStatement(subject, predicate,object, !CHECK_DUPLICATES);
//	}
}

void importCsv(const char *file, Node *type, char separator, const char *ignoredFields, const char *includedFields,
               int nameRowNr, const char *nameRow) {
	p("\nimport csv start");
	bool tmp_autoIds = autoIds;
	autoIds = false;
	total_bad += badCount;
	badCount = 0;
	context = getContext(current_context);
	char line[MAX_CHARS_PER_LINE * 2];
	//	char* line=(char*)malloc(1000);// DOESNT WORK WHY !?! not on stack but why?
	char **values = (char **) malloc(sizeof(char *) * MAX_ROWS);
	char lastValue[MAX_CHARS_PER_LINE * 2];
	char *line0 = 0;// nullptr;
	map<char *, Node *> valueCache;
	Node *subject = 0;
	Node *predicate = 0;
	Node *object = 0;
	int linecount = 0;
	if (!type) {
		char *typeName = keep_to(editable(cut_to(cut_to(file, "/"), "/")), ".");
		type = getThe(typeName);
		getBest = true;
	}
	vector<Node *> predicates = *new vector<Node *>();
	vector<string> &ignoreFields = splitString(ignoredFields, ",");
	vector<string> &includeFields = splitString(includedFields, ",");

	bool cut_amazon = contains(file, "amazon");
	bool cut_billiger = contains(file, "billiger");
	N Marke = getThe("amazon Marke");
	N Author = getThe("amazon Author");
	N Kunstler = getThe("amazon Künstler");
	N TopKategorie = getThe("amazon TopKategorie");
	N AKategorie = getThe("amazon Kategorie");
	if (cut_billiger)Marke = getThe("billiger.de Marke");
	int fieldCount = 0;
	int size = 0;// per row ~ Hopefully equal to fieldCount
	while (readFile(file, &line[0])) {
		fixNewline(line, false);
//		free(line0); LEAK, why??: object was probably modified after being freed HOW???
		line0 = editable(line);
		if (!separator) {
			separator = guessSeparator(editable(line)); // would kill fieldCount
		}
		size = splitStringC(line0, values, separator);
		if (linecount == 0 && size > 1) {
			fieldCount = size;
			nameRowNr = getNameRow(values, nameRowNr, nameRow);
			fixValues(values, fieldCount);
			for (int i = 0; i < fieldCount; i++) {
				char *field = values[i];
				Node *fielt = getThe(field); // Firma		instance		Terror_Firma LOL
//				dissectWord(fielt);
				predicates.push_back(fielt);
			}
			++linecount;
			continue;
		}
		if (++linecount % 1000 == 0) {
			pf("importCsv %s stats: %d good, %d bad \r", file, linecount, badCount);
			fflush(stdout);
		}
//		if(contains(line, "DEBUG"))
//		    ps(line);
		if (fieldCount != size and fieldCount != size + 1 and fieldCount + 1 != size) {
			bad();
//			p(line);
//			if(debug) printf("Warning: fieldCount!=columns (%d!=%d) in line %d %s \n", fieldCount, size,linecount - 1, file);
			continue;
		}
		fixValues(values, size);
		char *name = values[nameRowNr];
		if (!name or len(name) == 0) {
			bad();
			continue;
		}
		if (cut_amazon or cut_billiger) {
			replaceChar(name, ',', 0);// cut!
			replaceChar(name, '(', 0);// cut!
			replaceChar(name, '[', 0);// cut!
			fixNewline(name);
			if (name[0] == 0)continue;//!
			// Tennis", "id":31409028,"topic":"Amazon music product BAD
			if (cut_billiger and !contains(name, " "))subject = addSubword(name, type);
			else {
				int l = len(name);
				int i = 0, w = 0;
				while (i++ < l)
					if (name[i] == ' ')// cut after 4th word
						if (++w == 5) {
							name[i] = 0;
							break;
						}
				if (cut_billiger) {
					addSubword(name, 1, type);
					addSubword(name, 2, type);
				}
				addSubword(name, 3, type);
				addSubword(name, 4, type);
				addSubword(name, 5, type);
			}

			if (cut_amazon) {
				addSubword(values[3], Marke);// brand
				addSubword(values[4], Author);// Author
				addSubword(values[5], Kunstler);// artist
				addSubword(values[10], TopKategorie);// topcategory
				addSubCategories(values[22], AKategorie);
			}
			if (cut_billiger) {
				N marke = getThe(values[1], Marke);
				getThe(values[3], getThe("billiger.de Kategorie"));
				if (subject)addStatement(subject, Marke, marke);
//				if(checkNode(m)){// and !contains(name, " ")
//					string full=string(m->name)+" "+name;
//					N f=getThe(full.data(),getThe("billiger.de Produkt"));
//				}
			}
			//			if(cut_amazon)
//						if(cut_billiger)
			continue;
			// test1: NO PROPERTIES!
			// ESPECIALLY NO productdescription!!!
		}

		if (getSingletons)
			subject = getSingleton(name, type);
		else if (getBest)
			subject = getThe(name, type);
		else if (eq(name, lastValue))// danger if both are bad?
			subject = subject; //keep subject
		else if (name != lastValue)
			subject = getThe(name, type);
//			subject=getNew(name, type);
		if (subject == null)
			subject = getThe(name);

		strcpy(lastValue, name);

		// conflict: Neustadt.lon=x,Neustadt.lat=y,Neustadt.lon=x2,Neustadt.lon=y2
		// 2 objects, 4 relations, 1 name
		if (!checkNode(subject)) {
			bad();
			continue;
		}
		//		if (debug and subject and type and subject->kind > 0 and subject->kind != type->id) {		// subject->kind == 0 ??? MEANING?
		//			pf("Found one with different type subject->kind != type->id %d,%d,%s,%s\n", subject->kind, type->id, subject->name, type->name);
		//      //			subject = getThe(values[nameRowNr], type, dissect); // todo : match more or less strictly? EG Hasloh
		//		}
		for (int i = 0; i < size; i++) {
			if (i == nameRowNr)continue;
			predicate = predicates[i];
			if (predicate == null or !checkNode(predicate)) continue;
			if (contains(ignoreFields, predicate->name)) continue;
			if (includedFields != null and !contains(includeFields, predicate->name)) continue;
			char *vali = values[i];
			if (!vali or strlen(vali) == 0) continue; //HOW *vali<100??
			if (atoi(vali) != 0 and isInteger(vali))// eq(itoa(atoi(vali)),vali))
				object = value(vali, atoi(vali), Integer);
			else if (atof(vali) != 0 and isNumber(vali))// and eq(itoa(atof(vali)),vali)
				object = value(vali, atof(vali), Number);
			else {
				object = getThe(vali);
//				dissectWord(object);// nee, einmal für alle!
			}
			if (!object or object->id > maxNodes) {
				bad();
				if (debug) printf("ERROR %s\n", line);
				continue;
			}
			if (doSplitValues and contains(object->name, '|'))
				splitValues(subject, predicate, object->name);
			else addStatement(subject, predicate, object, !CHECK_DUPLICATES);
		}
		if (checkLowMemory()) {
			printf("Quitting import : id > maxNodes\n");
			exit(0);
			break;
		}
	}
	p("import csv ok ... lines imported:");
	p(linecount);
	autoIds = tmp_autoIds;
}

void importList(const char *file, const char *type) {
	p("import list start");
	char line[1000];
	Node *subject = getClass(type, 0);
	Node *object;
	int linecount = 0;
	FILE *infile = open_file(file);
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 10000 == 0) {
			pf("importList %d  \r", linecount);
			fflush(stdout);
		}
		if (linecount % 10000 == 0 and checkLowMemory()) break;
		fixNewline(line);
		object = getThe(line);
		if (!object or object->id > maxNodes) {
			bad();
			if (debug) printf("ERROR %s\n", line);
			continue;
		}
		addStatement(object, Type, subject,
		             false); // This should still be skipped If approached by subject!!! not object
		//		addStatement(subject, Instance, object, false);// This would not be detected with the instant gap
	}
	fclose(infile); /* Close the file */
	p("\nimport list ok");
}

char *cut_wordnet_id(char *key) {
	for (int i = len(key); i > 1; --i) {
		if (key[i] == '_') {
			char *id = key + i + 1;
			key[i] = 0;
			return id; // <wordnet_album_106591815> -> 06591815 DROP LEADING 1 !!!
		}
	}
	return 0;
}

char *cut_wiki_id(char *key) {
	int i = len(key);
	if (key[i - 1] == '>')key[i - 1] = 0;
	if (key[i - 1] == '"')key[i - 1] = 0;
	if (key[0] != '<')return ++key; // quoted
	key++;
	for (; i > 1; --i) {
		if (key[i] == '#' || key[i] == '/') {
			char *id = key + i + 1;
			key[i] = 0;
			return id; // <wordnet_album_106591815> -> 06591815 DROP LEADING 1 !!!
		}
	}
	return key;
}


bool hasCamel(char *key) {
	if (contains(key, "_")) return false;
	char last = key[0];
	for (int i = 0; i < strlen(key); i++) {
		char c = key[i];
		if (c > 64 and c < 91)
			if (last > 96 and last < 123) {
				p(i);
				return true;
			}
		last = c;
	}
	return false;
}

char *removeHead(char *key, cchar *bad) {
	if (startsWith(key, bad)) return key + strlen(bad);
	return key;
}

char *fixYagoName(char *key) {
	if (key[0] == '<') key++;
	int le = len(key);
	if (le == 0)return key;
	if (key[le - 1] == '>') key[le - 1] = 0;
	key = removeHead(key, "wikicategory_");
	key = removeHead(key, "geoclass_");
	key = removeHead(key, "wordnetDomain_");
	if (startsWith(key, "wordnet_")) {
		p("SHOULDNT BE REACHED");
		//		char* id=cut_wordnet_id(key);
		key = removeHead(key, "wordnet_");
	}
	//	if(hasCamel(key)) // NOO: McBain
	//		return deCamel(key).data();
	return key;
}

Node *rdfOwl(char *name) {
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
	if (eq(name, "xsd:nonNegativeInteger"))return Number;// getAbstract("natural number");
	if (eq(name, "xsd:boolean")) return getAbstract("boolean"); // !
	if (eq(name, "xsd:gYear")) return getAbstract("year"); // !
	if (eq(name, "owl:disjointWith")) return getAbstract("disjoint with"); // symmetric!
	//	if(eq(key,"xsd:string"))return Text;// redundant
	if (eq(name, "owl:FunctionalProperty")) return Label;
	if (!startsWith(name, "wiki") and contains(name, ":") and !startsWith(name, "http")) {
		name = strstr(name, ":");
		if (!name) return 0; // contains(name, ":") WTF????????
		name = name + 2;
		//		printf(" unknown key %s\n", name);
		//		return 0;
	}
	return getThe(name); //Unknown;
}


Node *parseWordnetKey(char *key) {
	char *id = cut_wordnet_id(key);
	int nid = norm_wordnet_id(atoi(id));
	if (!nid)return 0;
	Node *wn = get(nid);
	if (wn) return wn;
	else return initNode(wn, nid, removeHead(key, "<wordnet_"), 0, 0);
}

Node *getYagoConcept(char *key) {
	if (startsWith(key, "<wordnet_")) return parseWordnetKey(key);
	char *name = fixYagoName(key); // Normalized instead of using similar, key POINTER not touched
	if (contains(name, ":") and !startsWith(name, "http")) return rdfOwl(key);
	if (eq(name, "isPreferredMeaningOf")) return Label;
	if (eq(name, "#label")) return Label;
	if (eq(name, "hasGloss")) return Label;
	if (eq(name, "hasWordnetDomain")) return Domain;
	//	if(eq(key,"owl:FunctionalProperty"))return Transitive;
	if (contains(name, "^^")) return rdfValue(key);
	//	if (contains(name, ".jpg") or contains(name, ".gif") or contains(name, ".svg") or startsWith(name, "#") or startsWith(name, "<#")) {
	//		printf(" bad key %s\n", name);
	//		return 0;
	//	}
	Node *n;
	if (!hasWord(name)) n = getAbstract(name);
	else n = getThe(name); //fixYagoName(key));
	//	dissectWord(n);
	//	if(!eq(name,"MC_Zwieback") and !eq(name,"Stefanie_Zweig") and !eq(name,"Ca���o_Central") and !eq(name,"Beitbridge"))
	//		dissectParent(getAbstract(name));
	return n;
}

int countRows(char *line) {
	int l = len(line);
	int row = 0;
	for (int var = 0; var < l; ++var)
		if (line[var] == '\n') line[var] = ' ';    // wtf!?
		else if (line[var] == '\t') row++;
	return row + 1;
}

bool importYago(const char *file) {
	autoIds = false;
	pf("import YAGO %s start\n", file);
	if (!contains(file, "/")) file = concat("yago/", file);
	Node *subject;
	Node *predicate;
	Node *object;
	int linecount = 0;
	FILE *infile = open_file(file);
	char *line = (char *) malloc(10000);
	char *id = (char *) malloc(10000);
	char *subjectName = (char *) malloc(10000);
	char *predicateName = (char *) malloc(10000);
	char *objectName = (char *) malloc(10000);
	int rows = 0;
	int leadingId = !contains(file, "Data");
	bool labels = 0;
	while (fgets(line, 10000, infile) != NULL) {
		fixNewline(line);
		if (line[0] == '\t') line[0] = ' '; // don't line++ , else nothing left!!!
		if (linecount % 10000 == 0) {
			labels = endsWith(line, "@eng"); // or endsWith(line,"@en") or endsWith(line,"@de");
			if (checkLowMemory()) //MEMORY
				return 0; //exit(0);
		}
		if (++linecount % 10000 == 0) {
			printf("%d %s  \r", linecount, file);
			fflush(stdout);
		}
		int ok = rows = countRows(line);
		if (labels and line[0] == ' ')
			ok += sscanf(line + 1, " %s\t%s\t\"%[^\"]s", subjectName, predicateName, objectName);
		else if (labels) ok = sscanf(line, "%*s\t%s\t%s\t\"%[^\"]s", subjectName, predicateName, objectName);
		else if (rows == 4 and leadingId)
			ok = sscanf(line, "%s\t%s\t%s\t%s", id, subjectName, predicateName, objectName);
		else if (rows == 4 and !leadingId)
			ok = sscanf(line, "%s\t%s\t%s\t%s", subjectName, predicateName, objectName, id); // data
		else ok = sscanf(line, "%s\t%s\t%s", subjectName, predicateName, objectName /*, &certainty*/);
		if (ok < 3) {
			printf("id %s subjectName %s predicateName %s objectName %s\n", id, subjectName, predicateName, objectName);
			printf("MISMATCH2 %s\n", line);
			if (/* DISABLES CODE */ (true))
				continue; //todo
			char **all = splitStringC(line, '\t');
			if (all[2] == 0) {
				if (debug) printf("ERROR %s\n", line);
				continue;
			}
			char *predicateName = all[2];
			if (startsWith(predicateName, "has")) {
				char next = predicateName[3];
				if (next == ' ') predicateName = predicateName + 4;
				if ('A' <= next and next <= 'Z') predicateName = predicateName + 3;
			}

			subject = getYagoConcept(all[1]); //
			predicate = getYagoConcept(predicateName);
			object = getYagoConcept(all[3]);
			free(all);
		} else {
			if (labels and wordhash(objectName) == wordhash(subjectName)) continue;
			if (eq(predicateName, "<hasGeonamesEntityId>")) continue;
			//			if(eq("<Tommaso_Caudera>",subjectName))
			//							p(subjectName);// subject==0 bug Disappears when debugging!!!

			object = getYagoConcept(objectName);
			if (object == Type) {
				bad();
				continue;      //!?
			}
			subject = getYagoConcept(subjectName); //
			predicate = getYagoConcept(predicateName);
			//			if (subject == 0) subject=getYagoConcept(editable(subjectName)); // wth ???
			//			if (object == 0) object=getYagoConcept(objectName); // wth ???
		}
		if (subject == 0 or predicate == 0 or object == 0) {
			//			if (debug) printf("ERROR %s\n", line);
			//			subject==0 bug Disappears when debugging!!!
			//			subject = getYagoConcept(subjectName); //
			//			object = getYagoConcept(objectName);
			bad();
			pf("|>%d<", linecount);
			continue;
		}
		Statement *s = 0;
		//		if (contains(objectName, subjectName, true))
		//			s = addStatement(subject, Member, object, false); // todo: id
		//		else
		s = addStatement(subject, predicate, object, !CHECK_DUPLICATES); // todo: id

		if (checkLowMemory()) {
			printf("Quitting import : id > maxNodes\n");
			exit(0);
			break;
		}
		if (!s)bad();
		//		showStatement(s);
	}
	free(id);
	free(subjectName);
	free(objectName);
	free(predicateName);
	fclose(infile); /* Close the file */
	pf("import %s ok\n", file);
	return true;
}

const char *fixFreebaseName(char *key) {
	key = (char *) fixYagoName(key);
	if (startsWith(key, "http")) return key;
	if (endsWith(key, ".svg")) return key;
	if (endsWith(key, ".png")) return key;
	if (endsWith(key, ".PNG")) return key;
	if (endsWith(key, ".jpg")) return key;
	if (endsWith(key, ".JPG")) return key;
	if (endsWith(key, ".gif")) return key;
	if (endsWith(key, ".bmp")) return key;

	int l = len(key);
	if (l > 5 and key[l - 4] == '.')return key;// .svg ... file endings
	for (int i = l - 1; i > 0; --i)
		if (key[i] == '.' and key[i - 1] > '9') {// no numbers!
			key[i] = 0;
			if (!eq(&key[i + 1], "topic")) return &key[i + 1];
		} else if (key[i] == '#') return &key[i + 1];
	return key;
}

long freebaseHash(cchar *x) {
	long hash = 0;
	if (x[0] == '1') hash = 1;
	if (x[0] == '2') hash = 2;
	x++;
	char c;
	while ((c = *x++)) {
		if (c == '>') break;
		int n = normChar(c);
		if (c == '_') n = 36;
		hash = hash * 64 + n;
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
//map<string, Node*> labels;
//map<int, Node*> labels;
static map<int, Node *> labels;//=new map<int, Node*>();
map<int, string> wn_labels;
int freebaseKeysConflicts = 0;

map<int, bool> wiki_abstracts;

bool importWikiLabels(cchar *file, bool properties = false, bool altLabels = false) {
	p("ONLY COMPATIBLE WITH PURE WIKIDATA");
	char line[MAX_CHARS_PER_LINE];// malloc(100000) GEHT NICHT PERIOD!
	char *label0 = (char *) malloc(10000);
	char *label;
	char *key0 = (char *) malloc(10000);
	char *key;
	char *test0 = (char *) malloc(10000);
	char *test;
	int linecount = 0;
	bool english = contains(file, ".en.");

	while (readFile(file, &line[0])) {
		if (++linecount % 10000 == 0) {
			printf("%d labels, %d bad\r", linecount, badCount);
			fflush(stdout);
		}
		if (line[0] == 0)
			continue;
//		if(contains(line, "Q3521>"))
//			p(line);
		//		if(debug)if(linecount>100)break;
		u8_unescape(line, MAX_CHARS_PER_LINE,
		            line); // utf8 unicode fix umlauts   not with new labels.csv!! removes 'ä' WTF why?
		//		if(line[0]=='#')continue;
		sscanf(line, "%s\t%s\t\"%[^\"]s", key0, test0, label0);
		fixNewline(line);
		key = key0;// Make modifiable
		label = label0;
		test = test0;
		if (strlen(label) == 0)label = test;
		if (eq(label, key))continue;
		test = dropUrl(test);
		if (key[0] == '<')key++;
		if (test[0] == '<')test++;

		bool isLabel = startsWith(test, "label");// or startsWith(test, "<#label"); or P1476:title
		if (!altLabels and startsWith(test, "altLabel"))continue;
		if (!isLabel and !startsWith(test, "altLabel") and !startsWith(test, "description")) continue;
		if (germanLabels and !english and !endsWith(line, "@de ."))continue;
		if (!(germanLabels or english) and !endsWith(line, "@en ."))continue;

		label = fixLabel(label);
		if (!label or label[0] == 0) {
			bad();
			continue;
		}
		key = dropUrl(key);
		int id = atoi(key + 1);
		if (properties or key[0] == 'P')// <P2900> "Faxnummer"@de .
			id = -propertyOffset - id;
		if (startsWith(test, "altLabel")) {
			if (!altLabels)continue;
			N oldLabel = getEntity(key);
			if (!oldLabel or oldLabel->id == 0 or oldLabel == Error) {
				bad();
				continue;
			}// NOT YET?
			N the_label = getAbstract(label);// NOT YET! getTHE??
			addStatement(oldLabel, Label, the_label);
			continue;//for now!
		}
		if (startsWith(test, "description")) {
			if (eq(label, "scientific article")) {
				N a = getEntity(key);
				a->kind = _ignore;
			}
			if (startsWith(label, "Wikimedia")) {
				N a = getEntity(key);
//				bool hadAbstract=isAbstract(a);
//				if(!hadAbstract){
				wiki_abstracts[id] = true;
//				insertAbstractHash(a,true);// remove old abstract
				insertAbstractHash(a, false);// keep old abstract
				a->kind = _abstract;// too early-> ignored! if(hasWord(key)==oldLabel)
//				}
				continue;
			}
			if (!altLabels)continue;
			if (english and germanLabels)continue;
			N oldLabel = getEntity(key);
			if (!oldLabel or oldLabel == Error)continue;
//			if(oldLabel->value.text){
//				p(oldLabel->value.text);
//			}
			if (oldLabel and checkNode(oldLabel)) {
				// _singleton vs _abstract->value conflict !?
				setText(oldLabel, label); //addStatement(oldLabel,Description,getAbstract(label));
				continue;
			}// else use as label i.e. "<Q9486626> <description> \"Wikimedia-Kategorie\"@de .\n"	WTF!
		}
		if (altLabels)continue;// important in second run only!

		//		if(properties)id=(int)maxNodes-id;
//		if(id==2)
//			p("ERde");

		if (id < maxNodes / 2) {
			Node *node = &context->nodes[id];
			if (english and germanLabels and node->name)
				continue;// Only set labels of entities that don't have a German translation
			N old = hasWord(label);
			if (!old) {
				context->nodeCount++;
				if (id < 0)
					initNode(node, id, label, _relation, 0);// _entity -> class later
				else if (wiki_abstracts[id])
					initNode(node, id, label, _abstract, 0);// _entity -> class later
				else
					initNode(node, id, label, _singleton, 0);// MAY LOOSE _singleton status!! _entity -> class later
				insertAbstractHash(node);// _singleton as abstract!? HACK, DANGER !
			} else {
				if (old == node)
					continue;// WHY: a->kind= _entity; no longer _singleton
				if (!node->name) {
					context->nodeCount++;
					initNode(node, id, label, _entity, 0);
					if (english and germanLabels)continue;
				}
				N ab;
				if (wiki_abstracts[id] or old->kind == _abstract) {
					ab = old;
					ab->kind = _abstract;
				} else {
					ab = add(label, _abstract);// moment, Maybe there will be a wiki disambiguation later!
					insertAbstractHash(ab, true);
					addStatement(ab, Instance, old);
					old->kind = _entity;// LOST _singleton status
					if (ab->value.node)
						p(ab->value.node);
					ab->value.node = 0;// no more singletons (for now)
				}
				addStatement(ab, Instance, node, false);
			}
		} else bad();// Not enough memory :(
	}
	free(key0);
	free(label0);
	free(test0);
	return true;
}

bool importLabels(cchar *file, bool useHash = false, bool overwrite = false, bool altLabel = false, bool checkDuplicates = true) {
	// TODO: RESTORE TO BEFORE! ~ 01.01.2016
	char line[MAX_CHARS_PER_LINE];// malloc(100000) GEHT NICHT PERIOD!
	char *label0 = (char *) malloc(10000);
	char *label;
	char *key0 = (char *) malloc(10000);
	char *key;
	char *test0 = (char *) malloc(10000);
	char *test;

	int linecount = 0;
	int rowCount = 3;
	//
	while (readFile(file, &line[0])) {
		//	while (fgets(line, sizeof(line), infile) != NULL) {
#ifdef __APPLE__
		if (linecount > 1000000) break;
#endif
		if (++linecount % 10000 == 0) {
			printf("%d labels, %d duplicates   \r", linecount, freebaseKeysConflicts);
			fflush(stdout);
			if (checkLowMemory()) {
				printf("Quitting import\n");
				exit(0);
			}
			//			if(!labels["Q1"]){printf("NO Q!");}
			//			if(!labels[1]){printf("NO Q!");}SIGSEV WHY??
			rowCount = countRows(line);
		}
		if (line[0] == '#')continue;
		//		memset(label0, 0, 10000);
		//		memset(key0, 0, 10000);
		//		memset(test, 0, 10000);
		//		label0[0]=0;// remove old!!
		//		if(contains(line, "Q2696789"))
		//			rowCount=rowCount;
		if (rowCount == 2) {
			sscanf(line, "%s\t%[^\n]s", key0, label0);
			test0 = label0;
		} else
			sscanf(line, "%s\t%s\t\"%[^\"]s", key0, test0, label0);
		fixNewline(line);
		key = key0;
		label = label0;
		test = test0;
		if (strlen(label) == 0)label = test;
		if (eq(label, key))continue;
		test = dropUrl(test);
		if (key[0] == '<')key++;
		if (test[0] == '<')test++;
		bool isLabel = startsWith(test, "label");// or startsWith(test, "<#label");
		if (!isLabel and !startsWith(test, "altLabel") and !startsWith(test, "description")) continue;
		//		if (startsWith(label, "\"")) label++;
		//		if (startsWith(key, "<")){ key++;key[strlen(key)-1]=0;}
		if (germanLabels and !endsWith(line, "@de ."))continue;
		if (!germanLabels and !endsWith(line, "@en ."))continue;
		//		if (!useHash and contains(label, "\\u")) continue;// no strange umlauts 'here'
		if (useHash) {
			if (!startsWith(key, "m.") and !startsWith(key, "g.")) continue;
			else key = key + 2;
		}
		//		if (startsWith(label, "http")) continue;
		//		if (startsWith(label, "/")) continue; // "/wikipedia/de_title"@en etc
		//		if (endsWith(label, "_id")) continue; // "/wikipedia/de_title"@en etc
		//		if (endsWith(label, "_title")) continue; // "/wikipedia/de_title"@en etc
		key = dropUrl(key);
		fixLabel(label);
		label = dropUrl(label);
		if (eq(key, "Q2696789") or eq(key, "P106"))
			p(line);
		int l = len(label);
		if (l > 50) {
			int spaces = 0;
			for (int i = 0; i < l; i++) {
				if (label[i] == ' ') spaces++;
				if (spaces == 6 or label[i] == '(' or label[i] == ':') {
					label[i] = label[i + 1] = label[i + 2] = '.';
					label[i + 3] = 0;
					break;
				}
			}
			label[100] = 0;
		}
		if (overwrite and hasWord(key)) {
			freebaseKeysConflicts++;
			N old = getAbstract(key);
			setLabel(old, label, false, true);
			continue;
		}
		if (!key) {
			printf("!KEY\n%s\n", line);
			continue;
		}

		long h = freebaseHash(key);        // skip m.
		int kei = atoi(key + 1);

		//		Node* oldLabel=labels[key];
		Node *oldLabel = labels[kei];
		if (eq(test, "description")) {
			if (oldLabel and checkNode(oldLabel)) {
				setText(oldLabel, label);
				//				addStatement(oldLabel,Description,getAbstract(label));
				continue;
			}// else use as label i.e. "<Q9486626> <description> \"Wikimedia-Kategorie\"@de .\n"	WTF!
			//			oldLabel->value.text=label;// copy!
		}
		if (oldLabel) {
			if (!altLabel)continue;
			if (eq(oldLabel->name, label))continue;// OK
			if (contains(label, "\\u"))continue; //Stra��enverkehr or Stra\u00DFenverkehr
			freebaseKeysConflicts++;
			printf("labels[key] duplicate! %s => %s or %s\n", key, oldLabel->name, label);
			addStatement(oldLabel, Label, getAbstract(label), !CHECK_DUPLICATES);
			continue;// don't overwrite german with english
			//Stra��enverkehr or Stra\u00DFenverkehr
			//			setLabel(oldLabel, label,false,false);
		}

		if (useHash and freebaseKeys[h] != 0) {
			//      printf("freebaseKeys[key] already reUSED!! %s => %s or %s\n", key , label, freebaseKeys[key]->name);
			//    freebaseKeysConflicts:2305228 not worth It
			//				if(!eq(get(freebaseKeys[h])->name, label,false))
			//					printf("freebaseKeys[h] already USED!! %s %d %s or %s\n", key + 3, h, label, get(freebaseKeys[h])->name);
			//				else
			//					printf("freebaseKeys[h] already reUSED!! %s %d %s or %s\n", key + 3, h, label, get(freebaseKeys[h])->name);
			freebaseKeysConflicts++;
			continue;
		}
		//    if(contains(line,"Arsenic"))
		//      p(line);

		Node *n;
		if (hasWord(label)) n = getNew(label);        //get(1);//
		else n = getAbstract(label);
		//		n->value.text=...
		addStatement(n, Labeled, getAbstract(key), !CHECK_DUPLICATES);// VERY EXPENSIVE ID !!!

		if (n) {
			//			if(keepLabel)addStatement(n, ID, key);
			if (useHash) freebaseKeys[h] = n->id;                    // idea: singleton id's !!! 1mio+hash!
				//        freebaseKeys.insert(pair<long,int>(h,n->id));
			else {
				//				labels[key]=n;

				if (key[0] == 'P') {
					labels[kei] = n;// overwrites labels!!!
					//					plabels[kei]=n;// overwrites labels!!!
					//					labels[concat(key,"c")]=n;// wikidata "P106c" compound property? hack
					//					labels[concat(key,"v")]=n;// wikidata "P106c" compound property? hack
				} else
					labels[kei] = n;
				//				if(contains((const char*)key,"_",false))
				//					labels[replaceChar(key,'_',' ')]=n;
			}
		}
	}
	//		add(key,label);
//	free(test0) free(label0) free(key0) fails, why? egal
	p("duplicates removed:");
	p(freebaseKeysConflicts);
//	testPrecious();
	pf("DONE importing %d labels from %s\n", linecount, file);
	closeFile(file);
	return true;
}

void importFreebaseLabels() {
	if (germanLabels)
		importLabels("freebase.labels.de.txt", true);
	importLabels("freebase.labels.en.txt", true);
}


bool useHash = false;

Node *dissectFreebase(char *name) {
	if (!contains(name, ".")) {
		//		if(endsWith(name," of"))// FLIP RELATION!
		N a = getRelation(name);
		if (a) return a;
		a = getAbstract(name);
		if (!a)return 0;//BUG HOW??? out of memory!
		if (a->lastStatement and getStatement(a->lastStatement)->predicate == _instance)
			return getStatement(a->lastStatement)->Object();// LAST INSTANCE == 'THE' one !?!? In import flow?
		else
			return a;
	}
	// reuse freebaseHash for <organization.organization.parent> etc
	long h;
	if (useHash) {
		h = freebaseHash(name);
		int got = freebaseKeys[h];
		if (got and get(got)->id != 0)
			return get(freebaseKeys[h]);
	}
	const char *fixed = fixFreebaseName(name);
	if (fixed[0] == 0)fixed = name;// NO FREEBASE!?
	N n = getThe(fixed);
	if (useHash and n)
		freebaseKeys[h] = n->id;
	return n;
}


map<long, Node *> complex_values;
int MISSING = 0;


Node *getPropertyDummy(const char *id) {
	//	N p=getAbstract(id);
	long hash = freebaseHash(id);
	N p = complex_values[hash];
	if (p)
		return p;
	p = add("◊");
	p->kind = _property;
	complex_values[hash] = p;
	return p;
}


Node *getEntity(char *name) {//=true
	if (name[0] == '<') {
		name++;
		if (name[strlen(name) - 1] == '>')
			name[strlen(name) - 1] = 0;
	}
	if (name[0] == '"') {
		name++;
		if (name[strlen(name) - 1] == '"')
			name[strlen(name) - 1] = 0;
	}
	size_t len = strlen(name);
	if (len == 0)return 0;
//	if(name[0]=='_' and name[1]==':')// 	_:
//		return getPropertyDummy(name);// Missing;// _:node1a8gbf20dx14041
	if (contains(name, ":"))return getAbstract(name);// Kategorie: ...
	if (endsWith(name, "\" .")) {
		name[len - 3] = 0;
		len -= 3;
	}
//	if(name[len-1]=='Z' && name[4]=='-' && name[len-4]==':')// "2017-09-27T07:28:28Z"
//		return dateValue(name);
	if (contains(name, "^^"))
		return rdfValue(name);
//    if(fixUrls)cut_wordnet_id(name)
	cut_to(name, " (");// !?
//	if((name[0]=='V' and name[1]<='C')) // or (name[0]=='n' and name[1]<='m')) ???
//		return getPropertyDummy(name);// DUMMY!
	if (name[0] == 'Q' and name[1] <= '9') {
		if (contains(name, '-'))
			return getPropertyDummy(name);// DUMMY!
		int id = atoi(name + 1);
		if (id > maxNodes / 2) {
			throw "id>maxNodes/2";
		}
		if (id > 0) {
			N n = getNode(id);
			n->id = id;// That's all we know for dummies
			return n;
		}
	}
	if (name[0] == 'P' and name[1] <= '9') {
//		if(thing[len(predicateName)-1]==">")thing[len(predicateName)-1]=0;
		N p = getWikidataRelation(name);
		if (p)return p;
		if (contains(name, '-'))// "P2430S28b08e46-49c5-061b-4035-1142c2c62e62"
			return getPropertyDummy(name);// DUMMY!
		//		return getNode((int)maxNodes-atoi(name+1));
		int kei = -atoi(name + 1) - propertyOffset;
		return get(kei);
	}
	bool useFreebase = false;
	// skip <m. but LEAVE THE >
	if (useFreebase and (startsWith(name, "m.") or startsWith(name, "g."))) {
		long h = freebaseHash(name + 2);
		int got = freebaseKeys[h];
		if (got and get(got)->id != 0) return get(freebaseKeys[h]);
		else {
			//      pf("MISSING %s\n", name);
			MISSING++;
			return 0;
		}
	}
	if (name[0] == '+')name++;// ausser +49 ...
//	name= fixYagoName(name);
	return dissectFreebase(name);
}

#define DROP true;
#define KEEP false;

bool dropBadSubject(char *name) {
	if (!name)return DROP;
	if (eq(name, ""))return DROP;
	if (name[0] <= '9')return DROP;
	if (startsWith(name, "Category:"))return DROP;
	if (startsWith(name, "Kategorie:"))return DROP;
//	too expensive here!  startsWith not that expensive?
	//	if(endsWith(name,"#propertyStatementLinkage>"))return DROP;
//	if(startsWith(name,"Q79823>"))return DROP; // <18736170>	◊		Globe		Erde		17744458=>79823=>2
//	if(startsWith(name,"Q5570970"))return DROP; // Globe
//	if(startsWith(name,"Q26956302"))return DROP; // rdf.freebase.com id?
//	if(startsWith(name,"http") or startsWith(name,"<http")){
//		if(startsWith(name,"<http://www.wikidata.org/entity/"))return KEEP;
//		return DROP;
//	}
	return KEEP;
}

bool dropBadPredicate(char *name) {
	if (!name)return DROP;
	if (eq(name, ""))return DROP;

	//	if(name[0]=='.')return DROP;
	if (name[0] == '<')name++;

//    if (predicateName[3] == '-' or predicateName[3] == '_' or predicateName[3] == 0) continue;    // <zh-ch, id ...
//    if (predicateName[2] == '-' or predicateName[2] == '_' or predicateName[2] == 0) continue;    // zh-ch, id ...
	if (eq(name, "prefLabel"))return DROP;
	if (eq(name, "Name"))return DROP;// Label!?
	if (eq(name, "dateModified"))return DROP; // eventuell doch später interessant?
	if (eq(name, "Version"))return DROP;// hmmm
	if (eq(name, "P352"))return DROP; //	UniProt ID
	if (eq(name, "P536"))return DROP; //	ATP ID
	if (eq(name, "P652"))return DROP; //	UNII
	if (eq(name, "P494"))return DROP; //	ICD-10
	if (eq(name, "P637"))return DROP; //	RefSeq Protein ID
	if (eq(name, "P705"))return DROP; //	Ensembl Protein ID
	if (eq(name, "P213"))return DROP;//	 	ISNI
	if (eq(name, "P214"))return DROP;//	 	VIAF
	if (eq(name, "P646"))return DROP;// Freebase-ID
	if (eq(name, "P508"))return DROP;// BNCF-Thesaurus
	if (eq(name, "P910"))return DROP;// Hauptkategorie zum Artikel !?!
	if (eq(name, "P1566"))return DROP;// GeoNames-ID
	if (eq(name, "P268"))return DROP; // Amerigo Vespucci BnF-ID 12234845j 47674->-10268
	if (eq(name, "P950"))return DROP;// BNE-ID
	if (eq(name, "P349"))return DROP;// Web NDL Authorities
	if (eq(name, "P1006"))return DROP;// NTA-Nummer
	if (eq(name, "P269"))return DROP;// SUDOC-Normdaten
	if (eq(name, "P409"))return DROP;// NLA Authorities
	if (eq(name, "P1017"))return DROP;// BAV (Vatikan) ID
	if (eq(name, "P691"))return DROP;// NK CR
	if (eq(name, "P906"))return DROP;// SELIBR
	if (eq(name, "P1005"))return DROP;// PTBNP
	if (eq(name, "P949"))return DROP;
	if (eq(name, "P734"))return DROP;
	if (eq(name, "P1207"))return DROP;
	if (eq(name, "P3221"))return DROP;
	if (eq(name, "P3984"))return DROP;
	if (eq(name, "P2633"))return DROP;
	if (eq(name, "P1343"))return DROP;
	if (eq(name, "P3417"))return DROP;
	if (eq(name, "P3733"))return DROP;
	if (eq(name, "P3106"))return DROP;
	if (eq(name, "P3478"))return DROP;
	if (eq(name, "P1417"))return DROP;
	if (eq(name, "P1245"))return DROP;
	if (eq(name, "P3222"))return DROP;
	if (eq(name, "P3569"))return DROP;
	if (eq(name, "P1036"))return DROP;
	if (eq(name, "P3500"))return DROP;
	if (eq(name, "P3743"))return DROP;
	if (eq(name, "P3225"))return DROP;
	if (eq(name, "P998"))return DROP;
	if (eq(name, "P227"))return DROP;
	if (eq(name, "P1855"))return DROP;// Wikidata-Eigenschafts-Beispiel
	if (eq(name, "P244"))return DROP;// lcauth
//	if(eq(name,"P1263"))return DROP;// Notable Names Database
	if (eq(name, "P3138"))return DROP; // OFDb-ID
	if (eq(name, "P2892"))return DROP;// UMLS CUI
	if (eq(name, "P1043"))return DROP;// IDEO-Berufs-ID
	if (eq(name, "P971"))return DROP;// Category:cu:Earth		Kategorie kombiniert die Themen
	if (eq(name, "P3911"))return DROP;// STW-ID  // 421 statements not worth it
	if (eq(name, "P2347"))return DROP;// YSO ID
//	if(eq(name,"P3827"))return DROP;// JSTOR-Themen-ID
//	if(eq(name,"P"))return DROP;
//	if(eq(name,"P"))return DROP;
//	if(eq(name,"P"))return DROP;
//	if(eq(name,"P"))return DROP;
//	if(eq(name,"P"))return DROP;

	return KEEP;
}

bool dropRedundantPredicate(char *name) {
	if (name[0] == '<')name++;
	if (eq(name, "P1081"))return DROP;// Index der menschlichen Entwicklung
	if (eq(name, "P1082"))return DROP;// Einwohnerzahl
	if (eq(name, "P2046"))return DROP;// Fläche
	if (eq(name, "P36"))return DROP;// Hauptstadt
	return KEEP;
}


bool dropBadObject(char *name) {
	if (!name)return DROP;
	if (eq(name, ""))return DROP;

	//	if(strstr(name,"Q4167410>"))return DROP;// Wikimedia-Begriffsklärungsseite later | Merge with abstract!
	return KEEP;
}

bool filterFreebase(char *name) { // EXPENSIVE!! do via shell!
	//  	.
	//  <m.05bjb__>	<#label>	"final-fantasy-xiii.jpg"@en	.
	//  <m.05bswf1>	<#label>	"andrew jackson first.jpg"@en	.
	//  <m.05c76ht>	<#label>	"Via Crucis.jpg"@en	.
	//  <m.05crs6s>	<#label>	"weinberg1.jpg"@en	.
	//  <m.05dchmw>	<#label>	"scan0008.jpg"@en	.
	if (startsWith(name, "<book.isbn")) return DROP;
	if (startsWith(name, "<biology.gene")) return DROP;
	if (startsWith(name, "<freebase.")) return DROP;
	if (eq(name, "<common.topic>")) return DROP;
	// nutrition_information
	if (eq(name, "\"/#type\"")) return true;

	if (endsWith(name, "update>")) return true;
	if (endsWith(name, "controls>")) return true;
	if (endsWith(name, "webpage>")) return true;
	if (endsWith(name, "uploaded_by>")) return true;
	if (endsWith(name, "notable_for>")) return true;
	if (endsWith(name, "permission>")) return true;
	if (endsWith(name, "key>")) return true;

	if (endsWith(name, "referenced_by>")) return true;
	if (endsWith(name, "statistics_entry>")) return true;
	if (startsWith(name, "<http")) return true;
	if (startsWith(name, "http")) return true;
	if (startsWith(name, "<common.annotation")) return true;
	if (startsWith(name, "<dataworld.")) return true;
	return false;
}


bool isUrl(char *predicateName) {
	return eq(predicateName, "P854") or eq(predicateName, "P856") or eq(predicateName, "P973")  \
 or eq(predicateName, "P1896") or eq(predicateName, "P1630") or eq(predicateName, "P1065")\
 or eq(predicateName, "P953") or eq(predicateName, "P963") or eq(predicateName, "P2699");// website
	// P973 Wird beschrieben in URL   P953 Fulltext hmmm  P963 Downloadlink not used P2699 URL not used
}
#define var auto
#define let auto
//#define auto var
bool importN3(cchar *file) {//,bool fixNamespaces=true) {
	autoIds = false;
	//  if(hasWord("vote_value"))return true;
	context = getContext(current_context);
	pf("Current nodeCount: %d\n", context->nodeCount);
	Node *subject;
	Node *predicate;
	Node *object;
	Node *lastPredicate = 0;
	int ignored = 0;
	int foreign = 0;// special case of ignored
	badCount = 0;
	char *subjectName0 = (char *) malloc(10000);
	char *predicateName0 = (char *) malloc(10000);
	char *objectName0 = (char *) malloc(10000);
	int linecount = 0;
//	char* line=(char*) malloc(MAX_CHARS_PER_LINE* sizeof(char));
	char line[MAX_CHARS_PER_LINE * sizeof(char)];
	while (readFile(file, &line[0])) {
		if (line[0] == 0)continue;// skip gzip new_block
//		if(debug and !contains(line, "P1476"))
//			continue;
//        p(line);
		//    if(linecount > 1000)break;//test!
		//		if (linecount % 1000 == 0 and linecount > 140000) p(linecount);
		if (++linecount % 100000 == 0) {
			long lost = ignored + badCount + MISSING;
			var format = "\r%d triples, ignored:%d foreign %d BAD:%d MISSING:%d = LOST:%ld GOOD:%ld";
			printf(format, linecount, ignored, foreign,badCount, MISSING, lost, linecount - lost);
			fflush(stdout);
			if (checkLowMemory()) {
				printf("Quitting import : id > maxNodes\n");
				exit(0);
				break;
			}
		}
//		if(!contains(line,"Q32652340"))continue; else p(line);
//		if(linecount<29000000)continue;
		//		if(debug)if(linecount>100)break;
		memset(objectName0, 0, 10000);
		memset(predicateName0, 0, 10000);
		memset(subjectName0, 0, 10000);
		if (line[0] == '#')continue;
		char *subjectName = subjectName0;
		char *predicateName = predicateName0;
		char *objectName = objectName0;
//		if(contains(line,"Q16061885"))
//			p(line);
//		else continue;
		//		sscanf(line, "%s\t%s\t%[^\t.]s", subjectName, predicateName, objectName);
		//		sscanf(line, "%s\t%s\t%s\t.", subjectName, predicateName, objectName);// \t. terminated !!
		sscanf(line, "%s\t%s\t%[^@>]s", subjectName, predicateName, objectName);
		int leng = len(line);
		if ((line[leng - 1] == '.' || line[leng - 2] == '.') and contains(line, "\"@"))
			if (!contains(line, "@de ")) {
				foreign++;
				continue;
			}//  and !contains(line,"@en ")
		if (objectName[0] == 'Q' && objectName[1] <= '9')objectName[0] = 'q';// hack against auto wiki ids
		subjectName = cut_wiki_id(subjectName);
		predicateName = cut_wiki_id(predicateName);
		if (!isUrl(predicateName))
			objectName = cut_wiki_id(objectName);
		if (debug && eq(predicateName, "description"))continue;// ignore in debug!

		if (dropBadSubject(subjectName)) {
			ignored++;
			continue;
		}//p(line);}
		if (dropBadPredicate(predicateName)) {
			ignored++;
			continue;
		}
		if (dropBadObject(objectName)) {
			ignored++;
			continue;
		}
		//		if(filterFreebase(objectName)){ignored++;continue;}
		fixNewline(objectName);
		// deCamel(predicateName);

//		u8_unescape(objectName, (int)strlen(objectName), objectName); NOT with new wikidata.n3!
//		u8_unescape(subjectName, (int)strlen(subjectName), subjectName);

		if (!objectName or objectName[0] == '/' or objectName[1] == '/')
			continue; // Key", 'object':"/wikipedia/de/Tribes_of_cain
		subject = getEntity(subjectName);//,fixNamespaces); //
		object = getEntity(objectName);//,fixNamespaces);
		predicate = getEntity(predicateName);
		if (subject == object) {
			bad();
			continue;
		}// no cyclic statements!
		if (predicate == Label || predicate == Description)
			u8_unescape(objectName, len(objectName), objectName);
		// unicode utf8 umlaut fix done in labels!
		if (predicate == Label) {
			if (!subject->name)
				setLabel(subject, objectName);
			else if (!eq(subject->name, objectName))
				addStatement(subject, Label, object);
			continue;
		}
		if (!subject or !predicate or !object) {
			bad();
			continue;
		}
		if (!subject->id or !predicate->id or !object->id) {
			bad();
			continue;
		}
//		if (!subject->name or !object->name ){ ignored++;continue;}// only german!
		if (!subject->id == _ignore or !predicate->id == _ignore or !object->id == _ignore) {
			ignored++;
			continue;
		}
		if (subject == Error or predicate == Error or object == Error) {
			bad();
			continue;
		}
		if (predicate == Instance) {// flip here!
			predicate = Type;
			N t = subject;
			subject = object;
			object = t;
		}
		if (subject->kind == _ignore or predicate->kind == _ignore or object->kind == _ignore) {
			ignored++;
			continue;
		} else if (eq(predicate, lastPredicate) and dropRedundantPredicate(predicateName)) {
			ignored++;
			continue;
		} else {
			lastPredicate = predicate;
			// todo Wikimedia-Begriffsklärungsseite Q4167410 -> abstract (force!?!)
			if (object == Class)// or endsWith(objectName, "#Class"))
				subject->kind = _clazz;
			else if (object == Entity || object == Item)// or endsWith(objectName, "#Entity"))
				subject->kind = _entity;
			else if (object->id == 1172284/*Dataset*/ || object->id == 4167410 ||
			         object->id == 4167836/*Wikimedia-Kategorie*/)
				subject->kind = _abstract;// todo?
			else
				addStatement(subject, predicate, object, !CHECK_DUPLICATES); // todo: id
		}
	}
	p("import N3 ok");
	closeFile(file);
	pf("BAD: %d   MISSING: %d\n", badCount, MISSING);
	context->use_logic = false;
	//	free(subjectName0);free(objectName0);free(predicateName0);// noone cares about 100bytes
	//	freebaseKeys.clear();
	//	free(freebaseKeys);
	return true;
}


void importFreebase() {
	useHash = true;
	autoIds = false;
//	if (!freebaseKeys[freebaseHash("0zzxc3>")]) // always
	importFreebaseLabels();
	importN3("freebase.data.txt");
}


bool importFacts(const char *file, const char *predicateName = "population") {
	p("import facts start");
	printf("WARNING: SETTING predicateName %s\n", predicateName);
	Node *subject;
	Node *predicate;
	Node *object;
	char line[1000];
	//  char* predicateName=(char*) malloc(100);
	predicate = getClass(predicateName);
	int linecount = 0;
	FILE *infile = open_file(file);
	char *objectName = (char *) malloc(100);
	char *subjectName = (char *) malloc(100);
	while (fgets(line, sizeof(line), infile) != NULL) {
		/* Get each line from the infile */
		if (++linecount % 10000 == 0) {
			printf("%d %s  \r", linecount, file);
			fflush(stdout);
		}
		if (!eq(predicateName, "population"))
			sscanf(line, "%s\t%s", subjectName, objectName); // no contextId? cause this db assumes GLOBAL id!
		else
			sscanf(line, "%*d\t%s\t%s\t%*d", /*&id,*/subjectName,
			       objectName /*, &certainty*/); // no contextId? cause this db assumes GLOBAL id!
		// printf(line);
		//	 printf("%d\t%d\t%d\n",subjectId,predicateId,objectId);
		//		if (contains(subjectName, "Begriffskl") or contains(subjectName, "Abk��rzung") or contains(subjectName, ":") or contains(subjectName, "#"))
		//			continue;
		if (contains(objectName, "jpg") or contains(objectName, "gif") or contains(objectName, "svg") or
		    contains(objectName, "#")
		    or contains(objectName, ":"))
			continue;

		subject = getAbstract(subjectName); //
		object = getAbstract(objectName);
		dissectWord(subject);
		Statement *s = 0;
		if (contains(objectName, subjectName, true))
			s = addStatement(subject, Member, object, !CHECK_DUPLICATES); // todo: id
		else
			s = addStatement(subject, predicate, object, !CHECK_DUPLICATES); // todo: id
		if (checkLowMemory()) {
			printf("Quitting import : id > maxNodes\n");
			exit(0);
			break;
		}
		if (!s)bad();
		//		showStatement(s);
	}
	free(subjectName);
	free(objectName);
	fclose(infile); /* Close the file */
	p("import facts ok");
	return true;
}


void importEntities() {
	getSingletons = false;// desaster!
	getBest = true;
	importCsv("couchdb/entities.csv");
}


void importNames() {
	addStatement(all(firstname), are, the(name));
	addStatement(all(firstname), Synonym, the(first
			                                          name));
	addStatement(all(male
			                 firstname), have_the(gender), the(male));
	addStatement(all(male
			                 firstname), are, the(firstname));
	addStatement(all(male
			                 firstname), Owner, the(male));
	addStatement(all(female
			                 firstname), have_the(gender), the(female));
	addStatement(all(female
			                 firstname), are, the(firstname));
	addStatement(all(female
			                 firstname), Owner, the(female));
	if (!germanLabels) {
		importList("FrauenVornamen.txt", "female firstname");
		importList("MaennerVornamen.txt", "male firstname");
	} else {
		importList("FrauenVornamen.txt", "weiblicher Vorname");
		importList("MaennerVornamen.txt", "männlicher Vorname");
	}
}

void importAbstracts() {
	char line[1000];
	char name0[1000];
	const char *name;
	//	char* line=(char*) malloc(1000);
	//	char* name=(char*) malloc(1000);
	int linecount = 0;
	int id;
	doDissectAbstracts = false;
	//	memset(abstracts, 0, abstractHashSize * 2);
	FILE *infile = open_file("wordnet/abstracts.tsv");
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 10000 == 0) {
			printf("importAbstracts %d        \r", linecount);
			fflush(stdout);
		}
		sscanf(line, "%d\t%[^\n]s", &id, name0); // %[^\n]s == REST OF LINE!

		//		for (int i = 0; i < strlen(name); i++)if(name[i]==' ')name[i]='_';
		name = name0;
		fixLabel(name0);
//		if (hasWord(name)) continue; // use wikidata abstracts! ok, ignored add_force anyways: (why?)
		id = -id - wordnetOffset; // OVERWRITE EVERYTHING below!!!
		N a = add_force(current_context, id, name, _abstract);
		insertAbstractHash(a);
	}
	fclose(infile); /* Close the file */
}

int wn_synonym_count = 400000;// pointer autoincrement
//      ^^^^^^^ aarg !?? compatible with german??

void importGermanLables(bool addLabels = false) {
	bool modify_english = hasWord("autoeroticism");//english already there
	char line[1000];
	char english[1000];
	char german[1000];
	char translations[2000];
	char *wordkind = (char *) malloc(100);
	int linecount = 0;
	int id;
	//  char pos;
	FILE *infile = open_file("translations.tsv");
	//	char** translationList=(char**)malloc(100);
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 10000 == 0) {
			printf("importLables %d  \r", linecount);
			fflush(stdout);
		}
		if (line[0] == '[')continue;
		//		if(linecount==4951)continue;
		fixNewline(line);
		sscanf(line, "%d\t%s\t%s\t%s\t%[^\\]]s", &id, wordkind, /*08950407n -> noun */english, german, translations);
		//		id=id -100000; // label on abstract !?!

		wn_labels[id] = german;
		//		if(eq(german,"Autoerotik"))
		//			p(line);
		int kind = noun;
		if (wordkind[0] == 'n')kind = noun;
		else if (wordkind[0] == 'v') {
			kind = verb;
			id += 200000000;
		}
		else if (wordkind[0] == 'a') {
			kind = adjective;
			id += 300000000;
		}
		else if (wordkind[0] == 'r') {
			kind = adverb;
			id += 400000000;
		} // between -> preposition!!!
		else {
			p(line);
		}
		id = norm_wordnet_id(id, true); // 100001740 -> 200000 etc
		if (!id)continue;
		Node *node = get(id);
		if (!isAbstract(node))node->kind = kind;
		if ((id < 1000 and id > -1000) or id > 1000)
			printf("MOMENT!");
		//		Node* abstract=getAbstract(german);
		if (modify_english) {
			setLabel(node, german);// NOW?
			//			addLabel(node, german);// to abstracts or words?? get(id) -> word
		} else {
			initNode(node, id, german, kind, wordnet);
			insertAbstractHash(node);
			//			^^ later: in importSenses, via:
		}
		wn_labels[id] = german;
		wn_labels[-id] = german;
		if (addLabels and strlen(translations) > 2) {// later, when settled (?)
			char **translationList = (char **) malloc(1000);
			char sep = ',';
			char *translationz = modifyConstChar(translations);
			translationz = translationz + 1;// cut [ ]
			translationz[strlen(translationz) - 1] = 0;
			int translationCount = splitStringC(translationz, translationList, sep);
			while (translationCount > 0) {
				translationCount--;
				char *translation = translationList[translationCount];
				if (translation[0] == ' ')translation++;
				addStatement(node, Label, getAbstract(translation), false);
			}
			//			free(translationz);
			free(translationList);
		}
		//		addStatement(get(id),Label,getAbstract(definition));
	}
	free(wordkind);
	fclose(infile); /* Close the file */
}

void importSenses() {
	char line[1000];
	char *name0 = (char *) malloc(1000);
	const char *name;
	int linecount = 0;
	int id, labelid, synsetid0, synsetid_mapped, senseid, sensenum;
	FILE *infile = open_file("wordnet/senses.tsv");
	Node *Sense = add_force(wordnet, _sense, "sense #", _internal);
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 10000 == 0) {
			printf("importSenses %d  \r", linecount);
			fflush(stdout);
		}
		fixNewline(line);
		sscanf(line, "%d\t%d\t%d\t%d\t%d\t%*d\t%*d\t%s", &id, &labelid, &synsetid0, &senseid,
		       &sensenum,/*&lexid,&tags,*/ name0);
		if (id < 1000)continue;// skip relations
		//		id=id + 10000; // NORM!!!
		id = -id - 100000;
		//		if (130172 == id) p(line);

		synsetid_mapped = norm_wordnet_id(synsetid0);// 100001740  -> 200000 and so on, no gaps
		if (synsetid_mapped < 300000 and synsetid_mapped > -300000) {
			printf("MOMENT!");
			continue;
		}
		Node *sense = get(synsetid_mapped);
		for (int i = 0; i < strlen(name0); i++)
			if (name0[i] == '%') name0[i] = 0;

		//		if(eq(name0,"autoeroticism"))
		//			pf("id %d synsetid0 %d synsetid %d name %s",id,synsetid0,synsetid_mapped,name0);
		if (germanLabels) {
			const char *german = wn_labels[synsetid_mapped].data();
			if (!german or strlen(german) == 0) {
				//				pf("id %d synsetid0 %d synsetid %d >>> %s\n",id,synsetid0,synsetid_mapped,name0);
				name = name0;// DEBUG with english!! //continue;
			} else name = german;
			//      OR add label!
		} else name = name0;

		//		Node* word=get(id);// redundant and risky! just use
		Node *word = get(name);
		//		if(word!=word2){
		//			printf("bug");
		//		}
		//		if(word->name and !eq(word->name,name)){
		//			printf("bug");
		//		}


		if (!sense->id) {
			initNode(sense, synsetid_mapped, name, 0, wordnet);
		} else if (!eq(sense->name, name)) {
			int syn_id = -wn_synonym_count; // NEGATIVE !
			Node *syno = initNode(get(syn_id), syn_id, name, 0, wordnet);
			addStatement(syno, Synonym, sense);
			//			addStatement(word,Instance,syno);// Sense
			sense = syno;
			wn_synonym_count++;
		}
		addStatement(word, Instance, sense, false);
		if (!germanLabels)
			addStatement(sense, Sense, number(sensenum), false);
	}
	free(name0);
	fclose(infile); /* Close the file */
}


void importSynsets() {
	if (germanLabels)return;
	char line[1000];
	char definition[1000];
	int linecount = 0;
	int id;
	char pos;
	FILE *infile = open_file("wordnet/synsets.tsv");
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 1000 == 0) {
			printf("importSynsets %d  \r", linecount);
			fflush(stdout);
		}
		fixNewline(line);
		sscanf(line, "%d\t%c\t%*d\t%[^\n]s", &id, &pos, /*&lexdomain,*/
		       definition);
		id = norm_wordnet_id(id);
		if (!id)continue;
		if (pos == 'n') addStatement4(wordnet, id, Type->id, noun); // get(id)->kind = noun; DEFAULT!!
		if (pos == 'v') addStatement4(wordnet, id, Type->id, verb); //get(id)->kind = verb;
		if (pos == 'a') addStatement4(wordnet, id, Type->id, adjective); //get(id)->kind = adjective;
		if (pos == 'r') addStatement4(wordnet, id, Type->id, adverb); //get(id)->kind = adverb;
		if (pos == 's') addStatement4(wordnet, id, Type->id, adjective);
		if (pos == 'p') addStatement4(wordnet, id, Type->id, preposition);
		//			get(id)->kind = adjective; // satelite !?
		setText(get(id), definition);
	}
	fclose(infile); /* Close the file */
}

void importDescriptions() {
	// labels for abstracts? why?
	char line[1000];
	char definition[1000];
	int linecount = 0;
	int id;
	//  char pos;
	FILE *infile = open_file("wordnet/labels.tsv");
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 10000 == 0) {
			printf("importDescriptions %d  \r", linecount);
			fflush(stdout);
		}
		fixNewline(line);
		sscanf(line, "%*d\t%d\t%[^\n]s", &id, /*&lexdomain,*/definition);
		//		id=id + 10000; // label on abstract !?!
		id = -id - 100000; // label on abstract !?!
		//		id=norm_wordnet_id(id);
		if (id >= 300000 or id <= -300000) {
			p(line);
			continue;
		}
		Node *old = get(id);
		setText(old, definition);//
		//		addStatement(get(id),Label,getAbstract(definition));
	}
	fclose(infile); /* Close the file */
}

void importLexlinks() {
	char line[1000];
	int linecount = 0;
	int s, p, o;
	int ss, so;
	FILE *infile = open_file("wordnet/lexlinks.tsv");
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 10000 == 0) {
			printf("importLexlinks %d  \r", linecount);
			fflush(stdout);
		}
		//			printf(line);
		fixNewline(line);
		sscanf(line, "%d\t%d\t%d\t%d\t%d", &ss, &s, &so, &o, &p);
		//		Statement* old = findStatement(subject, predicate, object, 0, 0, 0); //,true,true,true);
		//		if (old)return old; // showStatement(old)
		//
		//		s=s + 10000;
		//		o=o + 10000;
		s = -s - 100000;
		o = -o - 100000;
		if (p == SubClass->id) continue; // Redundant data!
		if (p == Instance->id) continue; // Redundant data!

		Statement *x = 0;
		if (ss != so) x = addStatement4(wordnet, norm_wordnet_id(ss), norm_wordnet_id(p), norm_wordnet_id(so));
		if (debug and !x and p != 81)
			pf("ERROR %s\n", line);
		//		if(s!=o)x=addStatement4(wordnet, s, p, o); not on abstracts! hmm, for antonym properties? nah!
		//		if(!x)printf("ERROR %s\n",line);
	}
	fclose(infile); /* Close the file */
}

void importStatements() {
	char line[1000];
	int linecount = 0;
	int s, p, o;
	FILE *infile = open_file("wordnet/statements.tsv");
	while (fgets(line, sizeof(line), infile) != NULL) {
		if (++linecount % 1000 == 0) {
			printf("importStatements %d  \r", linecount);
			fflush(stdout);
		}
		fixNewline(line);
		sscanf(line, "%d\t%d\t%d", &s, &o, &p);
		//		Statement* old = findStatement(subject, predicate, object, 0, 0, 0); //,true,true,true);
		//		if (old)return old; // showStatement(old)
		if (p == SubClass->id) continue; // Redundant data!
		if (p == Instance->id) continue; // Redundant data!
		addStatement4(wordnet, norm_wordnet_id(s), norm_wordnet_id(p), norm_wordnet_id(o));
	}
	fclose(infile); /* Close the file */
}

void importWordnet() {
	autoIds = false;
	context = getContext(wordnet);
	load_wordnet_synset_map();
	//	if(hasWord()) checkWordnet()
	importAbstracts(); // MESSES WITH ABSTRACTS!!
	if (germanLabels)
		importGermanLables();
	importSenses();
	getContext(wordnet)->lastNode = wn_synonym_count; //200000+117659;//WTH!
	importSynsets();
	importDescriptions();// English!
	importStatements();
	importLexlinks();
	if (germanLabels)
		importGermanLables(true);// Now the other labels (otherwise abstracts might fail?)

//	mergeNode(get(-81), get(81));// derived
//	mergeNode(get(-80), get(80));// derives pertainym
//	mergeNode(get(-30), get(30));// opposite = antonym
	// fix how???? ^^^^^^^^
	context->use_logic = true;
}

void importWordnet2() {
	importNodes(); // FIRST! Hardlinked ids overwrite everything!!
	importStatements2();
}

void importGeoDB() {
	importCsv("cities1000.txt", getThe("city"), '\t', "alternatenames,modificationdate,geonameid",
	          "latitude,longitude,population,elevation,countrycode", 2, "asciiname");
}

/*
 asins,parentASIN,sku,brand,author,artist,title,imagepathmedium,imagepathsmall,imagepathlarge,topcategory,ean,mpn,eanlist,mpnlist,productdescription,platforms,releasedate,eec,salerank,browsenode1,browsenode2,subcategorypath1,subcategorypath2,gender,color,size,stp,info1,info2,price1,availablity1,shipping1,salerestriction1,url1,merchantId1,PPU1,promo1,claimcode1,startdate1,enddate1,price2,availablity2,shipping2,salerestriction2,url2,merchantId2,PPU2,promo2,claimcode2,startdate2,enddate2,deltaflag
 B00ERMAYT4,,"","","","","Kurs auf Marcus 12",http://ecx.images-amazon.com/images/I/41gUL5lpi-L._SL160_.jpg,http://ecx.images-amazon.com/images/I/41gUL5lpi-L._SL75_.jpg,http://ecx.images-amazon.com/images/I/41gUL5lpi-L.jpg,"TV Series Episode Video on Demand",,"","","","Kurs auf Marcus 12","",2012-12-31,"",105111,3015916031,3356021031,"Amazon Video/Kategorien/Serien","Amazon Video/Specialty Stores/Custom Stores/Specialty Stores in Amazon Video/Kaufen und Leihen/Serien","unisex","","",,,,2.99,"Versandfertig in 1 - 2 Werktagen",3.00,"siehe Website",http://www.amazon.de/dp/B00ERMAYT4/ref=asc_df_B00ERMAYT430533458?smid=A3HBS5CIENBL3I&tag=ihre_partner_id&linkCode=df0&creative=22506&creativeASIN=B00ERMAYT4&childASIN=B00ERMAYT4,A3HBS5CIENBL3I,"","",,,,,"",siehe Website,"siehe Website",,,"","",,,
 GOOD:
 */
void importBilliger() {
	//	importCsv("billiger.de/TOI_Suggest_Export_Categories.csv",getThe("billiger.de category")); besser da:
	importCsv("billiger.de/CURRENT-TOI_Suggest_Export_Products.csv.gz", getThe("billiger.de product"));
//	importCsv("billiger.de/TOI_Suggest_Export_Products.csv",getThe("billiger.de product"));
//	importCsv("billiger.de/20170120-TOI_Suggest_Export_Products.csv",getThe("billiger.de product"));
}

void importAmazon() {
//	char separator, const char* ignoredFields, const char* includedFields, int nameRowNr,	const char* nameRow) 
//	importCsv("amazon/de_v3_csv_apparel_retail_delta_20151211.base.csv.gz",getThe(""));
	const char *includedFields =// typ;
			"title";//,brand,author,artist,subcategorypath1";
//						"subcategorypath1";
//	"title,productdescription,asins,brand,author,artist,imagepathmedium,topcategory,ean,platforms,releasedate,salerank,subcategorypath1,subcategorypath2,gender,color,size,price1";
//	"asins,brand,author,artist,title,imagepathmedium,topcategory,ean,platforms,releasedate,salerank,browsenode1,subcategorypath1,subcategorypath2,gender,color,size,price1,availablity1,shipping1url1";
	const char *ignoredFields = 0;// rest! productdescription :(
	const char *in = includedFields;
	const char *out = ignoredFields;
	const char *t = "title";
//		int col=22;//subcategorypath1
	int col = 6;//title
	getSingletons = true;
	autoIds = false;

	importCsv("amazon/de_v3_csv_digital_video_retail_delta.base.csv.gz", getThe("Amazon digital_video product"), ',',
	          out, in, col, t);
	importCsv("amazon/de_v3_csv_dvd_retail_delta.base.csv.gz", getThe("Amazon dvd product"), ',', out, in, col, t);
	if (testing)exit(0);
	importCsv("amazon/de_v3_csv_apparel_retail_delta.base.csv.gz", getThe("Amazon apparel product"), ',', out, in, col,
	          t);
	importCsv("amazon/de_v3_csv_beauty_retail_delta.base.csv.gz", getThe("Amazon beauty product"), ',', out, in, col,
	          t);
	importCsv("amazon/de_v3_csv_automotive_retail_delta.base.csv.gz", getThe("Amazon automotive product"), ',', out, in,
	          col, t);
	importCsv("amazon/de_v3_csv_baby_retail_delta.base.csv.gz", getThe("Amazon baby product"), ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_books_retail_delta_part1.base.csv.gz", getThe("Amazon books product"), ',', out, in,
	          col, t);
	importCsv("amazon/de_v3_csv_ce_retail_delta.base.csv.gz", getThe("Amazon ce product"), ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_digital_sw_retail_delta.base.csv.gz", getThe("Amazon digital_sw product"), ',', out, in,
	          col, t);
	importCsv("amazon/de_v3_csv_digital_vg_retail_delta.base.csv.gz", getThe("Amazon digital_vg product"), ',', out, in,
	          col, t);
	importCsv("amazon/de_v3_csv_grocery_retail_delta.base.csv.gz", getThe("Amazon grocery product"), ',', out, in, col,
	          t);
	importCsv("amazon/de_v3_csv_home_improvement_retail_delta.base.csv.gz", getThe("Amazon home_improvement product"),
	          ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_home_retail_delta.base.csv.gz", getThe("Amazon home product"), ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_hpc_retail_delta.base.csv.gz", getThe("Amazon hpc product"), ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_jewelry_retail_delta.base.csv.gz", getThe("Amazon jewelry product"), ',', out, in, col,
	          t);
	importCsv("amazon/de_v3_csv_kitchen_retail_delta.base.csv.gz", getThe("Amazon kitchen product"), ',', out, in, col,
	          t);
	importCsv("amazon/de_v3_csv_lawn_garden_retail_delta.base.csv.gz", getThe("Amazon lawn_garden product"), ',', out,
	          in, col, t);
	importCsv("amazon/de_v3_csv_luggage_retail_delta.base.csv.gz", getThe("Amazon luggage product"), ',', out, in, col,
	          t);
	importCsv("amazon/de_v3_csv_major_appliances_retail_delta.base.csv.gz", getThe("Amazon major_appliances product"),
	          ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_musical_instruments_retail_delta.base.csv.gz",
	          getThe("Amazon musical_instruments product"), ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_office_retail_delta.base.csv.gz", getThe("Amazon office product"), ',', out, in, col,
	          t);
	importCsv("amazon/de_v3_csv_pc_retail_delta.base.csv.gz", getThe("Amazon pc product"), ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_personal_care_appliances_retail_delta.base.csv.gz",
	          getThe("Amazon personal_care_appliances product"), ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_pet_retail_delta.base.csv.gz", getThe("Amazon pet product"), ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_shoes_retail_delta.base.csv.gz", getThe("Amazon shoes product"), ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_software_retail_delta.base.csv.gz", getThe("Amazon software product"), ',', out, in,
	          col, t);
	importCsv("amazon/de_v3_csv_sports_retail_delta.base.csv.gz", getThe("Amazon sports product"), ',', out, in, col,
	          t);
	importCsv("amazon/de_v3_csv_toys_retail_delta.base.csv.gz", getThe("Amazon toys product"), ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_video_games_retail_delta.base.csv.gz", getThe("Amazon video_games product"), ',', out,
	          in, col, t);
	importCsv("amazon/de_v3_csv_watches_retail_delta.base.csv.gz", getThe("Amazon watches product"), ',', out, in, col,
	          t);
	importCsv("amazon/de_v3_csv_wine_retail_delta.base.csv.gz", getThe("Amazon wine product"), ',', out, in, col, t);
	importCsv("amazon/de_v3_csv_music_retail_delta.base.csv.gz", getThe("Amazon music product"), ',', out, in, col, t);

}
//
//void importAmazon(){
//	importAmazon2("subcategorypath1");
//}

void importDBPediaEN() {
	useHash = false;
	importLabels("dbpedia_en/labels_en.ttl");
	importLabels("dbpedia_en/raw_infobox_property_definitions_en.ttl");
	importLabels("dbpedia_en/category_labels_en.ttl");

	importN3("dbpedia_en/instance_types_en.ttl");
	importN3("dbpedia_en/mappingbased_properties_cleaned_en.ttl");
	importN3("dbpedia_en/raw_infobox_properties_en.ttl");
	importN3("dbpedia_en/persondata_en.ttl");
	importN3("dbpedia_en/images_en.nt");// IMAGE LOGIC??
	importN3("dbpedia_en/skos_categories_en.ttl");// broader == superclass OR type OR Something different!!! RELATED
}

// http://data.dws.informatik.uni-mannheim.de/dbpedia/2014/de/
void importDBPediaDE() {
	useHash = false;
	importLabels("dbpedia_de/labels.csv");
	////	importLabels("dbpedia_de/raw_infobox_property_definitions_en_uris_de.nt") REPLACED BY:;
	importLabels("dbpedia_de/labels_en_uris_de.nt");
	importLabels("dbpedia_de/category_labels_en_uris_de.ttl");

	//	importLabels("dbpedia_de/persondata_en_uris_de.ttl");
	importN3("dbpedia_de/raw_infobox_properties_en_uris_de.ttl");
	importN3("dbpedia_de/persondata_en_uris_de.ttl");
}

// IMPORTANT: needs manual collectAbstracts() afterwards (for speed reasons??)

void importAllYago() {
	autoIds = false;
	//  importYago("yagoGeonamesData.tsv");// expansive !!! don't dissect!
	//	importYago()
	load_wordnet_synset_map();
	//  importYago("yagoLabels.tsv");

	importYago("yagoSimpleTypes.tsv");
	importYago("yagoFacts.tsv");
	check(hasWord("Tom_Hartley"));
	importYago("yagoLiteralFacts.tsv");
	importYago("yagoStatistics.tsv");
	importYago("yagoSchema.tsv");
	//	importYago("yagoGeonamesEntityIds.tsv");
	//	importYago("yagoGeonamesClassIds.tsv");
	importYago("yagoGeonamesClasses.tsv");
	importYago("yagoGeonamesGlosses.tsv");
	importYago("yagoSimpleTaxonomy.tsv");
	//importYago("yagoWordnetIds.tsv");// hasSynsetId USELESS!!!
	//importYago("yagoWordnetDomains.tsv");
	//importYago("yagoMultilingualClassLabels.tsv");
	//	importYago("yagoTaxonomy.tsv");todo
	//importYago("yagoDBpediaClasses.tsv");
	//importYago("yagoDBpediaInstances.tsv");
	//importYago("yagoMetaFacts.tsv");
	importYago("yagoImportantTypes.tsv");

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


/*root@h1642655:~/netbase# l facts/
 actedIn      during       hasChild      hasISBN        hasRevenue    interestedIn  isSubstanceOf  subClassOf
 bornIn      establishedOnDate  hasCurrency    hasLabor        hasSuccessor   inTimeZone   livesIn     subPropertyOf
 bornOnDate    exports       hasDuration    hasMotto        hasTLD      inUnit     locatedIn    type
 created      familyNameOf    hasEconomicGrowth hasNominalGDP     hasUnemployment isAffiliatedTo madeCoverFor   until
 createdOnDate   foundIn       hasExpenses    hasNumberOfPeople   hasUTCOffset   isCalled    means      using
 dealsWith     givenNameOf     hasExport     hasOfficialLanguage  hasValue     isCitizenOf   musicalRole   worksAt
 describes     graduatedFrom    hasGDPPPP     hasPages        hasWaterPart   isLeaderOf   originatesFrom  writtenInYear
 diedIn      happenedIn     hasGini      hasPopulation     hasWebsite    isMarriedTo   participatedIn  wrote
 diedOnDate    hasAcademicAdvisor hasHDI       hasPopulationDensity  hasWeight    isMemberOf   politicianOf
 directed     hasArea       hasHeight     hasPoverty       hasWonPrize   isNativeNameOf produced
 discovered    hasBudget      hasImdb      hasPredecessor     imports     isNumber    publishedOnDate
 discoveredOnDate hasCallingCode   hasImport     hasProduct       influences    isOfGenre    range
 domain      hasCapital     hasInflation    hasProductionLanguage inLanguage    isPartOf    since
 */

void importTest() {
	context = getContext(wikidata);
//	replay();
	importBilliger();
	importAmazon();
	buildSeoIndex();
//	check(hasNode("ue55h6600"));
//	importAllDE();
//	importWikiLabels("wikidata/wikidata-terms.de.nt");
//	importWikiLabels("wikidata/wikidata-terms.en.nt",false);
}


void importWikiData() {
	context = getContext(wikidata);
	autoIds = false;
	importing = true;
	context->lastNode =
			(int) maxNodes / 2;// hack: Reserve the first half of memory for wikidata, the rest for other stuff
//	context->lastNode=30244576; // as of 06/2017!
//    importWikiLabels("wikidata/properties.de",true);
//	if(!eq(get(1)->name,"Universum"))
	importWikiLabels("wikidata/labels.de.n3");

//		importWikiLabels("wikidata/labels.csv");
//		importWikiLabels("wikidata/labels.csv",false,true);// altlabels after abstracts are sorted!
//	}
	importN3("wikidata/latest-truthy.nt");
//	importN3("wikidata/latest-truthy.nt.gz");// MISSING STUFF WHY?? only two Q1603262
//    importN3("wikidata/wikidata.n3");
}

void importWikiDataALT() {
	context = getContext(wikidata);
	useHash = false;
	autoIds = false;
	//	useHash=true;
	importing = true;
	//	doDissectAbstracts=false; // if MAC
	//		importLabels("dbpedia_de/labels.csv");
	//	importWikiLabels("wikidata/wikidata-terms.en.nt",false);// prefill!
	//	importWikiLabels("wikidata/wikidata-properties.en.nt",true);
	context->lastNode =
			(int) maxNodes / 2;// hack: Reserve the first half of memory for wikidata, the rest for other stuff
	if (germanLabels) {
		importWikiLabels("wikidata/wikidata-properties.nt.gz", true);
		importWikiLabels("wikidata/wikidata-terms.de.nt");
		importWikiLabels("wikidata/wikidata-terms.de.nt", false,
		                 true);// NOW alt labels: don't mess with abstracts before
		showContext(current_context);
		//		collectAbstracts(); BREAKS THINGS!
		//		importLabels("wikidata/wikidata-properties.de.nt");
		//		importLabels("wikidata/wikidata-terms.nt.gz");// OK but SLOOOW!
	} else {
		importWikiLabels("wikidata/wikidata-properties.nt.gz", true);
		importWikiLabels("wikidata/wikidata-terms.en.nt", false);// fill up missing ONLY!
//		importWikiLabels("wikidata/wikidata-terms.de.nt",false,true);// NOW alt labels: don't mess with abstracts before
	}
//	doDissectAbstracts=true;// already? why not
	//	importN3("wikidata/wikidata-properties.nt.gz");// == labels!
	importN3("wikidata/wikidata-taxonomy.nt.gz");
	importN3("wikidata/wikidata-instances.nt.gz");
	importN3("wikidata/wikidata-simple-statements.nt.gz");
	//	importN3("wikidata/wikidata-statements.nt.gz");
	//	importN3("wikidata/wikidata-sitelinks.nt");
	if (germanLabels) {// now fill up missing! Not before, otherwise would get useless statements.
		importWikiLabels("wikidata/wikidata-terms.en.nt", false);
		importWikiLabels("wikidata/wikidata-terms.en.nt", false, true);
	}
	showContext(current_context);
}


void import(const char *type, const char *filename) {
	importing = true;
	//  clock_t start;
	//  double diff;
	// start = clock();
	// diff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
	if (filename == 0) filename = type;
	if (eq(filename, "de")) {
		importAllDE();
	} else if (eq(type, "all")) {
		importAll();
	} else if (eq(type, "amazon")) {
		importAmazon();
	} else if (eq(type, "billiger") or eq(type, "billiger.de")) {
		importBilliger();
	} else if (eq(type, "test")) {
		importTest();
	} else if (eq(type, "labels")) {
		importLabels("labels.csv", false, true, true);
	} else if (endsWith(type, "csv")) {
		importCsv(filename);
	} else if (eq(type, "wordnet")) {
		importWordnet();
	} else if (eq(type, "freebase")) {
		importFreebase();
	} else if (eq(type, "geodb") or eq(type, "geo") or eq(type, "locations") or eq(type, "places") or
	           eq(type, "cities")) {
		importGeoDB();
	} else if (eq(type, "dbpedia")) {
		if (germanLabels)
			importDBPediaDE();
		else
			importDBPediaEN();
	} else if (eq(type, "names")) {
		importNames();
	} else if (eq(type, "images")) {
		importImages();
	} else if (startsWith(type, "images ")) {
		importImageTripels(substr(type, 7, -1));
	} else if (eq(type, "wiki") or eq(type, "wikidata")) {
		importWikiData();
		//	} else if (eq(type, "wiki")) {
		//		importWikipedia();
		//	} else if (eq(type, "topic")) {
		//		importWikipedia();
	} else if (eq(type, "entities")) {
		importEntities();
	} else if (eq(type, "yago")) {
		if (eq(filename, "yago")) importAllYago();
		//		else if (contains(filename, "fact"))
		//			importFacts(filename, filename);
		//		else
		importYago(filename);
	} else if (endsWith(filename, "txt")) {
		importCsv(filename);
	} else if (endsWith(filename, "csv")) {
		importCsv(filename);
	} else if (endsWith(filename, "tsv")) {
		importCsv(filename);
	} else if (endsWith(filename, "json")) {
		importJson(filename);
	} else if (endsWith(filename, "xml")) {
		importXml(filename);
	} else if (endsWith(filename, "n3")) {
		importN3(filename);
	} else if (endsWith(filename, "nt")) {
		importN3(filename);
	} else if (endsWith(filename, "ttl")) {
		importN3(filename);
	} else {
		//if (!importFacts(filename, filename))
		printf("Unsupported file type %s %s\n", type, filename);
		//		importAll();
	}
	importing = false;
	// cout<<"nanoseconds "<< diff <<'\n';

	// importSqlite(filename);
	//  importNodes();
	//  importStatements();
}

void importAllDE() {
	importing = true;
	germanLabels = true;
	autoIds = false;
	p("importAll GERMAN");
//	importLabels("labels.csv");
//	importWordnet();
	//	doDissectAbstracts=true;// already? why not
	doDissectAbstracts = false;//MESSES TOO MUCH! why?
	//	importDBPediaDE();
	importWikiData();
//	context->lastNode=1;// RADICAL: fill all empty slots, no gaps! // DANGER FUCKUPS! WITH ENGLISH!
//	context->nodeCount=context->lastNode;// adjust gaps as counted?
//	importNames();
	importGeoDB();
	importCsv("used_keywords.csv");
	importCsv("whole_data.csv");
	importCsv("Telekom_Entitaet.csv");
	importCsv("Telekom-Produkt.csv");
	importCsv("Telekom_Produkt.csv");
	importCsv("manual_entities.csv");

//	importQuasiris();:
	replay();
//	importFacts();
	importLabels("labels.csv");// todo: why again?
//	buildSeoIndex();
	importBilliger();
	buildSeoIndex();
	importAmazon();
	//importEntities();
	//importImagesDE(); deprecated
	importing = false;
}

void importRemaining() {
	importing = true;
	germanLabels = true;
	autoIds = false;
	importCsv("Telekom_Entitaet.csv");
	importCsv("Telekom-Produkt.csv");
	importCsv("whole_data.csv");
	importAmazon();
	importBilliger();
	buildSeoIndex();
	importing = false;
}

void importAll() {
	importing = true;
	autoIds = false;
	if (germanLabels)
		return importAllDE();
	p("importAll ENGLISH");
//	importWordnet();
	//	importCsv("adressen.txt");
	//	doDissectAbstracts=true;// already? why not
	doDissectAbstracts = true;// already? why not
//	importGeoDB();
	importWikiData();
	importNames();
	importAmazon();
//	importBilliger();
	//	if(germanLabels)
	//		importDBPediaDE();
	//	else
	//		importDBPediaEN();
	//	importImages();

	showContext(wordnet);
	//	importFreebase();
	showContext(wordnet);
	//  	importAllYago();// BETTER THAN DBPEDIA!?
	importImages();
	importing = false;
	// ./import/yago/yagoSimpleTypes.tsv Error opening file: No such file or directory Segmentation fault
	//  importEntities();
}


ostream &operator<<(ostream &outputStream, Node &node) {
	outputStream << "Node #" << node.id << " " << node.name;
	return outputStream;
}

ostream &operator<<(ostream &outputStream, Node *node) {
	outputStream << "Node #" << node->id << " " << node->name;
	return outputStream;
}

#define RAPIDJSON
#ifdef RAPIDJSON

#include "rapidjson/document.h"

using namespace rapidjson;
typedef rapidjson::Value JValue;

void p(JValue &s) {
	if (s.IsString())
		printf("%s", s.GetString());
	else if (s.IsDouble())
		printf("%lf", s.GetDouble());
}

Node *entity(JValue &s) {
	if (s.IsString())return getThe(s.GetString());
	else return bad();
}

Node *value(JValue &s) {
	if (s.IsDouble())return value(0, s.GetDouble());
}

void importJson(const char *file, Node *type, const char *ignoredFields, const char *foldFields) {
	p("\nimport importJson start\n");
#undef MAX_CHARS_PER_LINE
#define MAX_CHARS_PER_LINE 100000
	bool tmp_autoIds = autoIds;
	autoIds = false;
	badCount = 0;
	context = getContext(current_context);
	char line[MAX_CHARS_PER_LINE];
	char **values = (char **) malloc(sizeof(char *) * MAX_ROWS);
	char lastValue[MAX_CHARS_PER_LINE];
	char *line0 = 0;// nullptr;
	map<char *, Node *> valueCache;
	Node *subject = 0;
	Node *predicate = 0;
	Node *object = 0;
	Node *_street = getThe("street");
	Node *_state = getThe("state");
	Node *_postcode = getThe("postcode");
	Node *_city = getThe("city");
	Node *_longitude = getThe("longitude");
	Node *_latitude = getThe("latitude");
	Node *_osm_id = getThe("osm_id");
	int linecount = 0;
//	if(!type)type=extractType(file); // Node* type,  auto per row

	vector<Node *> predicates = *new vector<Node *>();
//	vector<string>& ignoreFields=splitString(ignoredFields, ",");
//	vector<string>& includeFields=splitString(includedFields, ",");
	int fieldCount = 0;
	int size = 0;// per row ~ Hopefully equal to fieldCount
	while (readFile(file, &line[0])) {
		fixNewline(line, false);
		line0 = &line[0];
		if (++linecount % 1000 == 0) {
			pf("importCsv %s stats: %d good, %d bad \r", file, linecount, badCount);
			fflush(stdout);
		}
/*{"_index":"photon","_type":"place","_id":"30006709","_score":1,"_source":{"osm_key":"building","coordinate":{"lon":13.558588612847423,"lat":52.76052905},"street":{"de":"Am Obersee","default":"Am Obersee"},"state":{"de":"Brandenburg","default":"Brandenburg","it":"Brandeburgo","fr":"Brandebourg"},"osm_type":"W","postcode":"16359","osm_value":"yes","city":{"de":"Wandlitz","default":"Wandlitz"},"country":{"default":"Deutschland","it":"Germania","fr":"Allemagne","en":"Germany"},"importance":0,"extent":{"type":"envelope","coordinates":[[13.5584493,52.7606334],[13.558727,52.7604211]]},"name":{"default":"Hochzeitsvilla am See in Lanke"},"context":{},"osm_id":510144480}}
*/
//		p("----------------------------------------");
		rapidjson::Document json;
		json.Parse(line);
		auto err = json.GetParseError();
		if (err) {
			p("ERROR");
			p(line);
			p(err);
			continue;
		}

		Node *type0 = entity(json["_type"]);
		if (!eq(type0->name, "place"))
			p(type0->name);
		Node *id = entity(json["_id"]);
		JValue &source = json["_source"];
		Node *type = entity(source["osm_key"]);

//		if(source["country"]["default"].GetString()!="Deutschland")
//			continue;
//		Node* node;

		Node *node;
		Node *street;

		if (source.HasMember("name") and source["name"].HasMember("default"))
			node = entity(source["name"]["default"]);
		else if (!source.HasMember("street"))
			continue;//bad

		if (source.HasMember("street")) {
			if (source["street"].HasMember("default"))
				street = entity(source["street"]["default"]);
			else if (source["street"].HasMember("de"))
				street = entity(source["street"]["de"]);
			else if (!node)continue;
		}
		if (!node)node = street;
//		p(source["osm_key"]);
		addStatement(node, get(_Type), type);
//		addStatement(node, get(_Type), type0);
		if (street and street != node)
			addStatement(node, _street, street);
		if (source.HasMember("postcode"))
			addStatement(node, _postcode, entity(source["postcode"]));
		if (source.HasMember("city"))
			addStatement(node, _city, entity(source["city"]["default"]));
		if (source.HasMember("state"))
			addStatement(node, _state, entity(source["state"]["default"]));
		if (source.HasMember("coordinate")) {
			Node *lon = value(source["coordinate"]["lon"]);
			Node *lat = value(source["coordinate"]["lat"]);
			addStatement(node, _longitude, lon);
			addStatement(node, _latitude, lat);
		}
		addStatement(node, _osm_id, id);
//		p(node);
	}
}

#else
void importJson(const char* file, Node* type, const char* ignoredFields, const char* foldFields) {
	p("importJson not supported without rapidjson");
}
#endif