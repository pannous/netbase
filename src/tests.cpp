#include <cstdlib> // malloc, exit:
#include <string.h> // strcmp
#include <stdlib.h>
#include <iomanip>

#include "netbase.hpp"
#include "import.hpp"
#include "util.hpp"
#include "relations.hpp"
#include "query.hpp"
#include "console.hpp"
#include "webserver.hpp"
#include "init.hpp"

//#define assert(cond) ((cond)?(0): (fprintf (stderr,"FAILED: \ %s, file %s, line %d \n",#cond,__FILE__,__LINE__), abort()))

#define a(word) getAbstract(#word)
#define _(w) getThe(#w)
//#define the(w) getThe(#w) netbase.hpp

void checkWordnet() {
	if (hasWord("effloresce")) return;
	else importWordnet();
}


void checkWikiData() {
	if (hasWord("hamburg") && eq(get(1055)->name, "hamburg")) return;
	else importWikiData();
}

void checkGeo() {
	if (hasWord("Canillo")) return;
	else importGeoDB();
}

#undef assert

bool assert(bool test, string what) { // bool nix gut
	printf("----\n");
	printf("%s", what.data());
	if (test) printf(" OK\n");
	if (!test) {
		printf(" Failed\n");
		flush();
		if (exitOnFailure != false) exit(1);
	}
	return test;
}


void testNameReuse() {
	N ab = a(TEST);
	check(hasWord("TEST"));
	N h = hasWord("TEST");
	check(h == ab);
	//	check(h->name==ab->name);
	N t = getNew("TEST");
	//	p((long)&ab->name);
	//	p((long)&t->name);
	//	check(&ab->name==&t->name);
	check(ab->name == t->name);
}


void testScanf() {
	//FALSCH!!: A feature with sscanf is that you can define variables inside it:
	//sscanf("foo % 1 2 3 fum","%s %% %{%d%} %s",string s1, array(string) as, string s2);
	char buf[] = "string1 string2 string3";
	char array[100];
	if (sscanf(buf, "%*s%*s%99s", array) == 1) {
		p(array);
	}
	char *as = (char *) malloc(1000);
	sscanf("foo abdf fum", "{%s}", as);
	p(as);

	//ghet nicth
	//string s1;
	//string s2;
	//sscanf("foo % fum","%s %% %s", s1, s2);
	//char* as=(char* )malloc(1000);
	//string as[100];
	//sscanf("foo % 1 2 3 fum","%s %% %{%d%} %s", s1, as, s2);
	//ps(s1);ps(s2);
	//ps(as[1]);

	char a[100000];
	char b[100000];
	char c[100000];
	bool matching; // leider immer !? --
	cchar *match = "abc=er [sdaf=er ] =fe";
	matching = sscanf(match, "%s [%s ] =%s", a, b, c);
	p(matching);
	p(a);
	p(b);
	p(c);
	string x = "a[b=c]=d";
	ps(x);
	//	p(x.replace("["," ["));
	//	exit(1);
}


void testBasics() {
	//you have a pointer to some read-only characters
	cchar *a = "abc";

	char b[] = "abc";
	//you have an element array of characters that you can do what you like with.

	check(wordHash("abc") == wordHash(a));
	check(wordHash("abc") == wordHash(b));
	assert(contains("abcd", "bc"), "contains");
	assert(!contains("abcd", "bd"), "!contains");
	assert(startsWith("abce", "ab"), "startsWith");
	assert(!startsWith("abce", "ac"), "!startsWith");

	check(eq("abce", "abce"));
	check(!eq("abce", "ac"));
	check(!eq("abce", "A"));
	check(!eq("A", "abce"));
	check(!eq("abce", "abc"));
	check(!eq("abc", "abce"));
	check(!eq("abce", "aBcE", false));
	check(!eq("abce", "ac", true));
	check(eq("abce", "aBcE", true));
	check(eq("ABce", "aBcE", true));

	check(contains("abcd", "bc"));
	//  check(contains("abCd","Bc",true));
	check(!contains("abCd", "bd", true));
	check(!contains("abCd", "Bc", false));
	//  assert(!contains("abcd",""),"gr");
	int testContext = 1;
	Context *c = getContext(testContext);
	initRelations();
	Node *syn = &c->nodes[_synonym];
	show(syn);
	check(syn == Synonym);
	p(Synonym->name);
	// does not work in test environment ( only???? ) ... fix!!!
#ifdef inlineName // todo!
	check(eq(Synonym->name, "synonym"));
#endif
	showStatement(getStatementNr(syn, 0));
	//  initContext(c);
	int initialNodeCount = c->nodeCount;
	int initialStatementCount = (int) c->statementCount;
	c->nodeCount = initialNodeCount; // reset for tests! dont save!
	c->statementCount = initialStatementCount;
	Node *aTest = a(test);
	check(hasNode("test"));
	if (aTest->statementCount > 0) {
		//  memset(&c->nodes[initialNodeCount], 0, sizeof (Node) *( maxNodes() - initialNodeCount -1)); //calloc!
		//  memset(c->statements, 0, sizeof (Statement) * maxStatements() - 1);
		//  memset(0,c->statements,maxStatementsPerNode)
		p("deleteNode(a(test))");
		deleteNode(a(test), false);
		check(a(test)->statementCount == 0);
	}


	Node *good = add("good", _adjective, 1);
	Node *is = add("is", _verb, 1);
	Node *test = add("test", _noun, 1);
	addStatement(test, is, a(test));
	check(is != null);
	check(test->statementCount >= 1);
	p(test);
	Statement *pStatement = getStatementNr(test, 0);
	assert(pStatement, "has Statement");
	assertEquals(pStatement->Subject(), a(test));// 0=='noun'
//	check(pStatement == s);// prepended
//	assertEquals(getStatementNr(test, 1)->Subject(), a(test));// 0=='noun'

	showContext(current_context);
	//	c->name="Public";
	if (multipleContexts) assert(c->id == 1, "c.id==1 multipleContexts");
	else assert(c->id == wordnet, "c.id==wordnet"); //wordnet only context

	//	assert(c->name=="Public","c.name==Public");
	assert(c->nodeCount >= initialNodeCount + 3, "c.nodes==3"); //3+abstracts
	assert(eq(name(test), "test"), "name(test)=='test'");
//	assert(eq(c->nodes[initialNodeCount].name, "good"), "c.nodes[1].name==good");
	//	p(c->nodes[3].name);
	//	pi(c->nodes[3].name);
	//if(!loaded)
	//	assert(eq(c->nodes[3].name,null),"c.nodes[3].name==null");
	//  assert(findWord(testContext, "good",true)==good), "find(wordnet,good)==good");
	//  assert(eq(findWord(testContext, "good",true)->name,good), "find(wordnet,good)==good");
	Node *dead = &c->nodes[999];
	// Statement* s=addStatement4(c.id, test->id,is->id,good->id);
	int statementCount = c->statementCount;
	Statement *testIsGood = addStatement(test, is, good, false, true);
	p(c->statementCount);
	check(c->statementCount == statementCount + 1);// if not yet there
	check(testIsGood->subject == test->id);
	check(testIsGood->Subject() == test);
	check(testIsGood->Predicate() == is);
	check(testIsGood->Object() == good);
	p(test);
	check(getStatementNr(test, 1)->Subject() == a(test));
	p(good);
	check(getStatementNr(good, 0)->Subject() == a(good));
	bool explicitlyAddKindAsStatement = 0;
	if (explicitlyAddKindAsStatement) {
		check(getStatementNr(test, 1)->Object() == &c->nodes[(_noun)]);
		check(getStatementNr(good, 1)->Object() == &c->nodes[(_adjective)]);
	}
	p(is);
//	check(getStatementNr(is, 2) == testIsGood);
	check(getStatementNr(good, 1) == testIsGood);
	show(test);
	show(is);
	show(dead);
	statementCount = (int) c->statementCount;
	// sonderf�����lle
	addStatement4(-1, -2, -3, -4);
	assert(c->statementCount == statementCount, "c.statementCount==1");

	//#ifdef inlineName // todo!
	//	Node* instance=the(instance);
	//	Node* instance2=the(instance);
	//	assert(instance == instance2, "instance==instance2"); // it failes when strings are lost
	//#endif
	// todo: Important! keep Context string pool in other .cpp object files!!!!!
	//#define inlineName true no good resolution!
	//	assert(instance==Instance,"instance==Instance");

	// addStatement4(0,0,0,0);
	// addStatement4(rand(),rand(),rand(),rand());
	// check(test,is,good);

}

void testGeoDB() {

	//  if(nodeCount()<100000)
	if (!hasNode("Gehren"))
		importGeoDB();

	das(Gehren);
	show((Gehren));
	N latitude = getProperty(Gehren, "Latitude");//Gehren.Latitude
	p(latitude);
	check(checkNode(latitude));
	p(latitude->value.number);
	check(latitude->value.number == 50.65);
	NV all;
	N city = the(city);
	p(city);
	NodeVector cities = all_instances(city, 2);
	check(cities.size() > 10);

	all = query("city where population=3703");// Gehren
	check(all.size() > 0);
	N population = getProperty(all[0], "population");
	p(population);
	check(population->value.number == 3703);

	all = query("city where elevation=141");
	//	all=query("city where Elevation=141");
	check(all.size() > 0);
	N elevation = getProperty(all[0], "Elevation");
	p(elevation);
	N e141 = the(141);
	p(e141->value.number);
	check(the(141)->value.number == 141.)
	check(elevation->value.number == 141.);
	//	check(e141==elevation);// Stored in ABSTRACT!
	all = query("city where latitude=50.65");// todo: filter backwards!! 50.65<-latitude<-[city?] !
	check(all.size() > 2);
	show(all[2]);
	latitude = getProperty(all[2], "latitude");
	p(latitude);
	p(latitude->value.number);
	//  check(latitude->value.number==50.65);// TOTO FOX

	all = query("all city with countrycode=AD");
	check(all.size() > 0);
	show(all[0]);
	N countrycode = getProperty(all[0], "countrycode");
	check(eq(countrycode->name, "AD"));

	NV all_plural = query("all cities with countrycode=AD");// plural
	check(all_plural.size() > 0);
	check(all[0] == all_plural[0])
	//  all=query("all city with countrycode AD");
	//  check(all.size()>0);
	//    show(all[0]);
	//  countrycode=getProperty(all[0],"countrycode");
	//  check(eq(countrycode->name,"AD"));



}

void testDummyLogic() {
	//	if(!hasWord("testDummy")){
	//  Node* testDummy = add("testDummy", _node);
	//  Node* size = add("size", noun);
	//  Node* funny = add("funny", adjective);
	//  Node* eleven = add("11", number);
	//  Node* testDummy2 = add("testDummy", verb);
	//  Node* beth = add("Beth", _person);
	//  Node* CEO = add("CEO", _person);
	//  Node* manager = add("manager", _person);
	//  Node* karsten = add("karsten", _node);
	//  Node* is = Type; // Kind;// add("is",verb);
	//  Node* cute = add("cute", adjective);
	//  Node* people = add("people", noun);
	//	}else{
	Node *testDummy = the(testDummy);
	Node *size = the(size);
	Node *funny = the(funny);
	Node *eleven = the(11);
	Node *testDummy2 = add("testDummy");
	Node *beth = the(Beth);
	Node *CEO = the(CEO);
	Node *manager = the(manager);
	Node *karsten = the(karsten);
	Node *is = Type; // Kind;// add("is",verb);
	Node *cute = the(cute);
	Node *people = the(people);
	//	}

	learn("testDummy.funny");
	learn("testDummy.size=11");
	// assert(*testDummy.is(funny));
	//  Statement* s1 = addStatement(beth, is, cute);
	Statement *s1 = addStatement(beth, Attribute, cute);
	Statement *s2 = addStatement4(current_context, _person, _plural, people->id);
	Statement *s2a = addStatement4(current_context, _person, _plural, people->id);
	Statement *s2b = addStatement(Person, Plural, people);
	Statement *s2c = addStatement(Person, Plural, people);

	//	assert(s2==s2a,"dont duplicate edges 4");??
	assert(s2c == s2b, "dont duplicate edges");
	//	assert(s2==s2b,"dont duplicate edges");// replace abstract with item!!
	Statement *s3 = addStatement(testDummy, Attribute, funny);
	//  Statement* s4 = addStatement(Person, Instance, karsten);
	Statement *s4 = addStatement(CEO, Instance, karsten);
	check(eq(karsten->name, "karsten"));
	Statement *s4a = addStatement(manager, SubClass, CEO);
	//  Statement* s4a = addStatement(CEO, SuperClass, manager);
	Statement *s4b = addStatement(manager, is, a(worker));
	if (s1 or s2 or s3 or s2a or s4 or s4a or s4b)
		s1 = s2 = s2a = s3 = s4 = s4a = s4b = 0;// as a counter measure against warnings
	check(isA(CEO, manager));
	clearAlgorithmHash();
	check(isA(karsten, CEO));
	clearAlgorithmHash();
	// !!!  todo: Instance -> SuperClass framebreak!
	check(isA(karsten, manager));
	clearAlgorithmHash();
	check(isA(karsten, Person));
	check(isA(karsten, a(worker)));

	showContext(current_context);
	showStatement(s2);

	assert(getContext(current_context)->statementCount >= 3, "current_context->statementCount==2");
	//	assert(getStatementNr(funny,0)==s3,"getStatementNr(funny,0)==s");
	addStatement(testDummy, size, eleven);
	show(testDummy);
	addStatement(testDummy2, Passive, eleven);

	showContext(current_context);
	// show(findMember(testDummy,"funny"));
	check(findMember(beth, "cute"))
	assert(findMember(testDummy, "funny") != null, "findMember(testDummy,funny)");
	assert(findMember(testDummy, "funny") == funny, "findMember(testDummy,funny)");
	Node *t = findMatch(testDummy, "funny");
	assert(t == testDummy, "findMatch(testDummy,funny)");
	t = findMatch(testDummy, "bunny");
	assert(t == 0, "findMatch(testDummy,bunny)");
	assert(findMatch(testDummy, "size=11") == testDummy, "findMember(testDummy,size=11)");
	assert(findMatch(testDummy, "size=12") == 0, "findMember(testDummy,size=12)");
	/* How can this have ever passed, when not using parseFilter ? didn't
	 check(findMember(testDummy, "size>10") == testDummy);
	 check(findMatch(testDummy, "size>10") == testDummy);
	 assert(findMatch(testDummy, "size<12") == testDummy, "findMember(testDummy,size<12)");
	 assert(findMatch(testDummy, "size>=11") == testDummy, "findMember(testDummy,size>=11)");
	 assert(findMatch(testDummy, "size<=11") == testDummy, "findMember(testDummy,size<=11)");
	 */
	// get data
	//	vector<Node*> rows1=query("select * from testDummy where size=11 and testDummy.funny");
	//	Node* n = (Node *)rows1[0];
	p(testDummy);
	clearAlgorithmHash();
	NodeVector testDummys = allInstances(testDummy);
	check(contains(testDummys, testDummy));
	//	check(the(testDummy)==testDummy);
	//	testDummys=all_instances(the(testDummy));
	//	check(contains(testDummys,testDummy));
	testDummys = allInstances(a(testDummy));
	check(contains(testDummys, testDummy));
	NodeVector funnys = filter(testDummys, "funny");
	check(contains(testDummys, testDummy));
	cchar *sql = "select * from testDummy where funny";
	NodeVector list = query(sql);
	//	show((Node *)(list[0]));
	//	show((Node *)list[1]);
	showNodes(list, true);
	assert(contains(list, testDummy), sql);
	//	sql="select * from testDummy funny";
	//	assert(last(query(sql)) == 0,sql);
	sql = "select * from testDummy where size=11";
	list = query(sql);
	assert(contains(list, testDummy), sql);
	sql = "select * from testDummy where size=11 and testDummy.bunny";
	list = query(sql);
	assert(false == contains(list, testDummy), sql);
	sql = "select * from testDummy where size=11 and funny";
	list = query(sql);
	assert(contains(list, testDummy), sql);
	sql = "select * from testDummy where size=11 and testDummy.funny";
	list = query(sql);
	assert(contains(list, testDummy), sql);
	sql = "testDummy that are funny";
	list = query(sql);
	assert(contains(list, testDummy), sql);
	sql = "all testDummys";
	list = query(sql);
	assert(contains(list, testDummy), sql);
	sql = "testDummy where size=11";
	list = query(sql);
	assert(contains(list, testDummy), sql);
	sql = "testDummy with size=11";
	list = query(sql);
	assert(contains(list, testDummy), sql);
	sql = "testDummys with size=11";
	list = query(sql);
	assert(contains(list, testDummy), sql);
	sql = "testDummys with size=11 and testDummy.funny";
	list = query(sql);
	assert(contains(list, testDummy), sql);
	sql = "testDummy with size>10";
	list = query(sql);
	assert(contains(list, testDummy), sql);

	assert(isA(beth, Person), "isA(beth,Person->name)");
	// deprecated:
	//  assert(isA5(beth, Person->name), "isA(beth,Person->name)");
	show(Person);
	assert(has(Person, Plural, people), "has Person,plural,people");

	clearAlgorithmHash();
	assert(isA(Person, people), "isA(get(person),people)");
	clearAlgorithmHash();

	// deprecated:
	assert(isA(beth, people), "isA(beth,people)");
	//  assert(isA(beth,people->name),"isA(beth,people->name)");
	//  assert(!isA(cute,Person),"!isA(cute,Person)");
	clearAlgorithmHash();
	assert(findStatement(cute, Type, Person) == 0, "findStatement(cute,Type,Person)==0");
	addStatement(beth, Type, Person);
	assert(findStatement(cute, Type, Person) == 0, "findStatement(cute,Type,Person)==0 b");
	clearAlgorithmHash();
	vector<Node *> persons = allInstances(Person);
	assert(persons.size() > 0, "all_instances(Person).size()>0");
	assert(contains(persons, beth), "contains(persons,Beth)");
	//  check(contains(people, karsten));
	vector<Node *> managers = allInstances(manager);
	//  todo:
	//  check(contains(managers, karsten));
	//  vector<Node*>

	check(has(beth, Attribute, cute));
	check(findMember(beth, "cute"));
	check(findMatch(beth, "cute"));
	clearAlgorithmHash();
	sql = "select * from person where cute";
	vector<Node *> cuties = query(sql);
	check(contains(cuties, beth));
	check(last(cuties) == beth);
	assert(allInstances(people).size() > 0, "all_instances(people)>0");
	assert(find_all("people", current_context, true).size() > 1, "find_all(people)>1");
	//	check(contains(allInstances(people), beth));
	NodeVector all = find_all("people", current_context, true);
	check(contains(all, beth));
	//  check(isA(beth,cute));

	NodeVector peoples = query("select * from people");
	check(contains(peoples, beth));
	sql = "select * from people where cute";
	all = query(sql);
	check(contains(all, beth));
	check(last(all) == beth);

	persons = find_all("person", current_context, true);
	assert(persons.size() > 1, "find_all(person)>1");
	showNodes(persons); // could be over 100 !!! NOT containing person #35325
	check(contains(persons, beth));

	//  sql = "select * from person where mute";
	//  assert(last(query(sql)) == 0, sql);

	//  Statement* learned=learn("beth is cute");
	//  evaluate("beth is cute");
	/*
	 assert(match("testDummy[funny]")==testDummy,"match('testDummy[funny]')==testDummy");
	 assert(match("testDummy[size=11]")==testDummy,"match('testDummy[size=11]')==testDummy");
	 assert(match("testDummy[size=11,funny]")==testDummy,"match('testDummy[size=11,funny]')==testDummy");
	 assert(match("testDummy[size=11 and funny]")==testDummy,"match('testDummy[size=11,funny]')==testDummy");
	 assert(match("testDummy[size>10]")==testDummy,"match('testDummy[size>10]')==testDummy");
	 assert(match("testDummy[size<12]")==testDummy,"match('testDummy[size<12]')==testDummy");
	 assert(match("testDummy[size>10,funny]")==testDummy,"match('testDummy[size>10,funny]')==testDummy");
	 assert(match("testDummy[size<12,funny]")==testDummy,"match('testDummy[size<12,funny]')==testDummy");
	 assert(match("testDummy[size>10 and funny]")==testDummy,"match('testDummy[size>10,funny]')==testDummy");
	 assert(match("testDummy[size<12 And funny]")==testDummy,"match('testDummy[size<12,funny]')==testDummy");
	 */
	// get data
	//  	find_english("people that are cute");
	//  	find_english("people which are cute");
	//  	find_english("people with blond hair");
	//  	find_english("people without hair");
	//  	find_english("persons which are cute");
	//  	find_english("all persons which are cute");
	//  	find_english("the person that is cute");
	// kinds of x => ?->parent->x | ?->parent->x
	// wie sp�����t=
}

void testWordnet() {
	Context c = *getContext(wordnet);
	//  load();
	//  importAll();
//    checkWordnet();
	checkWikiData();
	showContext(wordnet);
	p("contexts[wordnet].nodeCount:");
	p(c.nodeCount);
	check(hasNode("drag_one's_feet"));
	show(&c.nodes[-128749]);// -38749
	check(strcmp(c.nodes[-128749].name, "Confirm") == 0);
	N bird = a(bird);
//	clearAlgorithmHash(); deprecatred
	check(isA(a(gooney), a(bird)));
	check(isA(a(duck), a(poultry)));
//	clearAlgorithmHash();
	check(isA(the(poultry), a(bird)));
	check(isA(a(poultry), a(bird)));
	check(isA(a(duck), a(bird)));// wow how?
//	check(isA(the(duck), the(bird)));// yeah: the! main concept
//	clearAlgorithmHash();
	//todo:
	assert(isA(a(duck), a(bird)), "duck isA bird"); // yuhu! 2010-02-07
	clearAlgorithmHash();
//	return;
	//	assert(has("duck", "beak"), "has(duck,beak)");// 241531	duck	flesh of a duck  confusion?
	check(has("duck", "tail"));
	assert(has("duck", "head"), "has(duck,head)"); // fails but not in console !?! wtf???
	assert(has("duck", "foot"), "has(duck,foot)");
	addStatement(the(foot), Plural, the(feet));
	assert(has("duck", "feet"), "has(duck,feet)");

	check(has(a(duck), a(feather)));

	addStatement(the(feather), Plural, the(feathers));
	assert(has(get("duck"), get("feathers")), "has(duck,feathers)");
	//  assert(contains(all_instances(a(bird)), the(sea_duck)), "bird has instance duck");

	// int i=0;
	// for(i=0;i<1000;i++)show(&contexts[100].nodes[i]);
	// for(i=0;i<1000;i++)
	// 	showStatement(&contexts[wordnet].statements[i]);
	// for(i=0;i<1000;i++)
	// 	showStatement(&contexts[100].statements[i]);
	// for(i=0;i<1000;i++)
	// 	showStatement(&contexts[0].statements[i]);
	// for(i=0;i<1000000;i++)show(&contexts[0].nodes[i]);
	// for(i=0;i<1000000;i++)show(&c2.nodes[i]);
	// show(&c.nodes[11244]);
	// show(&c.nodes[rand()]);
}


//#define new(word) Node* word=getThe(#word);

void testStringLogic() {
	Node *Schlacht_von_Kleverhamm = getThe(editable("Schlacht_von_Kleverhamm"));
	//  die(Schlacht_von_Kleverhamm);
	eine(Schlacht);
	Node *Kleverhamm = the(Kleverhamm);
	dissectWord(Schlacht_von_Kleverhamm);
	show(Schlacht_von_Kleverhamm);
	show(Kleverhamm);
	check(isA(Schlacht_von_Kleverhamm, Schlacht));
	check(has(Kleverhamm, Schlacht_von_Kleverhamm));
}

void testInstancesAtEnd() {
	Node *t = the(testInstancesAtEnd1);
	Node *p = the(testInstancesAtEndP);
	Node *o = the(testInstancesAtEndO);
	addStatement(t, Type, o, !CHECK_DUPLICATES);
	addStatement(t, Instance, o, !CHECK_DUPLICATES);
	//	S s=addStatement(t, p, o, true);
	S s = addStatement(t, Instance, o, false, false);
	addStatement(t, p, o, false, false);// warning: addStatementToNode skipped
	//	addStatement(t, Type, o, false); types up too
	showStatement(s);
	printf("%d %d\n", t->lastStatement, s->id());
	show(t);
	printf("%p %p\n", getStatement(t->lastStatement), s);
	//	check(t->lastStatement == s->id());
}

void testInsertForceStart() {
	Node *t = the(testInsertForceStart5);
	Node *p = the(testInsertForceStartP);
	Node *o = the(testInsertForceStart0);
	addStatement(t, Instance, o, !CHECK_DUPLICATES);
	addStatement(t, Part, o, !CHECK_DUPLICATES);
	addStatement(t, Type, o, !CHECK_DUPLICATES);
	addStatement(t, Instance, o, false);
	addStatement(t, PartOf, o, !CHECK_DUPLICATES);

	//	S s=addStatement(t, p, o, false);// warning: addStatementToNode skipped
	//	addStatement(t, Type, o, false); types up too
	Statement *first = getStatement(t->firstStatement);
	showStatement(first);
	check(first->Predicate() == Type);
	S s = addStatement(t, p, o, false, true);// checkDuplicate, bool force_insert_at_start
	show(t);
	showStatement(s);
	first = getStatement(t->firstStatement);// new first
	showStatement(first);
	printf("%d %d\n", t->firstStatement, s->id());
	printf("%p %p\n", first, s);
	check(t->firstStatement == s->id());
}

void testStringLogic2() {
	//  NV t= parse("label 4557271 woooot");
	//  check(eq(t[0]->name,"woooot"));// OK
	eine(Schlacht);
	Node *Schlacht_bei_Guinegate2 = getThe(editable("Guinegate_(14791),_Schlacht_bei")); // intellij display bug!
	p(Schlacht_bei_Guinegate2);
	//  deleteNode(Schlacht_bei_Guinegate);
	//  Schlacht_bei_Guinegate=the(Guinegate_(1479),_Schlacht_bei);
	Node *Schlacht_bei_Guinegate = getThe(editable("Schlacht_bei_Guinegate_(14791)"));
	//	deleteNode(Schlacht_bei_Guinegate);
	//	Schlacht_bei_Guinegate=the(Schlacht_bei_Guinegate_(1479));
	dissectWord(Schlacht_bei_Guinegate);
	check(isA(Schlacht_bei_Guinegate, Schlacht));
	check(eq(the(near)->name, "near"));
	show(Schlacht_bei_Guinegate);
	check(findStatement(Schlacht_bei_Guinegate, a(near), a(Guinegate)));
	NV all = instanceFilter(a(Guinegate));
	check(contains(all, the(Guinegate)));
	check(findStatement(Schlacht_bei_Guinegate, _(near), a(Guinegate)));
	check(has(Schlacht_bei_Guinegate, _(near), a(Guinegate)));
	check(has(Schlacht_bei_Guinegate, _(near), the(Guinegate)));
	// TODO !!!
	//  check(has(a(Schlacht_bei_Guinegate), _(near), a(Guinegate)));
	Node *Armagnac_Weinbrand = the(Armagnac_(Weinbrand));
	dissectWord(Armagnac_Weinbrand);
	show(Armagnac_Weinbrand);
	check(isA(word(Armagnac), word(Weinbrand)));
	// todo : not if place:
	//  Heinrich-Heine-Preis_(Stadt_D�����sseldorf)
	// der_<place> von_der/of
	//Kultureller_Ehrenpreis_der_Landeshauptstadt_M�����nchen

	show(getThe(editable("Musyoka,_Kalonzo,")));

}

void testHash() {
	check(wordHash("Stefanie_Zweig") != wordHash("MC_Zwieback"));
	check(wordHash("Stefanie_Zweig") > 0);

	int a = wordHash("Harr");
	int b = wordHash("Harz");
	int c = wordHash("Hart");
	check(a != b != c);

	cchar *thing = "city";
	Node *city = getAbstract(thing);
	ps(city->name);
	check(eq(city->name, "city"));
	//	insertAbstractHash(wordHash(Circa->name), Domain);
	//	insertAbstractHash(Circa);
	//#ifdef inlineName
	//	Node* a2 = getAbstract(Circa->name);
	//	check(a2 == Circa);
	//#endif
}

void testImportContacts() {
	if (!hasWord("Alexandra Neumann")) importCsv("/Users/pannous/data/base/contacts/adressen.txt"); //,","
	check(a(Alexandra_Neumann) == a(Alexandra
			      Neumann));
	check(the(Alexandra_Neumann) == the(Alexandra
			      Neumann));
	N plz = a(Postleitzahl (privat));
	dissectWord(plz);
	check(isA(plz, a(Postleitzahl)));

	check(the(Alexandra
			      Neumann) != null);
	check(the(Alexandra_Neumann) != null);
	show(the(Alexandra
			         Neumann));

	check(has(the(Alexandra_Neumann), plz, a(12167)));
	check(has(the(Alexandra_Neumann), a(Postleitzahl), a(12167)));
	check(has(the(Alexandra_Neumann), pattern(a(Postleitzahl), Greater, a(12166))));

}

void testImportExport() {
	//  show(word(female firstname));
	//  check(isA(word(James), _(male_name)));
	//  check(query("all firstnames starting with 'a'").size() > 0);

	/* NOO dont overwrite nodes.bin ...
	 Context* c=context;
	 int nodeCount=c->nodeCount;
	 c->nodeCount=9999;
	 save();
	 load();
	 check(c->nodeCount==9999);
	 c->nodeCount=nodeCount;
	 save();
	 */
	//  if (!hasWord("male firstname") or !hasWord("female firstname"))
	importNames();
	//	deleteStatements(a(female firstname));
	//	p(a(female firstname));
	//	addStatement(a(female firstname),Instance,the(female firstname),false);// bug hack
	//	addStatement(a(female firstname),Instance,get(489285),false);// bug hack
	//	addStatement(get(9028726),Synonym,get(9025182),false);// bug hack

	check(isA(a(Abdulajewitsch), a(name)));
	check(query("all names", 1000).size() > 10);
	check(allInstances(a(female
			      firstname)).size() > 5); //
	check(allInstances(a(name)).size() > 20);
	check(query("all female firstnames", 10).size() > 5);

	p(a(female
			    firstname));
	p(the(female
			      firstname));
	////  find_all("Jenny");
	//	check(findStatement(a(female firstname),Instance,the(female firstname),0,0,0,0));
	////	exit(1);
	//	check(all_instances(the(female firstname)).size()>5);//

	//	check(all_instances(a("female firstname")).size()>5);

	////	check(all_instances(a(female firstnames)).size()>5);// plural not in system OK HERE
	//	addStatement(a(female firstname),Plural,the(female firstnames),true);
	//	check(all_instances(a(female firstnames)).size()>5);// plural in system OK


	//	addStatement(all(female firstname), a(gender), a(female));
	//	addStatement(all(female firstname), Owner, a(female));
	//	addStatement(all(male firstname), are, a(firstname));
	//	addStatement(all(male firstname), a(gender), a(male));
	//	addStatement(all(firstname), are, a(name));
	check(isA(a(female
			      firstname), a(name)));
	check(isA(a(Ahney), a(female
			      firstname)));
	check(isA(a(Ahney), the(female
			      firstname)));

	clearAlgorithmHash();
	addStatement(a(female
			               firstname), is_a, a(name));
	check(isA(a(Ahney), a(name)));
	clearAlgorithmHash();

	show(a(Zilla)); //51566;
	show(the(Zilla));
	check(isA(a(Zilla), _(female
			      firstname)));
	clearAlgorithmHash();
	addStatement(a(firstname), is_a, a(name));
	check(isA(a(firstname), a(name)));
	clearAlgorithmHash(true);
	check(isA(a(Zilla), a(name)));
	check(isA(a(Zilla), a(female
			      firstname)));
	check(isA(a(female
			      firstname), a(firstname)));
	//  check(isA(a(Zilla), a(first_name)));
	//  check(isA(a(Zilla), _(first_name)));
	clearAlgorithmHash(true);
	check(isA(a(Zilla), a(firstname)));
	//	clearAlgorithmHash(true);
	//  check(isA(a(Zilla), _(firstname)));// OK, not 'THE'
	check(isA(a(Zilla), a(name)));

	clearAlgorithmHash(true);
	check(isA(word(James), _(male
			      firstname)));
	check(isA(word(James), _(name)));

	check(query("all firstnames", 10).size() > 5);
	check(query("all names", 100).size() > 50);

	//	check(isA(a(Zilla), _(name)));
	clearAlgorithmHash();
	testImportContacts();
}

void testImages() {
	the(alabama);
	the(Alabama);
	if (getImage("alabama") == "" or getImage("Alabama") == "") {
//		clearMemory();
//		importAll();
		importImages();
//		import("images");
	}
	p(getImage("Alabama"));
	p(getImage("alabama"));
	show(the(alabama));
	//  check(getImage("wiki_image")=="");//todo!
	check(getImage("alabama") != "");
	check(getImage("Alabama") != "");
	check(getImage("abagsfadd") == "");
}

void testInstanceLogic() {
	// needs addStatementToNodeWithInstanceGap instead of addStatementToNode
	// why is that important? to skip 100000 instances (cities) when accessing abstract properties
	//	deleteWord("test",true);
	//	deleteWord("tester",true);
	//	deleteNode(a(test));
	Node *test2 = getThe("test", Noun);
	check(test2->kind == _noun);
	check(!isA4(test2, Adjective, 6, true)); //semantic, depth 0
	Node *test3 = getThe("test", Adjective); //add("test", adjective);
	check(!isA4(test2, Adjective, 6, true)); //semantic, depth 0
	check(test3->kind == _adjective);
	check(test3 != test2);// type Adjective vs Noun matters
	N abstrac = a(test);
	check(abstrac->kind == _abstract);
	//	check(getStatementNr(a(test), 1)->object==test3->id);
	check(isA4(test3, Adjective)); //semantic, depth 0
	Node *test4 = getThe("test", Adjective);
	p(abstrac);
	check(test3 == test4);
//	deleteNode(test3);// STILL EVIL!
//	deleteNode(test4);
	//	exit(0);//test make!!
	//  Node* aBaum=a(Baum);
	//	deleteWord("tester",true);
	ein(tester);
	show(tester);
	Statement *s = addStatement(tester, the(colour), _(blue));
	addStatement(tester, Instance, the(Ulme));
	show(tester);
	addStatement(tester, the(colour), _(green));
	addStatement(tester, the(colour), _(pink));
	show(tester);
	Statement *c = getStatementNr(tester, 1);
	check(c->Predicate() != Instance);
	c = getStatementNr(tester, 2);
//	check(c->Predicate() != Instance);
	deleteStatement(s);
}


void testValueLogic() {
	//  deleteStatements(a(Booot));
	//  deleteStatements(get(271156));
	//  deleteNode(get(421153));
	//  deleteNode(the(Booot));
	deleteNode(a(Booot), false);
	deleteNode(a("14.32 meter"), false);
	ein(Booot);
	check(eq(Booot->name, "Booot"));
	NodeVector alle = allInstances(Booot);
	//  check(contains(alle, Booot)); // Nee, all_classes ja, all_instances nicht. oder?
	Node *m14 = value("", 14, "m");
	Node *m15 = value("", 15, "m");
	Node *mm14 = value("", 14000, "mm");
	Node *mm15 = value("15000", 15000, "mm");
	Node *m143 = value("14.3", 14.3, "meter");
	Node *m1430 = value("", 14.30, "meter");
	//	Node* m1430=value("14.30", 14.30, "meter");
	Node *m1433 = value("14.330", 14.330, "meter");
	Node *m1433_duplicate = value("14.330", 14.330, "meter");
	check(m1433_duplicate == m1433);
	check(m1430->value.number == 14.3);

	Statement *boot_length = addStatement(Booot, a(length), m143);
	countInstances(m143);
	show(m143);
	Statement *boot_length2 = addStatement(Booot, a(length), m143);
	check(boot_length == boot_length2); // duplicate

	show(Booot);
	check(Booot->statementCount >= 2);
//	trace();
	check(findStatement(Booot, a(length), m143, 1, 1, 1));
	check(findStatement(Booot, a(length), m143, 0, 0, 0));

	show(m14);
	show(m1433);
	show(Booot);
	check(isGreater(m15, m14));
	check(isGreater(m143, m14));
	check(isGreater(m15, m1430));
	check(!isLess(m15, m14));
	check(!isLess(m15, m1430));
	check(isLess(m14, m15));
	check(isLess(m14, m143));
	check(isLess(m1430, m15));
	check(!isGreater(m1430, m15));
	check(isEqual(m143, m1430));
	check(isAproxymately(m1433, m1430));
	bool convert = false;// not yet
	if (convert) {
		check(isEqual(m14, mm14));
		check(isEqual(m15, mm15));
	}
	//  check(isA(m14,m143));
	//  check(isA(m14,m1432));
	check(isA(m143, m1430));

	//  check(isA4(m14,m1432));
	check(isA4(m143, m1430));
	//  check(isA4(m14,m143));//??

	//  getA("length")
	show(Booot);
	show(m1430);
	p(m1430->value.number);
	//  p(m1430->value.datetime);

	check(has(Booot, the(length), m1430));
	check(has(Booot, a(length), m1430));

	//  check(has(Booot,a(length),matcher(Equals,m143)));
	//  check(has(Booot,a(length),matcher(Equals,m1432)));
	//  check(has(Booot,a(length),matcher(Greater,m14)));
	//  check(has(Booot,a(length),matcher(Less,m15)));
	//  check(has(Booot,a(length),matcher(Greater,mm14)));
	//  check(has(Booot,a(length),matcher(Less,mm15)));
	//  check(has(Booot,a(length),m14));

	Node *kind = the(kind);
	Node *length = the(length);
	check(eq(length->name, "length")); //60350
	clearAlgorithmHash();
	show(length);
	clearAlgorithmHash();

	check(!isA4(length, a(category)));
	check(!isA4(a(category), length));
	check(!isA4(get(60350), length)); //category
	check(!isA4(kind, length, 0, 0));
	check(!isA4(kind, length, 8, 1, 1));
	clearAlgorithmHash();
	S nos = findStatement(kind, SuperClass, length, 1, 1, false);
	showStatement(nos);
	check(!findStatement(kind, SuperClass, length, 1, 1, false));
	newQuery();
	check(!has(kind, SuperClass, length, 1, 1, false));
	check(!isA4(kind, length, true, true));
	//  show(m143);
	newQuery();
	Statement *s = findStatement(Booot, a(length), Any);
	check(checkStatement(s));
	Node *n2 = s->Object();
	check(isEqual(n2, m143));
	check(n2 == m143);
	Statement *s2 = findStatement(Booot, the(length), Any, 1, 1, 1);
	n2 = s2->Object();
	check(n2 == m143);
	//  show(m143); //  417287
	//  show(has(Booot, a(length)));
	Node *n = has(Booot, the(length));
	check(eq(n, m143));
	//  show(n); // 413730
	//  check(has(Booot,the(length))==m143);
	check(eq(has(Booot, a(length)), m143));
	check(eq(has(Booot, the(length)), m143));
	//  show(has(Booot, a(length)));
	showStatement(findStatement(Booot, a(length), Any));
	check(!isA4(the(length), the(kind)));
	check(!isA4(get(74), get(33)));
	check(eq(has(Booot, a(length), Any), m143));
	check(eq(has(Booot, a(length)), m143));

	//  check(has(Booot,a(length))==m143);

	check(has(Booot, pattern(a(length), Equals, m143)));
	check(has(Booot, pattern(the(length), Equals, m143)));
	check(has(Booot, pattern(a(length), Equals, m1430)));

	clearAlgorithmHash();
	check(has(Booot, pattern(a(length), Greater, m14))); // TODO CLEAR PATTERNS!!!
	check(has(Booot, pattern(a(length), Less, m15)));
	//check(has(Booot,pattern(a(length),Greater,mm14)));// unit transformation
	//check(has(Booot,pattern(a(length),Less,mm15)));
}

Statement *andStatement(Statement *s1, Statement *s2) {
	return addStatement(reify(s1), And, reify(s2));
}

Statement *orStatement(Statement *s1, Statement *s2) {
	return addStatement(reify(s1), Or, reify(s2));
}

void testValueQuery() {
	deleteNode(the(Booot), false);
	deleteNode(a(Booot), false);
	das(Booot);
	Node *m14 = value("", 14, "m");
	Node *m15 = value("", 15, "m");
	Node *m143 = value("14.320", 14.32, "meter");
	show(Booot);
	check(isA4(m14, the(14 m)));
	//  check(isA4(m14, the(14 meter)));
	addStatement(Booot, a(length), m143);
	check(findStatement(Booot, a(length), m143));
	Query q;
	q.keyword = Booot;
	q.autoFacet = true;
	//  q.fields.push_back(the(length));
	q.fields.push_back(a(length));
	Statement *s1 = pattern(a(length), Greater, m14);
	Statement *s2 = pattern(a(length), Less, m15);
	Statement *s3 = andStatement(s1, s2);
	Statement *s4 = pattern(a(length), Less, m14);
	Statement *s5 = orStatement(s3, s4);
	//  q.filters.push_back(s4);
	//  ps(query(q));
	//  check(!contains(q.instances, Booot));
	empty(q.filters);

	ps(query(q));
	check(contains(q.instances, Booot));
	check(has(Booot, s1));
	q.filters.push_back(s1);
	ps(query(q));
	check(contains(q.instances, Booot));
	q.filters.push_back(s2);
	ps(query(q));
	check(contains(q.instances, Booot));
	q.filters.push_back(s3);
	ps(query(q));
	check(contains(q.instances, Booot));
	q.filters.push_back(s5);
	ps(query(q));
	check(contains(q.instances, Booot));
	q.filters.push_back(s4);
	ps(query(q));
	check(!contains(q.instances, Booot));
}

void testPropertyQuery() {
	if (!hasWord("Sheberghan"))
		importGeoDB();

	//	show(the(Sheberghan));
	//  check(has(the(Sheberghan),the(population),the(55641)));// ?

	//	show(the(Samangan));
	//  check(has(the(Samangan),the(population),the(47823)));//960?
	check(has(a(Samangan), the(population), the(47823))); //960?
	// todo match Wordnet Samangan with geoname Samangan in importCsv!

	//  show(the(city));
	countInstances(the(city));
	Node *hasloh = the(Hasloh); // todo match addressbook Hasloh with geoname Hasloh in importCsv!
	show(hasloh);
	//  check(eq(the("Hasloh"),the(Hasloh)))
	//  check(has(the(Hasloh),the(population),the(3460)));// SLOW!!

	//  check(!areAll(the("latitude"),the(population)));
	Node *n = parseValue("3.4 mg");
	check(n->value.number == 3.4);
	//  has(n,Unit,a("mg"));
	//  check(isEqual(n,parseValue("3400 ��g"))

	//  Node *pp=has(the(Hasloh),the(population), Any);
	//  Node *p=has(the(Hasloh),the(population));
	//  show(p);
	return;// todo:
}

void testComparisonQuery() {
	int limit = 10;
	NodeVector nv;
	Query q;
	q.instances = allInstances(the(city));
	q.limit = limit;
	q.lookuplimit = limit * 2;
	nv = filter(q, pattern(the(population), Less, the(1200)));
	p(nv.size());
	check(nv.size() > 10);
	check(atoi(getProperty(nv[0], "population")->name) < 1200);

	nv = filter(q, pattern(the(population), Greater, the(1300)));
	p(nv.size());
	check(nv.size() > 10);
	check(atoi(getProperty(nv[0], "population")->name) > 1300);

	q.instances = all_instances(the(city), true, lookupLimit, false); // again??
	q.limit = 10;
	//	check(q.instances.size() >= defaultLookupLimit);
	p(q.instances.size());

	//	q.instances = all_instances(get(35329),true,defaultLookupLimit,false);// again??
	//	check(contains(q.instances ,get(708772)));
	//	show(get(708772));
	//	check(contains(q.instances ,get(708772)));

	//	Statement* filter1 = pattern(the(population), Equals, a(1140));
	Statement *filter1 = pattern(a(population), Equals, a(1140));
	q.semantic = true;
	nv = filter(q, filter1);
	p(nv.size());
	check(nv.size() >= 1);
	check(atoi(getProperty(nv[0], "population")->name) == 1140);
	p(nv[0]);

	q.limit = 1000;
	Statement *and_filter = andStatement(pattern(the(population), Less, the(1141)),
	                                     pattern(the(population), More, the(1139)));
	nv = filter(q, and_filter);
	p(nv.size());
	check(nv.size() >= 1);

	and_filter = andStatement(pattern(the(population), Less, the(1200)), pattern(the(population), More, the(1100)));
	nv = filter(q, and_filter);
	p(nv.size());
	check(nv.size() > 0);

	and_filter = andStatement(pattern(the(population), More, the(1139)), pattern(the(population), Less, the(1141)));
	nv = filter(q, and_filter);
	p(nv.size());
	check(nv.size() > 0);

}

void testComparisonQueryString() {
	Query q;
	NV nv;
	q = parseQuery("city where population=1140", 100);
	q.lookuplimit = 10000;
	nv = query(q);
	check(nv.size() > 0);
	p(nv[0]);
	N population = getProperty(nv[0], "population");
	p(population);
	check(eq(population->name, "1140"));
	check(atoi(population->name) == 1140);
	//	check(population->value.number==1140);
	//	showNodes(q.instances, false);
}

void testComparisonQueryString2() {
	Query q = parseQuery("city where countrycode=de", 100);
	NV nv = query(q);
	check(nv.size() > 0);
	N countrycode = getProperty(nv[0], "countrycode");
	check(eq(countrycode->name, "de"));
}

void testQueryAnd() {
	Query q = parseQuery("city where countrycode=\"us\" and population<2000", 100);
	NV nv = query(q);
	check(nv.size() > 0);
	N countrycode = getProperty(nv[0], "countrycode");
	N population = getProperty(nv[0], "population");
	p(nv[0]);
	check(eq(countrycode->name, "us"));
	check(atoi(population->name) > 0 and atoi(population->name) < 2000);
}

void testQueryMore() {
	Query q = parseQuery("city where population>10000000", 100);
	S filterTree = q.filters[0];
	N node = get(657649);
	N ok = has(node, filterTree, q.recursion, q.semantic, false, q.predicatesemantic);
	p(ok);
	check(!ok);
	q.limit = 1;
	NV nv = query(q);
	check(nv.size() > 0);
	N population = getProperty(nv[0], "population");
	p(nv[0]);
	check(atoi(population->name) > 0 and atoi(population->name) > 10000000);
}

void testComparisonQueryStringLess() {
	Query q;
	NV nv;
	string s = "select * from city where population<1100";
	q = parseQuery(s);
	check(eq(q.keyword->name, "city"));
	check(q.filters.size() == 1);
	check(checkStatement(q.filters[0]));
	nv = query(q);
	check(nv.size() > 0);
	p(nv[0]);
	N population = getProperty(nv[0], "population");
	p(population);
	check(atoi(population->name) > 0 and atoi(population->name) < 1100);
}

void testSelectQuery() {
	query2("select population from city limit 1000");
	//	query2("select population from city where population=400914");// limit 10 where population=400914
	query2("select population from city where population>20000 and countrycode=us");

	//		query2("select population,latitude from city");
}

void testFacets() {
	query2("select population from city limit 1000");
}

void testQueryInHandler() {
	handle(":learn a.b=c");
	NV cs = parse(":query a.b", false, false);
	show(cs);
	check(cs[0])
}

void testQuery() {
	testQueryInHandler();
	testComparisonQuery();
	testQueryAnd();
	testComparisonQueryString2();
	testComparisonQueryStringLess();
	testPropertyQuery();
	testSelectQuery();
	testFacets();
	//	testQueryMore();
}

void testWinsOK(){
	const char *iphoneX = "Apple_iPhone_X_256_GB_silber";
	N iPhoneX=getThe(iphoneX);
	check(the(iphone_se)->statementCount>10);
	check(a(iphone+se)==a(iphone_se))
	a(iphone+se)->value.node = 0;// reset 'The' WTF bug<<<<
	check(the(iphone+se)->statementCount>10);
	check(parseProperty("iphone+se.vorgänger"));

	check(parseProperty("Apple_iPhone_X_256_GB_silber.farbe"));

	N aClass = getClass(getThe("tx_tcwins_keyfacts_rel-uid-10941"));
	N aClass1 = getClass(the(DSL-RAM));
	check(aClass==aClass1);
	check(aClass!=Abstract);
	check(aClass==Attribute);
	parse(":class DSL-RAM");

	N found=parseProperty("vorgänger.iphone+se");
	check(parseProperty("vorgänger.iphone+se"));

//	handle("/short/query/iphone+se.vorgänger");
//	handle("/short/query/vorgänger.iphone+se");
//	findProperty("vorgänger.iphone+se")

	Node *zoom = findProperty(iPhoneX, "zoom");
	check(zoom and zoom->value.number==6);

	check(parseProperty("iPhone_X.farbe"));


	check(parseProperty("Apple_iPhone_X.farbe"));
	check(findProperty(the(Apple_iPhone_X),the(farbe)))
	check(findProperty(a(Apple_iPhone_X),the(farbe)))
//	check(findProperty(the(Apple_iPhone_X),a(farbe)))
//	check(findProperty(a(Apple_iPhone_X),a(farbe)))


	check(parseProperty("Apple_iPhone_X_256_GB_silber.ean"))
	check(findProperty(the(Apple_iPhone_X_256_GB_silber),the(ean)))
	check(findProperty(a(Apple_iPhone_X_256_GB_silber),the(Ean)))
	check(findProperty(a(Apple_iPhone_X),the(Ean)))
//	check(findProperty(the(Apple_iPhone_X),the(Ean)))

	newQuery();
//	check(parseProperty("Apple_iPhone_X.Ean"));

}

void testWins() {
	germanLabels=1;
	autoDissectAbstracts=true;
	if(!hasNode("iphone_x")){
		import("wins_all.n3");
		import("wins_smartpones.n3");
		import("DiAS.n3");
	}
//	import("test.n3");

	check(hasNode("blau (sapphire-blue)"));
	p(the("blau (sapphire-blue)")->id);
	check(isA(the("blau (sapphire-blue)"),the(blau)));
	check(isA(the("blau (sapphire-blue)"),the(sapphire-blue)));
	N typ = getType(the(silber));
	p(typ);
	check(eq(typ, Value))
	N aClass = getClass(the(silber), true, typ);
	p(aClass);
	check(eq(aClass, the(Farbe)));
//	check(eq(aClass, the(Key:8313)));
//	check(eq(aClass, a(Key:8313)));

//	N SAR = getThe("tx tcwins keyfacts rel-uid-8413");
//	N SAR = getThe("Key:8413");

//	dissectWord("a_b/c");

	N farbe=a(Farbe);// a(Key:8313);
	check(eq(getClass(the(blau)),farbe));
	check(eq(getID(the(farbe)),"Key:8313"));// Farbe

	p(getID(the(blau)));

	check(eq(getID(the(blau)),"Key:8313"));// Farbe
	check(eq(getID(the(SAR Wert)),"Key:8413"));
	check(eq(getID(the(Strahlung)),"Key:8413"));

	if(!hasNode("Apple_iPhone_X"))
		import("wins_all.n3");
//	testWinsOK();
//	check(findProperty(a(Apple_iPhone_X),the(Ean)));
	N breite=normEntity(getThe("breite"));
	check(!eq(breite->name,"breite"))
	check(autoDissectAbstracts)
	dissectWord(the(Aluminiumgehäuse in silber));
	check(has(the(Aluminiumgehäuse in silber),the(silber)))
	check(isA(the(Aluminiumgehäuse in silber),the(Aluminiumgehäuse)))

	check(parseProperty("iPhone+X+256+GB+silber.Abmessungen"));
	check(parseProperty("iPhone+X+256+GB+silber.breite"));
	check(parseProperty("iphone+x.breite"));
	check(parseProperty("breite.iphone+x"));

//	check(parseProperty("Apple_iPhone_X.farbe"));
//	check(parseProperty("iPhone+X+256+GB+silber.farbe"));
	check(parseProperty("Farbe.iPhone+X+256+GB+silber"));

	Node *ean = a(EAN_(European_Article_Number));
	dissectWord(ean);
	Node *EAN = the(EAN);
//	Node *Ean = findProperty(ean, Label);
	Node *Ean = findProperty(EAN, Label);
	check(Ean);
		p(Ean);
	check(ean==Ean)
//	check(ean==the(EAN));

//	Query aquery = parseQuery("iphone > 3 gb");
//	NV results=query(aquery);
	//	check(results.size()>0);

//	testWinsOK();
}

//#define sn showNode

void testReification() {
	Statement *p = pattern(_(karsten), Attribute, _(cool));
	Node *re = reify(p);
	show(re);
	show(get(_statement));
	check(isA(re, get(_statement)));
	//	check(isA(re,Pattern));
	//	check(isA(re,_(pattern)));
}


void testDelete() {
	Statement *s;
	Node *t = the(testDelete);
	Node *p = the(testDelete1);
	Node *o = the(testDelete2);
	addStatement(t, Instance, o, !CHECK_DUPLICATES);
	s = addStatement(t, Part, p, !CHECK_DUPLICATES);
	addStatement(t, Type, o, !CHECK_DUPLICATES);
	addStatement(t, Instance, o, false);
	addStatement(t, PartOf, o, !CHECK_DUPLICATES);
	int statementCount0 = t->statementCount;
	deleteNode(p->id);
	check(t->statementCount == statementCount0 - 1);
	showNode(t);
	return;

	// part 2
	N P = the(Peter);
	N aP = a(Peter);
	int statementCount = aP->statementCount;
//	show(P);
	show(aP);
	s = learn("Peter loves Jule21");
	check(aP == a(Peter));
	check(aP != P);
	check(aP != s->Subject());
	check(P == s->Subject());
	check(aP->statementCount == statementCount);// the instance was there before
	show(P);
	deleteNode(the(Peter), false);
	check(aP == a(Peter));
	check(aP->statementCount == statementCount - 1);// instance
	check(P->firstStatement == 0);
	check(P->name == 0);
	//	check(P->id!=0); HOLE!
	learn("dummy loves Jule");
//	Statement* s2=learn("Peter loves dummy");
	Statement *s2 = learn("Peter loves Jule21");// knows from abstract!
	N P2 = the(Peter);
	check(s != s2);
	check(P != P2);
	showNode(P2);
	show(the(Jule));
}

void testFactLearning() {
	Statement *s = learn("Peter loves Jule");
	Statement *s2 = learn("Peter loves Jule");
	check(s->Subject() == the(Peter) or s->Subject() == a(Peter));
	check(s == s2);
	p(s);
	check(isA(s->Predicate(), a(loves)));
	check(s->Object() == the(Jule) or s->Object() == a(Jule));
	check(has(a(Peter), a(loves), a(Jule)));
	p(the(Peter));
	check(has(the(Peter), a(loves), a(Jule)));
	//	check(has(the(Peter),a(loves),the(Jule))); how could he? todo : NO, but MAYBE!
	addStatement(the(love), Plural, the(loves)); //the(tense) ??
	//	check(has(the(Peter), a(love), the(Jule), 1, 1, 1));// todo

	addStatement(the(german_translation), is_a, Translation, true);
	addStatement(the(son), the(german_translation), the(Sohn), true);
	//	check(has(the(Peter),a(sons),the(Milan)));
	s = learn("Peter.son=Milan");
	check(s->Subject() == the(Peter));
	check(isA(s->Predicate(), a(son)));
	check(s->Object() == the(Milan));
	check(has(the(Peter), a(son), the(Milan)));
	addStatement(a(son), Synonym, a(cadet));
	check(has(the(Peter), a(cadet), the(Milan)));
	addStatement(a(son), Translation, a(Sohn));// not transitive
	check(has(the(Peter), a(Sohn), the(Milan)));
	addStatement(a(Sohn), Synonym, a(Sohnemann));// Translation not transitive
	//	check(has(the(Peter),a(Sohnemann),the(Milan)));
	p(the(Milan));
	check(isA(the(Milan), a(son)));
	check(isA(a(Milan), a(son)));// erst recht!
}

void testPaths() {
	checkWordnet();
	check(has(a(man), a(hand)));
	NodeVector path = memberPath(a(human), a(hand));
	check(path.size() > 2);
	check(has(a(man), Part, a(hand)));
	//	check(has(a(human),Member, a(hand)));// WTH wordnet!
	//	check(has(a(human),a(hand)));
	check(has(a(bird), a(feather)));
	check(!memberPath(a(animal), a(body)).empty());
	check(has(a(animal), a(body)));
	check(has(a(animal), a(foot)));
	check(!memberPath(a(mouse), a(foot)).empty());
	check(has(a(mouse), a(foot)));
}

void testCities() {
//	clearMemory();
	if (!hasWord("Mersing")) {
		importGeoDB();
	}
	NV ns=query("Mersing.population");
//	check(len<Node*>(ns) > 0);
	check(ns.size() > 0);
	p(the(Mersing));
	N pop = getProperty(the(Mersing), a(population));
	check(pop == the(22007));
	check(eq(pop->name ,"22007"));
	check(findStatement(the(Mersing), a(population), Any));
	check(findStatement(the(Mersing), a(population), the(22007)));
	check(has(the(Mersing), a(population), the(22007)))
	check(has(the(Mersing), a(population)))
	check(!has(the(Mersing), a(population), the(22008)))
	check(has(the(Mersing), value("population", 22007)))// todo
//	check(!has(the(Mersing), value("population", 22008)))// todo
}

void testSplit() {
	char **splat = splitStringC("a.a", ".");
	char *thing = splat[0];
	char *property = splat[1];
	check(eq(thing, property));
}

void testOpposite() {
	checkWordnet();
	check(has(a(good), Antonym, Any));

	check(!findStatement(the(evil), Instance, Synonym, 6, 1, 0, 0));
	//	check(!findStatement(the(evil), Instance,Synonym ,1, 1, 0,1));

	check(!has(the(evil), Instance, Synonym, 1, 1, false)); // ARGH!!!!
	check(!isA4(Synonym, the(evil), 1, 1));
	check(has(the(evil), the(opposite)));
	check(!isA4(Synonym, the(evil)));
	check(!has(the(opposite), the(evil)));
	//	check(!);
	//	check(!isA(,a(derived)));

	p(Antonym);
	check(the(antonym) == Antonym);
	Node *anto = the(Antonym);
	check(anto == Antonym);
	checkWordnet();
	Node *opposite0 = a(opposite);
	Node *the_opposite0 = getThe(opposite0);
	Node *the_opposite0a = the(opposite);
	check(the_opposite0a == the_opposite0);
	Node *the_opposite0b = getThe(the_opposite0);
	p(the_opposite0);
	p(the_opposite0b);
	check(the_opposite0b == the_opposite0);
	Node *the_opposite0c = getThe(opposite0);
	check(the_opposite0c == the_opposite0);
	//	checkWordnet();// messes with abstract !!!
	Node *opposite = a(opposite);
	Node *the_opposite = the(opposite);
	Node *the_oppositea = getThe(opposite);
	Node *the_oppositeb = getThe(the_opposite);
	p(opposite);
	p(opposite0);
	check(opposite == opposite0);
	check(the_opposite == the_opposite0);
	check(the_oppositea == the_opposite0);
	check(the_opposite == the_opposite0a);
	check(the_opposite == the_opposite0b);
	check(the_oppositeb == the_opposite0);
	p(opposite);
	p(the_opposite);

	Node *property = the_opposite;
	Node *propertyA = opposite;
	Node *node = the(good);
	Node *nodeA = a(good);
	clearAlgorithmHash();
	findStatement(node, property, Any, 3, true, 0, true);
	has(node, property);
	//		check(isA4(pa,Antonym, false, false));
	//		check(isA4(Antonym,pa, false, false));
	check(has(node, property));

//	Statement* s=
	addStatement(Antonym, Synonym, opposite);
	addStatement(Antonym, Synonym, the_opposite);
	//		s=addStatement(opposite,Synonym,Antonym,false);
	//		s=addStatement(the_opposite,Synonym,Antonym,false);

	//	p(the(antonym));
	//	check(the(antonym)==Antonym); no, wordnet now!!

	//	check(a(Synonym)==Synonym); NAH
	//	check(a(synonym)==Synonym); NAH

	clearAlgorithmHash(true);
	p(opposite);
	p(the_opposite);
	//30   antonym
	//<786073>    antonym     synonym     opposite        30->21->103493
	//489284 opposite
	//<786078>    opposite        instance        opposite        103493->4->489284
	check(isA(Antonym, opposite)); //OK
	check(isA(opposite, Antonym));
	check(isA(Antonym, the_opposite)); //OK
	check(isA(the_opposite, Antonym));
	//		p(Antonym);
	clearAlgorithmHash(true);
	p(opposite);
	check(isA4(Antonym, opposite, false, true));
	check(isA4(opposite, Antonym, false, true));
	p(the_opposite);
	check(isA4(Antonym, the_opposite, false, true));
	check(isA4(the_opposite, Antonym, false, true));
	//	check(isA4(Antonym,a(opposite),false,false));// has to check Synonym Statement
	//	check(isA4(a(opposite),Antonym,false,false));
	check(has(a(good), Antonym));
	N evil = has(a(good), Antonym);
	p(evil);
	//	check(has(the(good),Antonym));
	check(has(a(good), Antonym, a(evil)));
	//	addStatement(Antonym,Synonym,a(opposite));
	//	addStatement(Antonym,Synonym,the(opposite));
	//	addStatement(the(opposite),Synonym,Antonym);
	check(has(the(good), Antonym, a(bad)));

	check(has(nodeA, property));
	check(has(node, propertyA));
	check(has(nodeA, propertyA));
	//check(has(Any, property, node));
	//check(has(Any, propertyA, node));
	//check(has(Any, property, nodeA));
	//check(has(Any, propertyA, nodeA));
	//check(has(nodeA, propertyA,Any,true,true,true,true));

	check(has(the(good), a(opposite), a(bad)));
	check(has(a(good), the(opposite), a(bad)));
	check(has(a(good), a(opposite), a(bad)));
	// TODO:
	//	check(has(the(good),a(opposite),a(evil)));
	//	check(has(a(good),the(opposite),a(evil)));
	//	check(has(a(good),a(opposite),a(evil)));
	p("opposite test OK");
	//	learn("Antonym Synonym opposite2");// needs checkWordnet
}

void testYago() {
	//	parentPath(a(insect),a(bug));// reverse 1 level !!!
	//	parentPath(a(bug),a(insect));
	//	parentPath(a(depeche_mode),a(Depeche_Mode));
	//	parentPath(a(Banco_de_Gaia),a(group));
}

void testFreebase() {
	N m = getThe("Madonna", More);
	check(m->statementCount > 100);
	m = getThe(a(Madonna), More);
	check(m->statementCount > 100);
	NV all = findProperties("Carlos Barbot", "type");
	check(all.size() > 0);
	showNodes(all);
}

void fixNullNames() {
	p("fixNullNames");
	context = getContext(current_context);
	for (int i = 1; i < context->statementCount; i++) {
		S s = &context->statements[i];
		if (s->predicate == Instance->id) {
			N su = s->Subject();
			N o = s->Object();
			if (su->name == null)
				su->name = o->name;
		}
	}
}


void fixNames() {
	fixNullNames();
	p("fixNames like \"@ \" .");
	char *x = name_root;
	while (x < name_root + maxNodes * averageNameLength - 10000) {
		int l = (int) strlen(x);
		if (contains(x, "\"@")) {
			p(x);
			strstr(x, "\"@")[0] = 0;
		}
		if (contains(x, "\" .")) {
			p(x);
			strstr(x, "\" .")[0] = 0;
		}
		x = x + l + 1;
	}
}

void testSqlDe() {
	cchar *sql = "Karin with Rufnummer";
	//  char* sql="select Synonym from dogs where anhinga";
	NodeVector r = query(sql);
	check(r.size() >= 1);
	p(r[0]->name);
	check(eq(r[0]->name, "Karin"));
}

void testSqlDe2() {
	//	char* sql="select Klasse from dogs where Doenges";
	//	char* sql = "select Kette from dogs where Tock";
	//  	char* sql = "select Klasse from dogs where Schlangenhalsvögel";
	//    	char* sql = "select Kontext from frau where Anrede"; // 2
	//      	char* sql = "select Kontext from frau where Eva";
	cchar *sql = "select Klasse from grau where Anthrazit";
	//  char* sql="select Synonym from dogs where anhinga";
	NodeVector r = query(sql);
	check(r.size() == 1);
	p(r[0]->name);
	check(eq(r[0]->name, "Achromatische farbe"));

}

void old_to_remove() {

	//  load(true);
	//  save();
	//  export_csv();
	//  setLabel(get("surname"), "Nachname");
	//  parse("surname");
	//  parse("surname");
	//    parse("surname");

	//  importCsv("couchdb/entities.csv");
	//  		import("entities");
	//  import("images");
	//  		import("dbpedia");
	//  import("linkedin_connections_export_microsoft_outlook.csv");
	//  tests();
	//  fixNames();
	//  check(a(Tom-Hartley)==a(Tom Hartley));
	//  importAll();
	//  import("yago","yagoLabels.tsv");
	//  showNodes(parse("all 42650559"));
	//  deleteNode(get(42032201));

	//	int var=0;
	//  start_server();
	//  p("statementSize)");
	//  p( statementSize);
	//  importFreebase();
	//  testFreebase();
	//	if(context->nodeCount<10000)importAll();
	//	testHash();
	//	checkWordnet();
	//  const char* x=concat("ab","cde");
	//  check(eq(x,"abcde"));
	//	import("freebase");
	//  importFreebase();// needs /Volumes/Data/BIG/ !
	//  	import("yago");
	//  check(hasWord("Tom_Hartley"));
	//  N a=  a(Tom_Hartley);
	//  N b=  the(Tom_Hartley);
	//  N a1=  a(Tom Hartley);
	//  N b1=  the(Tom Hartley);
	//  N a2=  a(Tom_Hartley);
	//  N b2=  the(Tom_Hartley);
	//  check(a==a2);
	//    check(b==b2);
	//  importAll();
	//	tests();
	//	testQueryMore();
	//	testQuery();
	//	testFacets();
	//		testOpposite();
	//	ps("test brand new stuff");
	//	parse("opposite of bad");
	//	parse("all bug");
	//	showNodes(all_instances(a(bug)));
}

void testInclude() {
	//  parse("include hamburg Population");
	//  parse("include city Elevation");
	//  NV l= parse("learn hamburg type city");
	//  show(l[0]);
	//  check(has(l[0],"type","city"));

	//  parse("learn 5463914 type 2000586");
	//  parse("include hamburg Bundesland");

	//  addStatement(the(hamburg),Type,the(city));
	//  addStatement(a("hamburg"),Type,the(city));
	//  parse("city include Erhebung");
	//  parse("city include Erhebung");
	parse("5136347 include Erhebung", false, false);



	//  Nachname
	//  parse("exclude fafdafds");
	//  1459866
	//  handle("/xml/all/1459866");
	//  handle("/xml/verbose/1459866");
	//  handle("/xml/excluded/1459866");
	//  handle("/xml/excluded/Hamburg");
	//  handle("/xml/long/Hamburg");
	handle("/xml/all/Hamburg +type");
	//  handle("/xml/all/Hamburg +type");
	//  handle("/xml/excluded");
}

bool assertResult(char *query, char *value0) {
	NodeVector result = parse(query, false, false);
	//	cchar* result=query2(query).data();
	Node *abstract = getAbstract(value0);
	Node *value = getThe(abstract);
	check(contains(result, value) or contains(result, abstract));
	return 1;
	//	return check(eq(result,value));
}

void flattenGeographischeKoordinaten() {
	N n = the(Geographische Koordinaten);
	if (isAbstract(n))n = get(-10625);
	N b = the(Breitengrad);
	N l = the(Längengrad);
	Statement *s = 0;
	while ((s = nextStatement(n->id, s))) {
		if (s->predicate == n->id) {
			N ort = s->Subject();
			N c = s->Object();
			N bb = findProperty(c, "Breitengrad");
			if (!bb)bb = findProperty(c, "Latitude");
			addStatement(ort, b, bb, true, false);
//			S st=findStatement(c, getNode(1129), Any);
//			if(!st)continue;
			//			N ll=st->Object();
			N ll = getProperty(c, "Längengrad");
			if (!ll)ll = getProperty(c, "Longitude");
			addStatement(ort, l, ll, true, false);
			//						S sl=addStatement(ort,l,ll,false,true);

			//			S sl=addStatement(ort,l,ll,false,false);
			//			show(ort);
			//			showStatement(sb);
			//			p(bb!=0);
		}
	}
}

void fixAllNames() {
	for (int i = 1000; i < maxNodes; i++) {
		N n = getNode(i);
		if (!n->name)continue;
		if (n == Error) {
			pf("STOPPING AT %d", i);
			break;
		}
		if (endsWith(n->name, "\" .")) {
			keep_to(n->name, "\" .");
		}
		if (endsWith(n->name, " .")) {
			keep_to(n->name, " .");
		}
	}
}

void fixRelations() {
	for (int i = 1; i < context->statementCount; i++) {
		S s = &context->statements[i];
		if (s->predicate == -10031)s->predicate = _Type; // Ist ein(e)
		if (s->predicate == -10460)s->predicate = _synonym;// Als gleichwertig bezeichnet
		if (s->predicate == -10361)s->predicate = _PartOf;// Ist Teil von
		if (s->predicate == -10527)s->predicate = _Part;// Besteht aus
		if (s->predicate == -10150)s->predicate = _Part;// x Untereinheit (administrative Einheit) // HAS
		if (s->predicate == -10171)s->predicate = _SuperClass;// übergeordnetes Taxon
	}
}

void getClasses() {
	NodeVector all;
	for (int i = 1; i < max(context->nodeCount, context->lastNode); i++) {
		N n = &context->nodes[i];
		if (!n->id or n->name < context->nodeNames or n->name > context->nodeNames + context->currentNameSlot)continue;
		if (startsWith(n->name, "http"))continue;
		if (startsWith(n->name, "_"))continue;
		if (eq(n->name, "◊"))continue;
		if (isNumber(n->name))continue;
		if (n->kind == _clazz)//_entity
			all.push_back(n);
		//			printf("%s	Q%d\n",n->name,n->id);
	}

	sortNodes(all);
	for (int i = 1; i < all.size(); i++) {
		N n = all[i];
		printf("%d	%s	Q%d\n", n->statementCount, n->name, n->id);
	}

}

void getSuperClasses() {
	NodeSet all;
	for (int i = 1; i < context->statementCount; i++) {
		S s = &context->statements[i];
		if (!checkStatement(s, 1, 0))continue;
		if (s->predicate == -10031 or s->predicate == _Type or s->Predicate() == SuperClass) { // Ist ein(e)
			N n = s->Object();
			if (!n->id or n->name < context->nodeNames or
			    n->name > context->nodeNames + context->currentNameSlot)
				continue;
			if (startsWith(n->name, "http"))continue;
			if (eq(n->name, "◊"))continue;
			if (startsWith(n->name, "_"))continue;
			if (isNumber(n->name))continue;
			//			if(!contains(all,))// NodeSet auto dedupe!
			all.insert(n);// .push_back(n);
		}
	}
	//	sortNodes(all);
	NodeSet::iterator it;
	//	for(it=all->begin();it!=all->end();it++){
	//		for(int i=1;i<all.size();i++){
	for (it = all.begin(); it != all.end(); it++) {
		N n = (Node *) *it;
		//		N n=all[i];
		printf("%s	%d\n", n->name, n->id);
	}

}

void recursiveTaxonomy() {
//	NS all= findAll(the(Person),subclassFilter);

	NS all = findAll(get(7569), subclassFilter);// Kind: Test
//	NS all= findAll(get(215627),subclassFilter);// Person
//	NS all= findAll(get(5),subclassFilter);// Mensch
//	NS all= findAll(get(28640),subclassFilter);// Beruf
//	NS all= findAll(get(-10106),subclassFilter);// Tätigkeit

	printf("------------------------------------\n");
//	show(all); //already done
}

//	cchar* query="president of the United States of America";
//	cchar* query="AWS Server";
//	cchar* query="Cloud Amazon Web Services Server";// vs Amazon Web Services ??
void testEntities() {
	handle("/json/entities/Elektroinstallateur darmstadt");
	parse(":ee Elektroinstallateur darmstadt", false, false);
	//	cchar* query="kopfsalat";
	cchar *query = "Elektroinstallateur darmstadt";
//	cchar* query="Kaufmann";
	N n = getClass(get(query));
//	N n=getType(get(query));
	show(n);

	NV all = findEntites(query);
	check(all.size() > 0);
	show(all);
//	getTopics(all);
}

void testWikidataTopics() {
	N t = getTopic(get(4543));
	check(!eq(t->name, "Beachvolleyball"));

	//	N t=getTopic(get(1744));
	//	N t=getTopic(get(2765216));

	filterWikiType(3423);
	N to = getTopic(get(550866));
	check(eq(to->name, "Dorf") || eq(to->name, "Kommune"));// 3266850
	N sackgassendorf = get(177966);
	N e = getTopic(sackgassendorf);// Sackgassendorf -> Reihendorf -> Dorf OK
	check(eq(e->name, "Dorf"));
	N ta = getType(sackgassendorf);
	check(eq(ta->name, "Siedlung"));
	N c = getClass(sackgassendorf);
	check(eq(c->name, "Reihendorf"));
	//	N e=getTopic(get(177966));// Sackgassendorf -> Reihendorf -> Dorf OK
	//	N e=the(Sackgassendorf);
	//	N e=the(Angela Merkel);
	//	show(e);
	//	t=getTopic(get(2765216));
	//	t=getTopic(get(2765216));
	//	t=getTopic(get(2765216));
	//	t=getTopic(get(2765216));
	//	t=getTopic(get(2765216));
	//	t=getTopic(get(2765216));
	p(t->name);
	//  testing=false;// NO RELATIONS!
	//	fixRelations();

}

void testWikidata() {
	testWikidataTopics();
	testEntities();
	testImportWiki();
}

void testLabelInstances() {
	NV all = allInstances(a(amazon));
	check(contains(all, get(3884)));
}


void testTheSingletons();

void testTheSingletons() {
	N a = the(bla_typ);
	N a2 = the(bla_typ);
	assertEquals(a, a2);
	N b = getThe("bla2", a);
	get("bla2");
	N b2 = getThe("bla2", a);
	assertEquals(b, b2);
}


void collectTopics() {
	debug = false;
	for (int i = 1; i < (int) nodeCount(); i++) {
		Context *context = getContext();
		if (!checkNode(i))continue;
		Node *n = &context->nodes[i];
		if (!checkNode(n))continue;
		Node *t = getTopic(n);
		if (!checkNode(t))continue;
		if (t == n)continue;
		p(t->name);
	}
}

void testSynonyms(){

	learn("Population Synonym Einwohnerzahl");
	learn("Population Synonym Einwohner");
	learn("Einwohner Synonym Population");
	learn("Postleitzahl Label PLZ");
	Node *normedEntity = normEntity(the(Population));
	check(normedEntity == the(Einwohnerzahl));
	Node *normedEntity2 = normEntity(the(Einwohner));
	check(normedEntity2 == the(Population));
	const NodeVector pop = query("Gehren.Population");
//	const NodeVector pop = parse("Gehren.Population", false, true);
	check(eq(pop[0], the(3703)));
	check(eq(parse("Gehren.Population", false)[0],the(3703)));
	check(eq(parse("Gehren.Einwohner", false)[0],the(3703)));

//	check(eq(findProperty(a(Gehren),the(Population)),the(3703)));
//	check(eq(findProperty(a(Gehren),the(Einwohnerzahl)),the(3703)));
//	check(eq(findProperty(a(Gehren),the(Population)),the(3703)));
//	check(eq(findProperty(a(Gehren),the(Einwohner)),the(3703)));
	check(eq(getProperty(a(Gehren),the(Einwohner)),the(3703)));

//	check(eq(findProperty(the(Gehren),the(Population)),the(3703)));
//	check(eq(getProperty(the(Gehren),the(Einwohner)),the(3703)));
//	check(eq(has(the(Gehren),the(Population)),the(3703)));
//	check(has(the(Gehren), the(Population)));
//	check(has(the(Gehren), the(Einwohner)));

	check(eq(findProperty(the(Gehren),the(PLZ)),the(98708)));
	check(eq(getProperty(the(Gehren),the(PLZ)),the(98708)));
	check(isA(the(Population), the(Einwohner)));
	check(isA(the(Postleitzahl), the(PLZ)));
	check(eq(getProperty(the(Gehren),the(Postleitzahl)),the(98708)));
	check(has(the(Gehren), the(Postleitzahl)));
	check(has(the(Gehren), the(PLZ)));

	check(yetvisitedIsA[1]==0);
	check(isA(the(Postleitzahl), the(PLZ)));
//	check(yetvisitedIsA[the(Postleitzahl)->id+the(PLZ)->id*billion]);
	p(has(the(Gehren), the(PLZ)));
	check(has(the(Gehren), the(PLZ)));
//	parse(":is Population Einwohner");
//	parse(":has Gehren Einwohner");

}

void testMarkedNodes() {
	Node n = *the(TEST123);
	addStatement4(0, n.id, _see, 1234);
	Node marked = *get(1234);
	check(marked.kind != 0)
//	initNode(id, nodeName, kind, contextId);
}

void entity_extraction(char *ee) {
	handle(concat("/ee/", ee));
	findEntites(ee);
}

void ee(cchar *entities) {
	handle(concat("/ee/", entities));
	findEntites(entities);
}

char *normLabel(char *old) {
	char *neu = (char *) malloc(10000);
	int j = 0;
	for (int i = 0; i < strlen(old); i++) {
		char c = old[i];
		bool good = 0;
		if (c >= 'a' and c <= 'z')good = 1;
		if (c >= 'A' and c <= 'Z')good = 1;
		if (c >= '0' and c <= '9')good = 1;
		if (c == ' ')good = 1;
//		if(c=='ä')good=1;
//		if(c=='ë')good=1;
//		if(c=='ï')good=1;
//		if(c=='ö')good=1;
//		if(c=='ü')good=1;
//		if(c=='ö')good=1;
		if (good) {
			neu[j] = c;
			j++;
		} else {
			p(int(c));
		}
	}
	neu[j] = 0;
//	if(i==j)return old;
	return neu;
}

void addNormLabels() {

	for (int i = 1; i < max(context->nodeCount, context->lastNode); i++) {
		N n = &context->nodes[i];
		if (!n->id or n->name < context->nodeNames or n->name > context->nodeNames + context->currentNameSlot)continue;
		if (!isAbstract(n))continue;
		if (strlen(n->name) < 5)continue;// strange ids ...
		if (n->name[0] < 'A')continue;// strange ids ...
		char *normed = normLabel(n->name);
		if (eq(normed, n->name)) {
			free(normed);
			continue;
		}
		N no = getAbstract(normed);
		addStatement(n, Label, no);
		free(normed);
	}

}

void testUmlauts() {
	learn("Mæ is mae");
	learn("Mü is mö");
	check(isA(the(mü), the(mö)));
	check(wordHash("mü") == wordHash("MÜ")); // todo: seo here!
}

void testServer(){
	start_server(8080);
}

extern "C" void testAll() {
#ifndef __clang_analyzer__
	context = getContext(0);
	germanLabels = false; // for tests!
//	clearMemory();
	testBasics();
	testUmlauts();
	testTheSingletons();
	testInstanceLogic(); // needs addStatementToNodeWithInstanceGap
	testStringLogic();
	testHash();
	testValueLogic();
	testInstancesAtEnd();
	testInsertForceStart();
	testCities();
	testSynonyms();
	testQuery();
	testWins();
	testLabelInstances();
	testDummyLogic();
	testReification();
	testStringLogic2();
	testOpposite();

	testScanf();
	testGeoDB();
	testWikidata();
//	testYago();

	//  share_memory();
	//  init();
	//	clearTestContext();
	germanLabels = false;
	testImportExport();
	checkWordnet();
	checkGeo();    //	testDummyLogic();// too big
	testInclude();
	// shaky
	testWordnet(); // PASSES if not Interfering with Yago!!!! Todo : stay in context first!!
	testFacets();
	testQuery();
	testPaths();
	testFactLearning();
	testNameReuse();
	p("ALL TESTS SUCCESSFUL!");
#endif
	//  testLoop();
}


void testImportant() {
	testTheSingletons();
	testBasics();
	testInsertForceStart();
	testAll();// todo!
}




void testBug() {// current
	ee("welche+sprachen+iphone+x");
//	handle("short/json/ee/hat+das+apple+iphone+x+gesichtserkennung");
//	handle("json/short/query/Apple+iPhone+X.Ean");
	exit(0);
}


void testBrandNewStuff() {
	#ifdef __clang_analyzer__
		return; // no tests when profiling
    #endif
	p("Test Brand New Stuff");
//	testBug();

clearMemory();
importWordnet();

//   testWins();
//	handle("/json/query/iphone_se.vorl%C3%A4ufer");
//	handle("/json/qa/iphone_se+vorl%C3%A4ufer");
//	handle("/json/qa/iphone_x");
	handle("/json/qa/iphone_x Barometer");
//  findAnswers("iphone_x Barometer");
//	Luftdruckmesser


//	testWins();
	exit(0);
	testSynonyms();
	testServer();
	testAll();
////////////////////////////////////////////////////////////////////
//	handle("/json/all/query/all/6256");
}
// Continue with shell


#undef a
#undef _