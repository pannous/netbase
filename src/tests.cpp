#pragma once

// malloc, exit:
#include <cstdlib>
// strcmp
#include <string.h>
#include <stdlib.h>

#include "netbase.hpp"
#include "import.hpp"
#include "export.hpp"
#include "util.hpp"
#include "relations.hpp"
#include "query.hpp"
#include "init.hpp"
//#define assert(cond) ((cond)?(0): (fprintf (stderr,"FAILED: \ %s, file %s, line %d \n",#cond,__FILE__,__LINE__), abort()))

void clearTestContext(){
	clearMemory();
	initRelations();
}

void checkWordnet(){
	if(hasWord("effloresce"))
		return;
	else
		importWordnet();
}


bool assert(bool test, string what) {// bool nix gut
    printf("----\n");
    printf(what.c_str());
    if (test) printf(" OK\n");
    if (!test) {
        printf(" Failed\n");
        flush();
        if (exitOnFailure != false)exit(1);
    }
    return test;
}

void testScanf() {
    //FALSCH!!: A feature with sscanf is that you can define variables inside it:
    //sscanf("foo % 1 2 3 fum","%s %% %{%d%} %s",string s1, array(string) as, string s2);
    char buf[] = "string1 string2 string3";
    char array[100];
    if (sscanf(buf, "%*s%*s%99s", array) == 1) {
        p(array);
    }
    char* as = (char*) malloc(1000);
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
    char d[100000];
    bool matching; // leider immer !? --
    char* match = "abc=er [sdaf=er ] =fe";
    matching = sscanf(match, "%s [%s ] =%s", a, b, c);
    pi(matching);
    p(a);
    p(b);
    p(c);
    string x = "a[b=c]=d";
    ps(x);
    //	p(x.replace("["," ["));
    //	exit(1);
}

void test() {
    //you have a pointer to some read-only characters
    char* a = "abc";

    char b[] = "abc";
    //you have an element array of characters that you can do what you like with.

    check(hash("abc") == hash(a));
    check(hash("abc") == hash(b));
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
    //    check(contains("abCd","Bc",true));
    check(!contains("abCd", "bd", true));
    check(!contains("abCd", "Bc", false));
    //    assert(!contains("abcd",""),"gr");
    int testContext = 1;
    Context* c = getContext(testContext);
    Node* syn = &c->nodes[(21)];
    show(syn);
    check(syn == Synonym);
    p(Synonym->name);
    //  does not work in test environment ( only???? ) ...  fix!!!
#ifdef inlineName // todo!
    check(eq(Synonym->name, "synonym"));
#endif
    showStatement(getStatement(syn, 0));
    //    initContext(c);
    int initialNodeCount = c->nodeCount;
    int initialStatementCount = c->statementCount;
    c->nodeCount = initialNodeCount; // reset for tests! dont save!
    c->statementCount = initialStatementCount;

    //    memset(&c->nodes[initialNodeCount], 0, sizeof (Node) *( maxNodes() - initialNodeCount -1)); //calloc!
    //    memset(c->statements, 0, sizeof (Statement) * maxStatements() - 1);
    //    memset(0,c->statements,maxStatementsPerNode)

    Node* good = add("good", adjective, 1);
    Node* is = add("is", verb, 1);
    Node* test = add("test", noun, 1);
    assert(is != null, "is!=null");

    showContext(current_context);
    //	c->name="Public";
    if (multipleContexts)
        assert(c->id == 1, "c.id==1 multipleContexts");
    else
        assert(c->id == wordnet, "c.id==wordnet"); //wordnet if

    //	assert(c->name=="Public","c.name==Public");
    assert(c->nodeCount >= initialNodeCount + 3, "c.nodes==3"); //3+abstracts
    assert(eq(name(test), "test"), "name(test)=='test'");
    assert(eq(c->nodes[initialNodeCount].name, "good"), "c.nodes[1].name==good");
    //	p(c->nodes[3].name);
    //	pi(c->nodes[3].name);
    //if(!loaded)
    //	assert(eq(c->nodes[3].name,null),"c.nodes[3].name==null");
    //    assert(findWord(testContext, "good",true)==good), "find(wordnet,good)==good");
    //    assert(eq(findWord(testContext, "good",true)->name,good), "find(wordnet,good)==good");
    Node* dead = &c->nodes[999];
    // Statement* s=addStatement4(c.id, test->id,is->id,good->id);
    int statementCount = c->statementCount;
    Statement* s = addStatement(test, is, good);
    pi(c->statementCount);
    //	assert(c->statementCount==statementCount+1,"c.statementCount==1");// if not yet there
    assert(s->subject == test->id, "s->subject==test->id");
    assert(s->Subject == test, "s->Subject==test");
    assert(s->Predicate == is, "s->predicate==is");
    assert(s->Object == good, "s->object==good");

    check(getStatement(test, 0)->Subject == a(test));
    check(getStatement(good, 0)->Subject == a(good));
    check(getStatement(test, 1)->Object == &c->nodes[(noun)]);
    check(getStatement(good, 1)->Object == &c->nodes[(adjective)]);
    check(getStatement(test, 2) == s);
    check(getStatement(is, 2) == s);
    check(getStatement(good, 2) == s);
    show(test);
    show(is);
    show(dead);
    statementCount = c->statementCount;
    // sonderf√§lle
    addStatement4(-1, -2, -3, -4);
    assert(c->statementCount == statementCount, "c.statementCount==1");

    Node* instance = getThe("instance");
    Node* instance2 = getThe("instance");
#ifdef inlineName // todo!
    assert(instance == instance2, "instance==instance2"); //  it failes when strings are lost
#endif
    // todo: Important! keep Context string pool in other .cpp object files!!!!!
    //#define inlineName true no good resolution!
    //	assert(instance==Instance,"instance==Instance");

    // addStatement4(0,0,0,0);
    // addStatement4(rand(),rand(),rand(),rand());
    // check(test,is,good);

}

void testLogic() {
    Node* test = add("test", _node);
    Node* size = add("size", noun);
    Node* funny = add("funny", adjective);
    Node* eleven = add("11", number);
    Node* test2 = add("test", verb);
    Node* beth = add("Beth", _person);
    Node* CEO = add("CEO", _person);
    Node* manager = add("manager", _person);
    Node* karsten = add("karsten", _node);
    Node* is = Type; // Kind;// add("is",verb);
    Node* cute = add("cute", adjective);
    Node* people = add("people", noun);
    Node* plural = Plural; // todo: semantic! add("plural", noun);

     learn("test.funny");
     learn("test.size=11");
    // assert(*test.is(funny));
    //    Statement* s1 = addStatement(beth, is, cute);
    Statement* s1 = addStatement(beth, Attribute, cute);

    Statement* s2 = addStatement4(current_context, _person, plural->id, people->id);
    Statement* s2a = addStatement4(current_context, _person, plural->id, people->id);
    Statement* s2b = addStatement(Person, plural, people);
    Statement* s2c = addStatement(Person, plural, people);

    //	assert(s2==s2a,"dont duplicate edges 4");??
    assert(s2c == s2b, "dont duplicate edges");
    //	assert(s2==s2b,"dont duplicate edges");// replace abstract with item!!
    Statement* s3 = addStatement(test, Attribute, funny);
    //    Statement* s4 = addStatement(Person, Instance, karsten);
    Statement* s4 = addStatement(CEO, Instance, karsten);
	check(eq(karsten->name,"karsten"));
    Statement* s4a = addStatement(manager, SubClass, CEO);
    //    Statement* s4a = addStatement(CEO, SuperClass, manager);
    Statement* s4b = addStatement(manager, is, a(worker));

    check(isA(CEO, manager));
    clearAlgorithmHash();
    check(isA(karsten, CEO));
    clearAlgorithmHash();
    // !!!   todo: Instance -> SuperClass framebreak!
    check(isA(karsten, manager));
    clearAlgorithmHash();
    check(isA(karsten, Person));
    check(isA(karsten, a(worker)));

    showContext(current_context);
    showStatement(s2);

    assert(getContext(current_context)->statementCount >= 3, "current_context->statementCount==2");
    //	assert(getStatement(funny,0)==s3,"getStatement(funny,0)==s");
    addStatement(test, size, eleven);
    show(test);
    addStatement(test2, Passive, eleven);

    showContext(current_context);
    // show(findMember(test,"funny"));
    check(findMember(beth,"cute"))
    assert(findMember(test, "funny") != null, "findMember(test,funny)");
    assert(findMember(test, "funny") == funny, "findMember(test,funny)");
    Node* t = findMatch(test, "funny");
    assert(t == test, "findMatch(test,funny)");
    t = findMatch(test, "bunny");
    assert(t == 0, "findMatch(test,bunny)");
    assert(findMatch(test, "size=11") == test, "findMember(test,size=11)");
    assert(findMatch(test, "size=12") == 0, "findMember(test,size=12)");
	/* How can this have ever passed, when not using parseFilter ? didn't
	check(findMember(test, "size>10") == test);
	check(findMatch(test, "size>10") == test);
	assert(findMatch(test, "size<12") == test, "findMember(test,size<12)");
	assert(findMatch(test, "size>=11") == test, "findMember(test,size>=11)");
	assert(findMatch(test, "size<=11") == test, "findMember(test,size<=11)");
	*/
    // get data
    //	vector<Node*> rows1=query("select * from test where size=11 and test.funny");
    //	Node* n = (Node *)rows1[0];
	clearAlgorithmHash();
	NodeVector& tests=all_instances(test);
	check(contains(tests,test));
	tests=all_instances(getThe("test"));
	check(contains(tests,test));
	tests=all_instances(getAbstract("test"));
	check(contains(tests,test));
	pi(tests.size());
	NodeVector funnys= filter(tests, "funny");
	check(contains(tests,test));
    char* sql = "select * from test where funny";
    NodeVector list = query(sql);
    //	show((Node *)(list[0]));
    //	show((Node *)list[1]);
	showNodes(list,true);
    assert(contains(list, test), sql);
    //	sql="select * from test funny";
    //	assert(last(query(sql)) == 0,sql);
    sql = "select * from test where size=11";
	list=query(sql);
    assert(contains(list, test), sql);
    sql = "select * from test where size=11 and funny";
    list=query(sql);
    assert(contains(list, test), sql);
    sql = "select * from test where size=11 and test.funny";
    list=query(sql);
    assert(contains(list, test), sql);
    sql = "select * from test where size=11 and test.bunny";
    list=query(sql);
    assert(false==contains(list, test), sql);
    sql = "test that are funny";
    list=query(sql);
    assert(contains(list, test), sql);
	sql = "all tests";
	list=query(sql);
    assert(contains(list, test), sql);
    sql = "test where size=11";
    list=query(sql);
    assert(contains(list, test), sql);
    sql = "test with size=11";
    list=query(sql);
    assert(contains(list, test), sql);
    sql = "tests with size=11";
    list=query(sql);
    assert(contains(list, test), sql);
    sql = "tests with size=11 and test.funny";
    list=query(sql);
    assert(contains(list, test), sql);
    sql = "test with size>10";
    list=query(sql);
    assert(contains(list, test), sql);


    assert(isA(beth, Person), "isA(beth,Person->name)");
    // deprecated:
    //    assert(isA5(beth, Person->name), "isA(beth,Person->name)");
    show(Person);
    assert(has(Person, plural, people), "has Person,plural,people");

    clearAlgorithmHash();
    assert(isA(Person, people), "isA(get(person),people)");
    clearAlgorithmHash();

    // deprecated:
    assert(isA(beth, people), "isA(beth,people)");
    //    assert(isA(beth,people->name),"isA(beth,people->name)");
    //    assert(!isA(cute,Person),"!isA(cute,Person)");
    clearAlgorithmHash();
    assert(findStatement(cute, Type, Person) == 0, "findStatement(cute,Type,Person)==0");
    addStatement(beth, Type, Person);
    assert(findStatement(cute, Type, Person) == 0, "findStatement(cute,Type,Person)==0 b");
    clearAlgorithmHash();
    vector<Node*> persons = all_instances(Person);
    assert(persons.size() > 0, "all_instances(Person).size()>0");
    assert(contains(persons, beth), "contains(persons,Beth)");
    //    check(contains(people, karsten));
    vector<Node*> managers = all_instances(manager);
    //    todo:
    //    check(contains(managers, karsten));
    //    vector<Node*>
    persons = find_all("person", current_context, true);
    assert(persons.size() > 1, "find_all(person)>1");
    check(contains(persons, beth));
    check(has(beth, Attribute, cute));
    check(findMember(beth, "cute"));
    check(findMatch(beth, "cute"));
    clearAlgorithmHash();
    sql = "select * from person where cute";
    vector<Node*> cuties = query(sql);
    check(contains(cuties, beth));
    check(last(cuties) == beth);
    sql = "select * from person where mute";
    assert(last(query(sql)) == 0, sql);
    clearAlgorithmHash(); //arg!
    assert(all_instances(people).size() > 0, "all_instances(people)>0");
    assert(find_all("people", current_context, true).size() > 1, "find_all(people)>1");
    clearAlgorithmHash();
    check(contains(all_instances(people), beth));
    check(contains(find_all("people", current_context, true), beth));
    //    check(isA(beth,cute));

    vector<Node*> peoples = query("select * from people");
    check(contains(peoples, beth));
    sql = "select * from people where cute";
    vector<Node*> all = query(sql);
    check(contains(all, beth));
    check(last(all) == beth);

    Statement* learned=learn("beth is cute");
    evaluate("beth is cute");
    /*
        assert(match("test[funny]")==test,"match('test[funny]')==test");
        assert(match("test[size=11]")==test,"match('test[size=11]')==test");
        assert(match("test[size=11,funny]")==test,"match('test[size=11,funny]')==test");
        assert(match("test[size=11 and funny]")==test,"match('test[size=11,funny]')==test");
        assert(match("test[size>10]")==test,"match('test[size>10]')==test");
        assert(match("test[size<12]")==test,"match('test[size<12]')==test");
        assert(match("test[size>10,funny]")==test,"match('test[size>10,funny]')==test");
        assert(match("test[size<12,funny]")==test,"match('test[size<12,funny]')==test");
        assert(match("test[size>10 and funny]")==test,"match('test[size>10,funny]')==test");
        assert(match("test[size<12 And funny]")==test,"match('test[size<12,funny]')==test");
     */
    // get data
    //    	find_english("people that are cute");
    //    	find_english("people which are cute");
    //    	find_english("people with blond hair");
    //    	find_english("people without hair");
    //    	find_english("persons which are cute");
    //    	find_english("all persons which are cute");
    //    	find_english("the person that is cute");

    // kinds of x => ?->parent->x | ?->parent->x
    // wie sp√§t=
}

void testWordnet() {
    load();
	checkWordnet();
    //    importAll();
    Context c = *getContext(wordnet);
    showContext(wordnet);
    p("contexts[wordnet].nodeCount:");
    pi(c.nodeCount);
    show(&c.nodes[38749]);
    // assert(strcmp(c.nodes[38749].name,"disposal")==0,"contexts[wordnet].nodes[38749].name=disposal");
    assert(checkNode(&c.nodes[38749], 38749), "checkNode(c.nodes[38749],38749)");
    // assert(strcmp(c.nodes[38749].name,"fall")==0,"contexts[wordnet].nodes[38749].name=fall");
    //    assert(findWord(wordnet, "fall") > 0, "find(wordnet,fall)>0");
    assert(findWord(wordnet, "fall", true) > 0, "find(wordnet,fall)>0");
    // Node: context:wordnet id=4919 name=test statementCount=17 kind=wordnet
    Node* test = findWord(wordnet, "test", true);
    // assert(test->id==4919)
    assert(strcmp(test->name, "test") == 0, "test->name==test");
    //	assert(test->statementCount==17,"test->statementCount==17");
    show(the(duck));
    show(a(duck));
    check(isA(a(gooney), a(bird)));
    //    assert(isA(a(duck), a(bird)), "duck isA poultry");
    //    assert(isA(a(duck), a(poultry)), "duck isA poultry");
    //    assert(isA(the(duck), the(bird)), "duck isA poultry");// yeah: the! main concept
    clearAlgorithmHash();
    //todo:
    assert(isA(getAbstract("duck"), getAbstract("bird")), "duck isA bird"); // yuhu! 2010-02-07
    assert(contains(all_instances(getAbstract("bird")), getThe("duck")), "bird has instance duck");

    //    assert(has(get("duck"),get("feathers")),"has(duck,feathers)");
    //    assert(has("duck","beak"),"has(duck,beak)");
    //    assert(has("duck","head"),"has(duck,head)");
    //    assert(has("duck","tail"),"has(duck,tail)");
    //    assert(has("duck","feed"),"has(duck,feed)");

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

void testOutput() {
    printf("Warnings:\n");
    fflush(stdout);
    // restart Idea if nothing appears
    cout << "bla" << endl;
    cout.flush();
    flush();
}

//#define new(word) Node* word=getThe(#word);

void testStringLogic() {

    Node* Schlacht_von_Kleverhamm = getThe("Schlacht_von_Kleverhamm");
    //    die(Schlacht_von_Kleverhamm);
    eine(Schlacht);
    Node* Kleverhamm = getThe("Kleverhamm");
    show(Schlacht_von_Kleverhamm);
    show(Kleverhamm);
    check(isA(Schlacht_von_Kleverhamm, Schlacht));
    check(has(Kleverhamm, Schlacht_von_Kleverhamm));
}

void testStringLogic2() {
    eine(Schlacht);
    Node* Schlacht_bei_Guinegate = getThe("Guinegate_(1479),_Schlacht_bei"); // intellij display bug!
//    deleteNode(Schlacht_bei_Guinegate);
//    Schlacht_bei_Guinegate=getThe("Guinegate_(1479),_Schlacht_bei");
    show(Schlacht_bei_Guinegate);
    check(isA(Schlacht_bei_Guinegate, Schlacht));
    check(eq(getThe("near")->name,"near"));
    check(contains(instanceFilter(a(Guinegate)), the(Guinegate)));
	check(findStatement(Schlacht_bei_Guinegate, _(near), a(Guinegate)));
	check(has(Schlacht_bei_Guinegate, _(near), a(Guinegate)));
    check(has(Schlacht_bei_Guinegate, _(near), the(Guinegate)));
    // TODO !!!
//    check(has(a(Schlacht_bei_Guinegate), _(near), a(Guinegate)));
    Node* Armagnac_Weinbrand = getThe("Armagnac_(Weinbrand)");
    show(Armagnac_Weinbrand);
    check(isA(word(Armagnac), word(Weinbrand)));
    // todo : not if place:
    //    Heinrich-Heine-Preis_(Stadt_D√ºsseldorf)
    // der_<place> von_der/of
    //Kultureller_Ehrenpreis_der_Landeshauptstadt_M√ºnchen

    show(getThe("Musyoka,_Kalonzo,"));

}

void testHash() {
    char* thing="city";
    Node* city= getAbstract(thing);
    ps(city->name);
    check(eq(city->name,"city"));
    insertAbstractHash(hash(Circa->name), Member);
    insertAbstractHash(Circa);
#ifdef inlineName
    Node* a2 = getAbstract(Circa->name);
    check(a2 == Circa);
#endif
}

void testImportExport() {
    /* NOO dont overwrite nodes.bin ...
    Context* c=currentContext();
    int nodeCount=c->nodeCount;
    c->nodeCount=9999;
    save();
    load();
    check(c->nodeCount==9999);
    c->nodeCount=nodeCount;
    save();
     */
    ps("importing...");
    //    importList("MaennerVornamen.txt", "male_firstname");
    importList("FrauenVornamen.txt", "female_firstname");
    find_all("Jenny");

    show(getAbstract("Zilla")); //51566;
    show(getThe("Zilla"));

    addStatement(all(female_firstname), a(gender), a(female));
    addStatement(all(female_firstname), Owner, a(female));
    addStatement(all(male_firstname), are, a(firstname));
    addStatement(all(male_firstname), a(gender), a(male));
    addStatement(all(firstname), are, a(name));
    //    check(isA(word(James), _(male_firstname)));
    //    check(isA(word(James), _(name)));
    check(isA(a(Zilla), _(female_firstname)));
    clearAlgorithmHash();
    check(isA(a(female_firstname), a(name)));
    clearAlgorithmHash();
    check(isA(a(firstname), a(name)));
    clearAlgorithmHash();
    check(isA(a(Zilla), _(firstname)));
    clearAlgorithmHash();
    check(query("all firstnames", 10).size() > 10);
    check(isA(a(Zilla), _(name)));
    clearAlgorithmHash();

    //    load();
    //    virgin_memory=1;
    //    init();// reset !
    //    check(eq(normTitle("a_- b"),"ab")==1);
    //    check(!eq(normTitle("a_- b"),"ac")==1);
    //    memset(root_memory, 0, sizeOfSharedMemory);
    importCsv("/Users/pannous/data/base/adressen.txt"); //,","
    show(the(Alexandra Neumann));
    check(the(Alexandra Neumann) != null);
    check(the(Alexandra_Neumann) != null);
    check(has(the(Alexandra_Neumann), a(Postleitzahl), a(12167)));
    check(has(the(Alexandra_Neumann), pattern(a(Postleitzahl), Greater, a(12167))));

    //    show(word(female_firstname));
    //    check(isA(word(James), _(male_name)));
    //    check(query("all firstnames starting with 'a'").size() > 0);
}

void testImages() {
    getThe("alabama");
    import("images", "");
    show(getThe("alabama"));
    //    check(getImage("wiki_image")=="");//todo!
    check(getImage("Alabama") != "");
    check(getImage("abagsfadd") == "");
}

void testInstanceLogic() {
	// needs addStatementToNodeWithInstanceGap instead of addStatementToNode
	// why was that important? to skip 100000 instances (cities)
    Node* test3 = getThe("test", Adjective); //add("test", adjective);
	Node* test4 = getThe("test", Adjective);
    check(getThe("test", Adjective) == test3);

//	exit(0);//test make!!
//    Node* aBaum=getAbstract("Baum");
    ein(Baum);
    Statement* s= addStatement(Baum,the(colour),_(blue));
    addStatement(Baum,Instance,the(Ulme));
    show(Baum);
    addStatement(Baum,the(colour),_(green));
    addStatement(Baum,the(colour),_(pink));
    show(Baum);
    Statement *c=getStatement(Baum,1);
    check(c->Predicate!=Instance);
    c=getStatement(Baum,2);
    check(c->Predicate!=Instance);
}


void testValueLogic() {
    //    deleteStatements(a(Boot));
    //    deleteStatements(get(271156));
//    deleteNode(get(421153));
//    deleteNode(the(Boot));
    deleteNode(a(Boot));
    deleteNode(a("14.32 meter"));
    ein(Boot);
    check(eq(Boot->name,"Boot"));
    NodeVector alle=all_instances(Boot);
//    check(contains(alle, Boot)); // Nee, all_classes ja, all_instances nicht. oder?
    Node* m14 = value("", 14, "m");
    Node* m15 = value("", 15, "m");
    Node* mm14 = value("", 14000, "mm");
    Node* mm15 = value("", 15000, "mm");
    Node* m143 = value("14.3", 14.3, "meter");
    Node* m1430 = value("14.30", 14.30, "meter");
    Node* m1433 = value("14.330", 14.330, "meter");
    Node* m1433_duplicate = value("14.330", 14.330, "meter");
    check(m1433_duplicate==m1433);


	Statement* boot_length=addStatement(Boot, a(length), m143);
    show(Boot);
    countInstances(m143);
    show(m143);
	check(findStatement(Boot, a(length), m143,1,1,1));
	check(findStatement(Boot, a(length), m143,0,0,0));
    Statement* boot_length2=addStatement(Boot, a(length), m143);
	check(boot_length==boot_length2);// duplicate

	show(m14);
    show(m1433);
    show(Boot);
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

    //    check(isA(m14,m143));
    //    check(isA(m14,m1432));
    check(isA(m143, m1430));

    //    check(isA4(m14,m1432));
    check(isA4(m143, m1430));
    //    check(isA4(m14,m143));//??

    //    getA("length")
    show(Boot);
    check(has(Boot, the(length), m1430));
    check(has(Boot, a(length), m1430));

    //    check(has(Boot,a(length),matcher(Equals,m143)));
    //    check(has(Boot,a(length),matcher(Equals,m1432)));
    //    check(has(Boot,a(length),matcher(Greater,m14)));
    //    check(has(Boot,a(length),matcher(Less,m15)));
    //    check(has(Boot,a(length),matcher(Greater,mm14)));
    //    check(has(Boot,a(length),matcher(Less,mm15)));
    //    check(has(Boot,a(length),m14));

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
    clearAlgorithmHash();
    showStatement(findStatement(kind, SuperClass, length, 1, 1, false));
    check(!findStatement(kind, SuperClass, length, 1, 1, false));
    check(!has(kind, SuperClass, length, 1, 1, false));
    check(!isA4(kind, length, true, true));
    //    show(m143);
    Statement *s=findStatement(Boot,a(length),Any);
    check(checkStatement(s));
    Node *n2=s->Object;
    check(isEqual(n2,m143));
    check(n2==m143);
    Statement *s2=findStatement(Boot,the(length),Any,1,1,1);
    n2=s2->Object;
    check(n2==m143);
//    show(m143); //   417287
//    show(has(Boot, a(length)));
    Node *n=has(Boot, the(length));
//    show(n); // 413730
    //    check(has(Boot,the(length))==m143);
    check(eq(has(Boot, a(length)), m143));
    check(eq(has(Boot, the(length)), m143));
//    show(has(Boot, a(length)));
    showStatement(findStatement(Boot, a(length), Any));
    check(!isA4(the(length), the(kind)));
    check(!isA4(get(74), get(33)));
    check(eq(has(Boot, a(length), Any), m143));
    check(eq(has(Boot, a(length)), m143));

    //    check(has(Boot,a(length))==m143);

    check(has(Boot, pattern(a(length), Equals, m143)));
    check(has(Boot, pattern(the(length), Equals, m143)));
    check(has(Boot, pattern(a(length), Equals, m1430)));

    clearAlgorithmHash();
    check(has(Boot, pattern(a(length), Greater, m14)));// TODO CLEAR PATTERNS!!!
    check(has(Boot, pattern(a(length), Less, m15)));
    //check(has(Boot,pattern(a(length),Greater,mm14)));// unit transformation
    //check(has(Boot,pattern(a(length),Less,mm15)));
}

Statement* andStatement(Statement* s1, Statement* s2) {
    return addStatement(reify(s1), And, reify(s2));
}

Statement* orStatement(Statement* s1, Statement* s2) {
    return addStatement(reify(s1), Or, reify(s2));
}

void testQuery() {
    deleteNode(the(Boot));
    deleteNode(a(Boot));
    das(Boot);
    Node* m14 = value("", 14, "m");
    Node* m15 = value("", 15, "m");
    Node* m143 = value("14.320", 14.32, "meter");
    show(Boot);
    check(isA4(m14, getThe("14 m")));
//    check(isA4(m14, getThe("14 meter")));
    addStatement(Boot, a(length), m143);
    check(findStatement(Boot, a(length), m143));
    Query q;
    q.keyword = Boot;
    q.autoFacet = true;
//    q.fields.push_back(the(length));
    q.fields.push_back(a(length));
    Statement *s1 = pattern(a(length), Greater, m14);
    Statement *s2 = pattern(a(length), Less, m15);
    Statement *s3 = andStatement(s1, s2);
    Statement *s4 = pattern(a(length), Less, m14);
    Statement *s5 = orStatement(s3, s4);
//    q.filters.push_back(s4);
//    ps(query(q));
//    check(!contains(q.instances, Boot));
    empty(q.filters);

    ps(query(q));
    check(contains(q.instances, Boot));
    check(has(Boot, s1));
    q.filters.push_back(s1);
    ps(query(q));
    check(contains(q.instances, Boot));
    q.filters.push_back(s2);
    ps(query(q));
    check(contains(q.instances, Boot));
    q.filters.push_back(s3);
    ps(query(q));
    check(contains(q.instances, Boot));
    q.filters.push_back(s5);
    ps(query(q));
    check(contains(q.instances, Boot));
    q.filters.push_back(s4);
    ps(query(q));
    check(!contains(q.instances, Boot));

    if(!hasWord("Sheberghan"))
        importCsv("/Users/me/data/base/geo/geonames/cities1000.txt",getThe("city"),'\t',"alternatenames,modificationdate,geonameid","latitude,longitude,population,elevation,countrycode",2,"asciiname");

//	show(the(Sheberghan));
//    check(has(the(Sheberghan),the(population),the(55641)));// ?

//	show(the(Samangan));
//    check(has(the(Samangan),the(population),the(47823)));//960?
    check(has(a(Samangan),the(population),the(47823)));//960?
// todo match Wordnet Samangan with geoname Samangan in importCsv!

//    show(the(city));
    countInstances(the(city));
	Node* hasloh=the(Hasloh);// todo match addressbook Hasloh with geoname Hasloh in importCsv!
	show(the(Hasloh));
//    check(eq(the("Hasloh"),the(Hasloh)))
//    check(has(the(Hasloh),the(population),the(3460)));// SLOW!!

//    check(!areAll(the("latitude"),the(population)));
    Node *n=parseValue("3.4 mg");
    check(n->value.number==3.4);
//    has(n,Unit,a("mg"));
//    check(isEqual(n,parseValue("3400 µg"))

//    Node *pp=has(the(Hasloh),the(population), Any);
//    Node *p=has(the(Hasloh),the(population));
//    show(p);

    int limit=200;
    string s="select * from city where population<11300";
    q=parseQuery(s,limit);
    check(eq(q.keyword->name,"city"));
    query(q);
    q.instances=all_instances(the(city));
    NodeVector nv=filter(q,pattern(the(population),Equals,the(8022)));
    pi(nv.size());

    q.instances=all_instances(the(city));
    nv=filter(q,pattern(the(population),Less,the(12300)));
    pi(nv.size());

    q.instances=all_instances(the(city));
    nv=filter(q,pattern(the(population),Greater,the(11300)));
    pi(nv.size());

    string result=query2("city where population=3460");
    ps(result);
    q=parseQuery("city where population=3460",100);
    query(q);
    showNodes( q.instances , false);
    query2("city where population<11300");
    query2("city where population>1300");
    query2("city where population<1300");

}

void testFacets() {
}


//#define sn showNode
void testReification(){
	Statement* p=pattern(_(karsten),Attribute,_(cool));
	Node* re=reify(p);
	show(re);
	check(isA(re,Pattern));
	check(isA(re,_(pattern)));

}


void testFactLearning(){
	Statement* s=learn("Peter loves Jule");
	check(s->Subject==the(Peter));
	check(isA(s->Predicate,a(loves)));
	check(s->Object==the(Jule));
	check(has(the(Peter),a(loves),the(Jule)));
	addStatement(the(love),Plural,the(loves));//the(tense) ??
	check(has(the(Peter),a(love),the(Jule),1,1,1));
	Statement* s2=learn("Peter loves Jule");
	check(s==s2);

	addStatement(the(german_translation),is_a,Translation,true);
	addStatement(the(son),the(german_translation),the(Sohn),true);
	s=learn("Peter.son=Milan");
	check(s->Subject==the(Peter));
	check(isA(s->Predicate,a(son)));
	check(s->Object==the(Milan));
	check(has(the(Peter),a(son),the(Milan)));
	check(has(the(Peter),a(sons),the(Milan)));
	check(isA(the(Milan),a(son)));
	check(has(the(Peter),a(Sohn),the(Milan)));
}

void testPaths(){
	checkWordnet();
	NodeVector path= memberPath(a(human),a(hand));
	check(path.size()>2);
	check(has(a(human),Member, a(hand)));
	check(has(a(human),a(hand)));
//	check(a(bird),has_a,a(feather));
//	check(a(mouse),has_a,a(foot));
}

void tests() {
    //    testOutput();
    //	testScanf();
    //    share_memory();
//    init();

//    test();
//    testHash();
	clearTestContext();
//    testInstanceLogic(); // needs addStatementToNodeWithInstanceGap
	testBrandNewStuff();
    testValueLogic();
    testStringLogic();

    testLogic();
    testImportExport();
    testWordnet();
    testQuery();
    testFacets();

	testFactLearning();
	testReification();
	testPaths();
	p("ALL TESTS SUCCESSFUL!");
    //    testLoop();
}
void testCities(){
	char* line="geonameid\tname\tasciiname\talternatenames\tlatitude\tlongitude\tfeatureclass\tfeaturecode\tcountrycode\tcc2\tadmin1code\tadmin2code\\tadmin3code\\tadmin4code\\tpopulation\\televation\\tgtopo30\\ttimezone\\tmodificationdate\\n";
	int nr=splitStringC(line,0,'\t',true);
//	int nr=splitStringC("geonameid\tname\tasciiname\talternatenames\tlatitude\tlongitude\tfeatureclass\tfeaturecode\tcountrycode\tcc2\tadmin1code\tadmin2code\\tadmin3code\\tadmin4code\\tpopulation\\televation\\tgtopo30\\ttimezone\\tmodificationdate\\n",0,"\t",true);
	check(nr>4);
	vector<char*> fields;
//	int n=getFields(modifyConstChar(line),fields,'\t');
//	check(fields.size()==nr);
	clearMemory();
	if(!hasWord("Mersing")){
		char* ignore="alternatenames,featureclass,featurecode,cc2,admin1code,admin2code,admin3code,admin4code,gtopo30,timezone,modificationdate";
		importCsv("cities1000.txt",the(city),'\t',ignore);
	}
	p(the(Mersing));
	check(has(the(Mersing),a(population)))
	check(has(the(Mersing),value("population",22007)))
	check(!has(the(Mersing),value("population",22008)))

}

void testBrandNewStuff() {
	ps("test brand new stuff");
	testCities();

//	parentPath(a(bee),a(insect));
//	addStatement4(current_context,440792,Synonym->id, 441226);// insect  bug
//	parentPath(a(insect),a(bug));
//	shortestPath(a(bug), a(frog));
	//	importCsv("import/wins.csv");
	//	if(!hasWord("zip"))
	//	importXml("/Users/me/data/base/geo/geolocations/Orte_und_GeopIps_mit_PLZ.xml","city","ort");
}