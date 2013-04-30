#pragma once
// strcmp
#include <string.h>
#include <cstdlib>


#ifdef sqlite3
#include "sqlite3.h"
#endif



#include "netbase.hpp"
#include "util.hpp"

#include "import.hpp"
#include "relations.hpp"// for wordnet

// 64 BIT : %x -> %016llX
string basepath = "./import/";
char* nodes_file = "data/nodes.txt";
char* statements_file = "data/statements.txt";
char* images_file = "data/images.txt";


void norm(char* title) {
	int len = strlen(title);
	for (int i = len; i >= 0; --i) {
		if (title[i] == ' ' || title[i] == '_' || title[i] == '-') {
			strcpy(&title[i], &title[i + 1]); //,len-i);
		}
	}
}
map<long, string> nodeNameImages;
map<long, string> nodeNameImages2; // chopped
//map<long,string> nodeNameImages3;// chopped image name

void importImages() {// 18 MILLION!   // 18496249
	p("image import starting ...");
	FILE *infile;
	char line[100];
	int linecount = 0;
	Node* wiki_image = getAbstract("wiki_image");
	addStatement(wiki_image, is_a, getThe("image"));

	/* Open the file.  If NULL is returned there was an error */

	printf("Opening File %s\n", (basepath + images_file).data());
	if ((infile = fopen((basepath + images_file).data(), "r")) == NULL) {
		perror("Error opening file");
		exit(1);
	}
	char tokens[1000];
	char image[1000];
	char title[1000];
	int good = 0;
	int bad = 0;
	while (fgets(line, sizeof (line), infile) != NULL) {
		if (++linecount % 10000 == 0) {
			pi(linecount);
		};
		sscanf(line, "%s %*s %s", title, image);
		if (!hasWord(title))
			norm(title); //blue -_fin ==> bluefin
		if (!hasWord(title)) {// currently only import matching words.
			//            if(++bad%1000==0){ps("bad image (without matching word) #");pi(bad);}
			continue;
		}
		if (getImage(title) != "")
			continue; //already has one ; only one so far!
		Node* subject = getAbstract(title);
		Node* object = getAbstract(image); // getThe(image);;
		addStatement(subject, wiki_image, object, false);
		if (++good % 1000 == 0) {
			ps("GOOD images:");
			pi(good);
		}
	}
	fclose(infile);

	good = 0;
	Node* object = getAbstract(image); // getThe(image);;
	/*
	// again, this time with word fragments
	 * MEMORY LEAK!? where??
	infile = fopen((basepath + images_file).data(), "r");
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
	p("done image import");
}

void importNodes() {
	p("node import starting ...");
	FILE *infile;
	// char fname[40]="/Users/me/data/base/netbase.sql";
	//  ('2282','Anacreon','N'),

	// char* fname="/Users/me/data/base/netbase/nodes.test";
	//2026896532	103	dry_out	Verb	\N	\N	11	103

	char line[100];
	int linecount = 0;
	/* Open the file.  If NULL is returned there was an error */
	printf("Opening File %s\n", (basepath + nodes_file).data());
	if ((infile = fopen((basepath + nodes_file).data(), "r")) == NULL) {
		perror("Error opening file");
		exit(1);
	}
	while (fgets(line, sizeof (line), infile) != NULL) {
		char tokens[1000];
		/* Get each line from the infile */

		if (++linecount % 1000 == 0)
			pi(linecount);
		strcpy(tokens, line);
		int x = 0; // replace ' ' with '_'
		while (tokens[x]) {
			if (tokens[x] == ' ')
				tokens[x] = '_';
			x++;
		}
		// char name[1000];
		char* name = (char*) malloc(100);
		// char kind[20];
		char contextId_s[100];
		char deleted[1];
		char version[1];
		char wordKindName[100];
		int wordKind;
		int Id;
		int kind;
		int contextId = wordnet;
		int contextID;
		//2026896532	103	dry_out	Verb	\N	\N	11	103
		//	sscanf(tokens,"%d\t%s\t%s\t%s\t%*s\t%*s\t%*d\t%d",&Id,contextId_s,name,wordKindName,&kind);
		sscanf(tokens, "%d\t%s\t%s\t%*s\t%*s\t%*s\t%d\t%*d", &Id, contextId_s, name, &kind); // wordKind->kind !
		if (kind == 105)kind = _relation; //relation
		if (kind == 1)kind = _concept; //noun
		//        if(kind==1)continue;
		if (kind == 103)kind = noun; //noun
		if (kind == 10)kind = noun; //noun
		if (kind == 11)kind = verb; //verb
		if (kind == 12)kind = adjective; //adjective
		if (kind == 13)kind = adverb; //adverb
		//        contextId = atoi(contextId_s);
		//
		//        if (Id < 1000)contextId = wordnet; //wn
		//        if (contextId == 100 && Id > 1000)contextId = 100;
		Node* n;
		if (Id > 1000)
			n = add(name, kind, contextId);
		else
			n = add_force(contextId, Id, name, kind);
		if (n == 0) {
			p("out of memory");
			break;
		}
		wn_map[Id] = n->id;
		wn_map2[n->id] = Id;
	}
	fclose(infile); /* Close the file */
	p("node import ok");
}

void importStatements() {
	FILE *infile;

	char line[1000];
	int linecount = 0;

	/* Open the file.  If NULL is returned there was an error */
	printf("Opening File %s\n", (basepath + statements_file).data());
	if ((infile = fopen((basepath + statements_file).data(), "r")) == NULL) {
		perror("Error opening file");
		exit(1);
	}
	char tokens[1000];
	while (fgets(line, sizeof (line), infile) != NULL) {
		/* Get each line from the infile */
		if (++linecount % 1000 == 0)
			pi(linecount);
		// p(line);
		//	strcpy(  tokens,line);
		//		int x = 0;
		//    while (tokens[x++])
		//        if (tokens[x]==' ')
		//		    tokens[x]='_';
		int contextId;
		int subjectId;
		int predicateId;
		int objectId;
		int id; // ignore now!!
		sscanf(line, "%d\t%d\t%d\t%d", &id, &subjectId, &predicateId, &objectId); // no contextId? cause this db assumes GLOBAL id!
		subjectId = wn_map[subjectId]; //%1000000;//wn!!
		predicateId = predicateId; //%1000000;
		if (predicateId > 100)
			predicateId = wn_map[predicateId]; //%1000000;
		objectId = wn_map[objectId]; //%1000000;
		// printf("%d\t%d\t%d\n",subjectId,predicateId,objectId);
		if (subjectId < 1000 || objectId < 1000 || predicateId == 50 || predicateId == 0 || subjectId == 1043 || subjectId == 1044)continue;
		Statement* s = addStatement4(wordnet, subjectId, predicateId, objectId);
	}
	fclose(infile); /* Close the file */
	p("statements import ok");
}
#ifdef sqlite3
int maxBytes = 1000000;

void importSqlite(char* filename) {
	sqlite3* db;
	sqlite3_stmt *statement;
	const char* unused = (char*) malloc(1000);
	int status = sqlite3_open(filename, &db);
	pi(status);
	status = sqlite3_prepare(db, "select * from nodes;", maxBytes, &statement, &unused);
	pi(status);

	// http://www.sqlite.org/c3ref/step.html
	status = sqlite3_step(statement);
	pi(status);
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
	for (int i = s.length(); i > 1; i--) {
		char c = s[i];
		if (c > 65 && c < 91)s.replace(i, 1, space + (char) tolower(c));
		if (c == '(')s[i - 1] = 0; //cut _( )
		if (c == ',')s[i] = 0; //cut ,_
		if (c == ')')s[i] = 0;
	}
	//    s[0]=tolower(s[0]);
	s = replace_all(s, "_", " ");
	s = replace_all(s, "  ", " ");
	return s;
};

// TODO:!test MEMLEAK!!

const char* parseWikiTitle(char* item, int id = 0, int context = current_context) {
	//    string s="wordnet_raw_material_114596700";
	static string s;
	s = string(item); //"Uncanny_Tales_(Canadian_pulp_magazine)";
	//    South_Taft,_California
	// facts/hasPopulationDensity/ArticleExtractor.txt:205483101	Sapulpa,_Oklahoma	397.1#/km^2	0.9561877303748038


	//    string s="wordnet_yagoActorGeo_1";
	//    wordnet_yagoActorGeo_1
	//    if(s.find("wordnet"))contextId=wordnet;
	//if(s.find("wikicategory")) kind=list / category
	s = replace_all(s, "wikicategory_", "");
	s = replace_all(s, "wordnet_", "");
	s = replace_all(s, "yago_", "");
	s = replace_all(s, "wikicategory", "");
	s = replace_all(s, "wordnet", "");
	s = replace_all(s, "yago", "");
	int last = s.rfind("_");
	int type = s.find("(");
	string clazz = deCamel(s.substr(type + 1, -1));
	string word = s;
	string Id = s.substr(last + 1, -1);
	//    int
	id = atoi(Id.c_str());
	if (id > 0) {
		word = s.substr(0, last);
	}
	//    word=deCamel(word);
	//    item=word.c_str();
	//    ~word;
	return word.c_str(); // TODO!! MEMLEAK!!
}

void extractUnit() {
	string s = "397.1#/km^2";
}

int getFields(char* line, vector<char*>& fields, const char* separator = ",;\t", int nameRowNr = -1, const char* nameRow = 0) {
	char * token;
	token = strtok(line, separator);
	int row = 0;
	while (token != NULL) {
		fields.push_back(token);
		if (nameRowNr < 0) {
			if (nameRow == 0) {
				if (eq("name", token))nameRowNr = row;
				if (eq("Name", token))nameRowNr = row;
				if (eq("title", token))nameRowNr = row;
				if (eq("Title", token))nameRowNr = row;
			} else if (eq(nameRow, token))nameRowNr = row;
		}
		//    printf ("%s\n",pch);
		token = strtok(NULL, separator);
		row++;
	}
	return nameRowNr;
}

void fixNewline(char* line) {
	int len = strlen(line);
	if (line[len - 1] == '\n')
		line[--len] = 0;
	if (line[len - 1] == '\r')
		line[--len] = 0;
}

char* extractTagName(char* line) {
	return match(line, "<([^>]+)>");
}

char* extractTagValue(char* line) {
	return match(line, ">([^<]+)<");
}

bool isNameField(char* field, char* nameField) {
	if (nameField && !eq(field, nameField))return false;
	if (eq(field, nameField))return true;
	if (eq(field, "name"))return true;
	if (eq(field, "title"))return true;
	if (eq(field, "label"))return true;
	return false;
}

Node* namify(Node* node, char* name) {
	Context* context=currentContext();
    node->name = &context->nodeNames[context->currentNameSlot];
    strcpy(node->name, name);// can be freed now!
    int len = strlen(name);
    context->nodeNames[context->currentNameSlot + len] = 0;
	addStatement(getAbstract(name), Instance, node,DONT_CHECK_DUPLICATES);
	// replaceNode(subject,object);
	return node;
}

void addAttributes(Node* subject, char* line) {
	line = (char*) replace_all(line, "\"", "'").c_str();
	do {
		char* attribute = match(line, " ([^=]+)='[^']'");
		char* value = match(line, " [^=]+='([^'])'");
		if (!attribute)break;
		Node* predicate = getThe(attribute);
		Node* object = getThe(value);
		addStatement(subject, predicate, object);
	} while (attribute != 0);
}

bool hasAttribute(char* line) {
	return match(line, " ([^=]+)=");
}

//Context* context,
//Node* type,
// importXml("/Users/me/data/base/geo/geolocations/Orte_und_GeopIps_mit_PLZ.xml","city","ort");
void importXml(const char* facts_file, char* nameField, const char* ignoredFields, const char* includedFields) {
	p("import csv start");
	bool dissect = false;
	char line[1000];
	char* line0= (char*) malloc(sizeof (char*) *100);
	char* field = (char*) malloc(sizeof (char*) *100);
	char* value = (char*) malloc(sizeof (char*) *10000000);

	Node* root=0;
	Node* parent=0;// keep track of 1 layer
	Node* subject=0;
	Node* predicate=0;
	Node* object=0;
	vector<Node*> predicates = *new vector<Node*>();
	map<char*, char*> fields;
	queue<Node*> parents; //constructed!!

//	char* objectName = (char*) malloc(100);
//	int depth = 0;
	//	vector<char*> ignoreFields = splitString(ignoredFields, ",");
	//	vector<char*>& includeFields = splitString(includedFields, ",");
	int linecount = 0;
	FILE *infile;
	printf("Opening XML File %s\n", (facts_file));
	if ((infile = fopen((facts_file), "r")) == NULL)facts_file = (basepath + facts_file).c_str();
	if ((infile = fopen((facts_file), "r")) == NULL) {
		perror("Error opening file");
		exit(1);
	}
	Node* UNKNOWN_OR_EMPTY=getThe("<unknown/>");
	//	map<Node*,Node*> fields;
	while (fgets(line, sizeof (line), infile) != NULL) {
		if (!line)break;
		if (++linecount % 1000 == 0)printf("%d\n", linecount);
		fixNewline(line);
		line0=line;// readable in Debugger!
		if (contains(line, "<?xml"))continue;
		if (!subject) {
			if(root)
				subject = add(extractTagName(line));
			else
				root= add(extractTagName(line));
			continue;
		}

		if (startsWith(line, "</")) {
			//			for(Node* predicate:fields){
			//				object = getThe(extractTagValue(line),null, dissect);
			//				Statement *s = addStatement(subject, predicate, object, false);
			//				showStatement(s);
			//			}
//			if(!parents.empty())
			subject = 0;// parents.back(); // back????
//			parents.pop();
			fields.clear();
			continue;
		}
		// <address> <city> <zip>12345</zip> <name>bukarest</name> </city> </address>
		if (match(line, "<[a-z]") && contains(line, "</")) {
			field = extractTagName(line);
			value = extractTagValue(line);
			if(ignoredFields&&contains(ignoredFields,field))
				continue;
			if (isNameField(field, nameField)) {
//				parent=parents.front();
				// rewrite address.member=city
//				deleteStatement(findStatement(parent, Member, subject, 0, 0, 0, 0));
				// address.CITY=bukarest
				predicate = getThe(subject->name); // CITY
				Node* object = namify(subject, value); // city.name=bukarest
				addStatement(parent, predicate, object,DONT_CHECK_DUPLICATES); // address.CITY=bukarest
				subject = object; // bukarest
//				show(subject,true);
				continue;
			}

			if (!value) {//<address> <city> ...
//				parents.push(subject);
				parent=subject;

//			if(eq(field,"zip")){
//				value=value;
//				showStatement(s);
//			}

				if(!contains(line,"><")){// else empty!
					object = add(field);
					addStatement(subject, Member, object,DONT_CHECK_DUPLICATES);
					subject = object;
				}
				else{
					object=getThe(field);
//					addStatement(subject, Unknown,object,DONT_CHECK_DUPLICATES);//EMPTY
					addStatement(subject, object,Unknown,DONT_CHECK_DUPLICATES);//EMPTY
//					addStatement(subject, object,UNKNOWN_OR_EMPTY,DONT_CHECK_DUPLICATES);//EMPTY
				}
				addAttributes(subject, line);
				continue;
			}
			if (hasAttribute(line)) {
				predicate = add(field); // <zip color='green'>12345</zip>
				addAttributes(predicate, line);
			} else {
				predicate = getThe(field, NO_TYPE, DONT_DISSECT);
			}
			object = getThe(value, NO_TYPE, DONT_DISSECT);
			Statement* s=addStatement(subject, predicate, object,DONT_CHECK_DUPLICATES);

			//			fields.insert(predicate,object);//
			continue;
		}
		if (startsWith(line, "<") && !contains(line, "</")) {
			parent=subject;
//			parents.push(subject); // <address> <city> ...
			field = extractTagName(line);
			subject = add(field); // can be replaced by name!
			addStatement(parent, Member, subject,DONT_CHECK_DUPLICATES); // address.city
			addAttributes(subject, line);
			continue;
		}
	}
	fclose(infile); /* Close the file */
	p("import xml ok ... items imported:");
	pi(linecount);
}

void importCsv(const char* facts_file, Node* type, const char* separator, const char* ignoredFields, const char* includedFields, int nameRowNr, const char* nameRow) {
	p("import csv start");
	char line[1000];
	char** values = (char**) malloc(sizeof (char*) *100);
	memset(values, 0, 100);
	//    vector<char*> values;

	Node* subject=0;
	Node* predicate=0;
	Node* object=0;
	vector<Node*> predicates = *new vector<Node*>();
	vector<char*> ignoreFields = splitString(ignoredFields, ",");
	vector<char*>& includeFields = splitString(includedFields, ",");
	vector<char*>& fields = *new vector<char*>();
	int linecount = 0;
	FILE *infile;
	printf("Opening File %s\n", (facts_file));
	if ((infile = fopen((facts_file), "r")) == NULL)facts_file = (basepath + facts_file).c_str();
	if ((infile = fopen((facts_file), "r")) == NULL) {
		perror("Error opening file");
		exit(1);
	}
	char* objectName = (char*) malloc(100);
	int fieldCount = 0;
	char* columnTitles;
	while (fgets(line, sizeof (line), infile) != NULL) {
		if (!line)break;
		if (linecount == 0) {
			columnTitles = line;
			if (fields.size() == 0)
				nameRowNr = getFields(line, fields, separator, nameRowNr, nameRow);
			fieldCount = fields.size();
			for (int i = 0; i < fieldCount; i++) {
				char* field = fields.at(i);
				predicates.push_back(getThe(field));
			}
			++linecount;
			continue;
		}
		if (++linecount % 100 == 0)printf("%d\n", linecount);

		fixNewline(line);
		//        values.erase(values.begin(),values.end());
		//        ps(line);

		int size = splitStringC(line, values, separator);
		if (fieldCount != size) {
			printf("Warning: fieldCount!=columns in line %d   (%d!=%d)\n%s\n", linecount - 1, fieldCount, size, line);
			//            ps(columnTitles); // only 1st word:
			//            ps(line);// split! :{
			continue;
		}
		bool dissect = type && !eq(type->name, "city"); // city special: too many!
		// todo more generally : don't dissect special names ...

		subject = getThe(values[nameRowNr], null, dissect);
		if (type && subject->kind != type->id) {
			//			p("Found one with different type");
			Node* candidate = subject;
			subject = getThe(values[nameRowNr], type, dissect); // todo : match more or less strictly? EG Hasloh
			addStatement4(type->context, candidate->id, Synonym->id, subject->id, true);
			//			addStatement4(type->context,candidate->id,Unknown->id,subject->id,false);
		}
		//		if(eq(subject->name,"Hasloh"))
		//			linecount++;
		for (int i = 0; i < size; i++) {
			if (i == nameRowNr)continue;
			predicate = predicates[i];
			if (predicate == null)continue;
			if (contains(ignoreFields, predicate->name))continue;
			if (includedFields != null && !contains(includeFields, predicate->name))continue;
			char* vali = values[i];
			if (!vali || strlen(vali) == 0)continue; //HOW *vali<100??
			object = getThe(vali);
			Statement *s = addStatement(subject, predicate, object, false);
			//            showStatement(s);
		}

		if (!subject || !predicate || !object || subject->id > maxNodes || object->id > maxNodes) {
			printf("Quitting import : id > maxNodes\n");
			break;
		}
	}
	fclose(infile); /* Close the file */
	p("import csv ok ... lines imported:");
	pi(linecount);
}

void importList(const char* facts_file, const char* type) {
	p("import list start");
	char line[1000];
	Node* subject = getClass(type);
	Node* object;
	int linecount = 0;
	FILE *infile;
	printf("Opening File %s\n", (facts_file));
	if ((infile = fopen((facts_file), "r")) == NULL) {
		perror("Error opening file");

		exit(1);
	}
	while (fgets(line, sizeof (line), infile) != NULL) {
		if (++linecount % 10000 == 0)printf("%d\n", linecount);
		char* objectName = (char*) malloc(100);
		object = getThe(line);
		addStatement(subject, Instance, object, false);
		if (!subject || !object || subject->id > maxNodes || object->id > maxNodes) {
			printf("Quitting import : id > maxNodes\n");
			break;
		}
	}
	fclose(infile); /* Close the file */
	p("import list ok");
}

bool importFacts(const char* facts_file, const char* predicateName = "population") {
	p("import facts start");
	Node* subject;
	Node* predicate;
	Node* object;
	char line[1000];
	//    char* predicateName=(char*) malloc(100);
	predicate = getClass(predicateName);
	int linecount = 0;
	FILE *infile;
	printf("Opening File %s\n", facts_file);
	if ((infile = fopen(facts_file, "r")) == NULL) {
		perror("Error opening file");
		return false;
		//        exit(1);
	}
	char* objectName = (char*) malloc(100);
	char* subjectName = (char*) malloc(100);
	while (fgets(line, sizeof (line), infile) != NULL) {
		/* Get each line from the infile */
		if (++linecount % 10000 == 0)printf("%d\n", linecount);
		int contextId;
		//	char* predicateName=(char*) malloc(100);
		int subjectId;
		int predicateId;
		int objectId;
		int id; // ignore now!!
		int certainty;

		if (!eq(predicateName, "population"))
			sscanf(line, "%s\t%s", subjectName, objectName); // no contextId? cause this db assumes GLOBAL id!
		else
			sscanf(line, "%d\t%s\t%s\t%d", &id, subjectName, objectName, &certainty); // no contextId? cause this db assumes GLOBAL id!
		// printf(line);
		//	 printf("%d\t%d\t%d\n",subjectId,predicateId,objectId);

		// important words first!!
		//if(contains(subjectName,"_") || contains(subjectName,"-") || contains(subjectName,":") || contains(subjectName,"#"))
		//    continue;
		if (contains(subjectName, "Begriffskl") || contains(subjectName, "Abkürzung") || contains(subjectName, ":") || contains(subjectName, "#"))
			continue;
		if (contains(objectName, "jpg") || contains(objectName, "gif") || contains(objectName, "svg") || contains(objectName, "#") || contains(objectName, ":"))
			continue;

		//     subject=getThe(subjectName);
		subject = getAbstract(subjectName); //

		//    show(subject);
		//    int objectValue=atoi(objectName);
		//    if(eq(predicateName,"population")&&objectValue<1000)continue;

		//if(contains(objectName,",") || contains(objectName,"("))
		//    object=getThe(objectName);
		//    else
		object = getAbstract(objectName);
		//dissectWord(abstract);

		Statement* s;

		if (contains(objectName, subjectName, true))
			s = addStatement(subject, Member, object, false); // todo: id
		else
			s = addStatement(subject, predicate, object, false); // todo: id

		if (!subject || !object || subject->id > maxNodes || object->id > maxNodes) {
			printf("Quitting import : id > maxNodes\n");
			break;
		}

		showStatement(s);
	}
	fclose(infile); /* Close the file */
	p("import facts ok");
	return true;
}

#define a(word) getThe(#word)
#define all(word) getThe(#word)

void importNames() {
	importList((basepath + "FrauenVornamen.txt").data(), "female_firstname");
	importList((basepath + "MaennerVornamen.txt").data(), "male_firstname");
	addStatement(all(firstname), are, a(name));
	addStatement(all(male_firstname), a(gender), a(male));
	addStatement(all(male_firstname), Owner, a(male));
	addStatement(all(female_firstname), a(gender), a(female));
	addStatement(all(female_firstname), Owner, a(female));
}

void importWordnet() {
	importNodes(); // FIRST! Hardlinked ids overwrite everything!!
	importStatements();
}

void importGeoDB() {
	importCsv("/Users/me/data/base/geo/geonames/cities1000.txt",\
			getThe("city"), "\t", "alternatenames,modificationdate,geonameid",\
		"latitude,longitude,population,elevation,countrycode", 2, "asciiname");
}

// IMPORTANT: needs manual collectAbstracts() afterwards (for speed reasons??)

void importAll() {
	//	importFacts()
//	importCsv("adressen.txt");
	importNames();
	importWordnet();
	importGeoDB();
	//    importImages();
}

void importWikipedia() {

}

void import(const char* filename, const char* path0) {
	// todo:
	// if(contains(filename,"/",false))
	// 	basepath=substr(filename,0,(int)(void*)(rindex(filename,"/")-filename));
	basepath = path0;

	clock_t start;
	double diff;
	//  start = clock();
	//  diff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;

	if (eq(filename, "all")) {
		importAll();
	} else if (eq(filename, "csv")) {
		importCsv(path0);
	} else if (eq(filename, "wordnet")) {
		importWordnet();
	} else if (eq(filename, "names")) {
		importNames();
	} else if (eq(filename, "images")) {
		importImages();
	} else if (eq(filename, "wiki")) {
		importWikipedia();
	} else if (eq(filename, "topic")) {
		importWikipedia();
	} else if (eq(filename, "yago")) {
		importFacts(filename, filename);
	} else if (contains(filename, "txt")) {
		importCsv(filename);
	} else if (contains(filename, "csv")) {
		importCsv(filename);
	} else if (contains(filename, "tsv")) {
		importCsv(filename);
	} else if (contains(filename, "xml")) {
		importXml(filename);
	} else if (!importFacts(filename, filename))
		importAll();


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
