/*
* File:  relations.cpp
* Author: me
*
* Created on March 28, 2012, 10:25 PM
*/
#include "util.hpp"
#include "relations.hpp"

Node *Error;
Node *Missing;
Node *Unknown;
Node *See;
Node *Similar;
Node *Any;// QUERY ONLY!
Node *Pattern; // temporary QUERY ONLY!
//Node* IS; // QUERY ONLY! TypeOrParent

//Node* Broader;
Node *Antonym;
Node *Parent;
Node *SuperClass; // Parent, HyponymOf
// Node* IsA;// Parent
Node *SubClass;// Hyponym
Node *Cause;
Node *Entailment; // Implication
Node *Owner; //Owner inverse Member  (Prince of Persia) <=> (Persia has Prince)
Node *Member;
Node *Part;
Node *PartOf;// Meronym: "wheels" is a meronym of "automobile".
Node *Substance;
Node *Synonym;
Node *Domain;
Node *PERTAINYM;
Node *Weight;
Node *Type;
Node *Instance;

Node *Active;
Node *Passive;
Node *Tag;
Node *Label;
Node *Labeled; // labeledAE / labelledBE adj.
//Node* Labels;
//Node* LabeledNode;
Node *Comment;// or as text value
Node *Description;// or as text value
Node *Category;// same:?
Node *Topic;
Node *SubContext;
Node *SuperContext;

// Types
Node *Internal;
Node *_Node;
Node *Thing;
Node *Item;
Node *Abstract; // Thing
Node *Class;
Node *Entity;
Node *Object;
Node *Relation;
Node *Reification;
Node *Property;
Node *Attribute; //ok, if Node* Attribute declared in netbase.cpp

Node *Person;
Node *Adjective;
Node *Noun;
Node *Verb;
Node *Adverb;

Node *Derives;// KF: Derived^-1
Node *Derived;// Adjective^-1
Node *DerivedFromNoun;

Node *Number;
Node *Double;// == Number
Node *Float;// == Number

Node *Integer;
Node *Long;// == Integer

Node *String;
Node *Date;

Node *Bytes;// byte[] -> serialized java object etc etc
Node *Map; // Map<String,T>
Node *Array;// T[]
Node *List; // List<T>

Node *Range;
Node *Unit;

Node *True;
Node *False;

Node *Translation;
Node *Plural;
Node *And;
Node *Or;
Node *Not;

//There are about 150 prepositions in English
Node *Of; // ==owner
Node *In; // ==location,...
Node *To; // ==direction,<object>
Node *From;
Node *By;
Node *For;
Node *On;
Node *Next;// followed by / comes-before
Node *Previous;// follows / predecessor / comes-after

Node *Equals;
Node *Greater;
Node *More;
Node *Smaller;
Node *Less; //smaller
Node *Between;
Node *Circa;
Node *Much;
Node *Little;
Node *Very;
Node *Contains;
Node *StartsWith;
Node *EndsWith;

Node *UsageContext;


void addLabel(Node *relation, const char lable[5]);

Node *addRelation(int id, const char *name, bool transitive = false) {
	Node *n = add_force(wordnet, id, name, _internal);
	insertAbstractHash(wordHash(name), n, false, false);// ECHT? TYP ?
//	if(n->statementCount==0 and id>0)// IT BETTER BE!!!
//		addStatement4(wordnet, getAbstract(name)->id,_instance,id);// Internal
//	if(transitive)??? baked into Algorithms, at least four standard relations?
	return n;
}

void initRelationsDE() {
	Error = addRelation(_error, "Fehler");
	Missing = addRelation(_missing, "Fehlend");
	Unknown = addRelation(_unknown, "?");
	See = addRelation(_see_also, "Siehe");
	Antonym = addRelation(_antonym, "Gegenteil");
	//	Part = addRelation(1, "part"); USA etc BUG!?!!
	Attribute = addRelation(_attribute, "Attribut"); // tag
	Property = Attribute;
	bool is_transitive = true;
	Cause = addRelation(_cause, "Grund", is_transitive); //
	Derived = addRelation(_derived, "Abgeleitet"); // Abgeleitet von
	Derives = addRelation(_derives, "abgeleitet"); //
	//	DerivedFromNoun =addRelation(_derived_from_noun, "derived from noun"); //
	//    DOMAIN_OF_SYNSET_CATEGORY =
	UsageContext = addRelation(_DOMAIN_CATEGORY, "Kontext"); // # sheet @ maths  // think of!! OWNER -> Part
	//  DOMAIN_OF_SYNSET_REGION =
	addRelation(_DOMAIN_REGION, "Region"); // mate @ australia
	//  DOMAIN_OF_SYNSET_USAGE =
	Domain = addRelation(_DOMAIN_USAGE, "Bereich"); // #bitch @ colloquialism  or fin de siecle @ French  # fuck(vulgar)
	//  ENTAILMENT =
//	Synonym !?!
//	addRelation(_ENTAILMENT, "Impliziert",is_transitive); //ENTAILMENT jump implies come down
	SuperClass = addRelation(_Hypernym, "Klasse", is_transitive); //Parent ,"Hypernym"
	Parent = SuperClass;
	//	IsA = SuperClass;

	SubClass = addRelation(_hyponym, "Unterklasse", is_transitive); // hyponym
	Owner = addRelation(_Owner, "von", is_transitive);
	Member = addRelation(_Member, "hat");
	//	MEMBER_OF_THIS_DOMAIN_CATEGORY=
	addRelation(_MEMBER_DOMAIN_CATEGORY, "contextual"); //aviation: to overfly
	//	MEMBER_OF_THIS_DOMAIN_REGION =
	addRelation(_MEMBER_DOMAIN_REGION, "regional"); //-r IN Japan : Sushi
	//	MEMBER_OF_THIS_DOMAIN_USAGE =
	addRelation(_MEMBER_DOMAIN_USAGE, "Domaine"); // colloquialism: bitch
	PartOf = addRelation(_PartOf, "Teil von", is_transitive);
	Part = addRelation(_Part, "Teil", is_transitive);
	//	PARTICIPLE_OF_VERB =
	addRelation(_PARTICIPLE_OF_VERB, "Partizip");
	PERTAINYM = addRelation(_PERTAINYM, "Abgeleitet"); // # cellular(a) \ cell(n) 	equally(adv)-equal(adj)
	Synonym = addRelation(_synonym, "Synonym", is_transitive); // similar?? 32??
	addRelation(_SubstanceOwner, "Substanz von", is_transitive);
	Substance = addRelation(_Substance, "Substanz", is_transitive);
	addRelation(_VERB_GROUP, "Verb Gruppe");
	//	RELATIONSHIP_COUNT =
	//	addRelation(25, "RELATIONSHIP_COUNT");
	addRelation(_DOMAIN, "Domaine");
	addRelation(_DOMAIN_MEMBER, "Hat");
	//	 Synonym = Relation(32, "synonym");// -> 21????? See see | tag
	Type = addRelation(_Type, "Typ");// is_a transitive?? // (instance->class) !=SuperClass
	Instance = addRelation(_instance, "Instanz", is_transitive);

	Weight = addRelation(_weight, "Gewicht");
	Active = addRelation(_active, "Activ");
	Passive = addRelation(_passive, "Passiv");
	Tag = addRelation(_tag, "tag"); // different to 'unknown' !! / label ?
	Label = addRelation(_label, "Label");
	Labeled = addRelation(_labeled, "Label von");
	Similar = addRelation(_similar, "aehnlich");// hypernym?? no synonym
//	See = 	addRelation(_see_also, "auch");// hypernym??
	Category = addRelation(_category, "Kategorie"); // tag
	Topic = addRelation(_topic, "Topic");// todo Deutsch!
	SubContext = addRelation(_subcontext, "Subcontext"); // tag
	SuperContext = addRelation(_supercontext, "Supercontext"); //
	Comment = addRelation(_comment, "Kommentar");
	Description = addRelation(_description, "Beschreibung");

	Internal = addRelation(_internal, "intern"); //ok
	_Node = addRelation(_node, "Knoten");
	Abstract = addRelation(_abstract, "Abstract");
	Class = addRelation(_clazz, "Klasse");
//	Object = addRelation(_object, "Object");
	Object = Entity = addRelation(_entity, "Entität");
	Relation = addRelation(_relation, "Relation");
	Pattern = addRelation(_pattern, "Muster");
	Reification = addRelation(_reification, "Reifikation");

	// Thing  = addRelation(101, "thing");
	// Item  = addRelation(101, "item");
	Person = addRelation(_person, "Person");
	Adjective = addRelation(adjective, "Adjective");
	Noun = addRelation(noun, "Nomen");
	Verb = addRelation(verb, "Verb");
	Adverb = addRelation(adverb, "Adverb");
	Number = addRelation(_number, "Zahl");
	Unit = addRelation(unit, "Einheit");

	Array = addRelation(_array, "Liste");
	List = addRelation(_list, "Liste");
	Map = addRelation(_map, "Hash");
	Bytes = addRelation(_bytes, "Daten");

	Plural = addRelation(_plural, "Plural");
	Translation = addRelation(translation, "Übersetzung", is_transitive);// is_transitive if not mapped to abstract!

	And = addRelation(_And, "Und");
	Or = addRelation(_Or, "Oder");
	Not = addRelation(_Not, "Nicht");
	Any = addRelation(_any, "*");


	Equals = addRelation(_Equals, "=");
	Greater = addRelation(_Greater, ">");
	More = Greater;
	Less = addRelation(_Less, "<"); //smaller
	Smaller = Less;
	Between = addRelation(_Between, "Between");
	Circa = addRelation(_Circa, "Circa");
	Much = addRelation(_Much, "much");
	Little = addRelation(_Little, "little");
	Very = addRelation(_Very, "very");
	Contains = addRelation(_Contains, "Contains");
	StartsWith = addRelation(_StartsWith, "starts with");
	EndsWith = addRelation(_EndsWith, "ends with");

	String = addRelation(_string, "String");
	Date = addRelation(_date, "Datum");
	Float = addRelation(_float, "Float");
	Integer = addRelation(_integer, "Integer");
	Range = addRelation(_range, "Bereich");

//	True = addRelation(_true, "Wahr");
//	False = addRelation(_false, "Falsch");// todo


	From = addRelation(_from, "Von");
	To = addRelation(_to, "Bis");// VS NACH!!
	To = addRelation(_to, "Bis");// VS NACH!!
	Next = addRelation(_next, "Next");
	Previous = addRelation(_previous, "Previous");
	addSynonymsDE();
}


void initRelations() {
//  if(testing){p("testing");return;}
//    p("initRelations");
	context = getContext(current_context);
	context->nodeNames=name_root;
	name_root=context->nodeNames;// set to correct language

	if (context->currentNameSlot == 0)
		context->currentNameSlot++;// not 0!
	if (germanLabels) {
		initRelationsDE();
		return;
	}
	Unknown = addRelation(_unknown, "unknown");
	See = addRelation(_see_also, "see also");

	bool is_transitive = true;
	Instance = addRelation(_instance, "instance", is_transitive);
	Any = addRelation(_any, "?");

	Error = addRelation(_error, "Error");
	Missing = addRelation(_missing, "Missing");
	Antonym = addRelation(_antonym, "opposite");// antonym
//	Part = addRelation(1, "part"); USA etc BUG!?!!
	Attribute = addRelation(_attribute, "attribute"); // tag
	Property = Attribute;
	Cause = addRelation(_cause, "cause", is_transitive); //
	Derived = addRelation(_derived, "derived"); //
	Derives = addRelation(_derives, "derives"); //
//	DerivedFromNoun =addRelation(_derived_from_noun, "derived from noun"); //
	//    DOMAIN_OF_SYNSET_CATEGORY =
	UsageContext = addRelation(_DOMAIN_CATEGORY, "usage context"); // # sheet @ maths  // think of!! OWNER -> Part
	//  DOMAIN_OF_SYNSET_REGION =
	addRelation(_DOMAIN_REGION, "usage region"); // mate @ australia
	//  DOMAIN_OF_SYNSET_USAGE =
	Domain = addRelation(_DOMAIN_USAGE,
	                     "usage domain"); // #bitch @ colloquialism  or fin de siecle @ French  # fuck(vulgar)
	//  ENTAILMENT =
	addRelation(_ENTAILMENT, "entails", is_transitive); //ENTAILMENT jump implies come down
	SuperClass = addRelation(_Hypernym, "superclass", is_transitive); //Parent ,"Hypernym"
	Parent = SuperClass;
	//	IsA = SuperClass;

	SubClass = addRelation(_hyponym, "subclass", is_transitive); // hyponym
	Owner = addRelation(_Owner, "of", is_transitive);
	Member = addRelation(_Member, "has");
	//	MEMBER_OF_THIS_DOMAIN_CATEGORY=
	addRelation(_MEMBER_DOMAIN_CATEGORY, "contextual word"); //aviation: to overfly
	//	MEMBER_OF_THIS_DOMAIN_REGION =
	addRelation(_MEMBER_DOMAIN_REGION, "regional word"); //-r IN Japan : Sushi
	//	MEMBER_OF_THIS_DOMAIN_USAGE =
	addRelation(_MEMBER_DOMAIN_USAGE, "domain word"); // colloquialism: bitch
	PartOf = addRelation(_PartOf, "part of", is_transitive);
	Part = addRelation(_Part, "part", is_transitive);
	//	PARTICIPLE_OF_VERB =
	addRelation(_PARTICIPLE_OF_VERB, "participle");
	PERTAINYM = addRelation(_PERTAINYM, "pertainym"); // # cellular(a) \ cell(n) 	equally(adv)-equal(adj)

	Synonym = addRelation(_synonym, "synonym", is_transitive); // similar?? 32??
	//  Similar = addRelation(21, "similar");//synonym ??

	//	SubstanceOwner=
	addRelation(_SubstanceOwner, "substance of", is_transitive);
	Substance = addRelation(_Substance, "substance", is_transitive);
	//	VERB_GROUP=
	addRelation(_VERB_GROUP, "verb group");
	//	RELATIONSHIP_COUNT =
//	addRelation(25, "RELATIONSHIP_COUNT");
	addRelation(_DOMAIN, "domain");
	addRelation(_DOMAIN_MEMBER, "DOMAIN_MEMBER");// ??


	Weight = addRelation(_weight, "weight");
	Type = addRelation(_Type, "type");// is_transitive?? // (instance->class) !=SuperClass
	Active = addRelation(_active, "active");
	Passive = addRelation(_passive, "passive");
	Tag = addRelation(_tag, "tag"); // different to 'unknown' !!
	Label = addRelation(_label, "label");
	Labeled = addRelation(_labeled, "label of");
	Similar = addRelation(_similar, "similar");// hypernym?? no synonym
//	See = addRelation(_see_also, "also");// hypernym??
	Description = addRelation(_description, "description");

	//	Labels = addRelation(40, "Label");//??
	//	LabeledNode = addRelation(41, "LabeledNode");// ?? ugly!!
	Topic = addRelation(_topic, "topic");// same as: ?
	Category = addRelation(_category, "category"); // tag
	SubContext = addRelation(_subcontext, "subcontext"); // tag
	SuperContext = addRelation(_supercontext, "supercontext"); //
	Comment = addRelation(_comment, "comment");

	// Node Types!
	/*
	31 Weight
	32 Unknown
	33 Type
	34 Instance
	35 Active
	36 Passive
	37 Tag
	38 Label
	39 Labeled
	41 LabeledNode
	42 Attribute
	 */
	//Context= addRelation(99, "context");
	Internal = addRelation(_internal, "internal"); //ok
	_Node = addRelation(_node, "node");
	Abstract = addRelation(_abstract, "abstract");
	Class = addRelation(_clazz, "class");
	Object = addRelation(_object, "object");
	Entity = addRelation(_entity, "entity");
	Relation = addRelation(_relation, "relation");
	Pattern = addRelation(_pattern, "pattern");
	Reification = addRelation(_reification, "reification");

	// Thing  = addRelation(101, "thing");
	// Item  = addRelation(101, "item");
	Person = addRelation(_person, "person");
	Adjective = addRelation(adjective, "adjective");
	Noun = addRelation(noun, "noun");
	Verb = addRelation(verb, "verb");
	Adverb = addRelation(adverb, "adverb");
	Number = addRelation(_number, "number");
	Unit = addRelation(unit, "unit");
	Array = addRelation(_array, "array");

	Plural = addRelation(_plural, "plural");
	Translation = addRelation(translation, "translation", is_transitive);// is_transitive if not mapped to abstract!

	And = addRelation(_And, "and");
	Or = addRelation(_Or, "or");
	Not = addRelation(_Not, "not");

	Equals = addRelation(_Equals, "=");
	Greater = addRelation(_Greater, ">");// super:derived
	More = Greater;
	Less = addRelation(_Less, "<"); //smaller
	Smaller = Less;
	Between = addRelation(_Between, "Between");
	Circa = addRelation(_Circa, "Circa");
	Much = addRelation(_Much, "much");
	Little = addRelation(_Little, "little");// vs less!
	Very = addRelation(_Very, "very");
	Contains = addRelation(_Contains, "Contains");
	StartsWith = addRelation(_StartsWith, "starts with");
	EndsWith = addRelation(_EndsWith, "ends with");

	String = addRelation(_string, "String");
	Date = addRelation(_date, "Date");
	Float = addRelation(_float, "Float");
	Integer = addRelation(_integer, "Integer");

	List = addRelation(_list, "List");
	Map = addRelation(_map, "Hash");
	Bytes = addRelation(_bytes, "Data");
	Array = addRelation(_array, "Array");

	Range = addRelation(_range, "Range");

//	True = addRelation(_true, "True");
//	False = addRelation(_false, "False");// todo 0 'see'

	From = addRelation(_from, "From");
	To = addRelation(_to, "To");
	Next = addRelation(_next, "Next");
	Previous = addRelation(_previous, "Previous");

	addSynonyms();
}


void addLabel(Node *relation, const char* lable) {
//	if(lable[0]<'a')lable[0] += ('A' - 'a');// make upper case
	addStatement(relation, Label, getThe(lable));
}


void addSynonymsDE(){

//	>
	addLabel(Greater, "mehr");
	addLabel(Greater, "höher");
	addLabel(Greater, "hoeher");
	addLabel(Greater, "größer");
	addLabel(Greater, "groesser");
	addLabel(Greater, "groeßer");
	addLabel(Greater, "grösser");

//	<
	addLabel(Smaller, "kleiner");
	addLabel(Smaller, "weniger");
	addLabel(Smaller, "geringer");

//	=
	addLabel(Equals, "gleich");
	addLabel(Equals, "genau");
	addLabel(Equals, "exakt");

//	~
	addLabel(Circa, "etwa");
	addLabel(Circa, "ungefähr");
	addLabel(Circa, "ungefaehr");
//	addLabel(Circa, "ziemlich");
//	addLabel(Circa, "plusminus");
//	addLabel(Circa, "plus minus");
	addLabel(Circa, "mehr oder weniger");
}


void addSynonyms(){
	if(germanLabels)
		addSynonymsDE();
	addLabel(Greater, "larger");
	addLabel(Greater, "taller");
	addLabel(Greater, "higher");
	addLabel(Greater, "bigger");
	addLabel(Less, "little");// vs much!
	addLabel(Less, "smaller");
	addLabel(Less, "inferior");
}




Node *invert(Node *relation) {
	if (relation == 0)return Error;
	if (relation == Synonym)return Synonym;
	if (relation == Label)return Label;
	if (relation == Unknown)return Unknown;
	if (relation == Antonym)return Antonym;
	if (relation == Parent)return SubClass; //?Instance;
	if (relation == SuperClass)return SubClass;
	if (relation == SubClass)return SuperClass;
	//if(relation==Cause)return ;
	//if(relation==Entailment)return ; // Implication
	if (relation == Owner)return Member;
	if (relation == Member)return Owner;
	if (relation == Part)return PartOf;
	if (relation == PartOf)return Part;
	if (relation == Substance)return PartOf; //?
	//if(relation==Weight)return ??;
	if (relation == Type)return Instance;
	if (relation == Instance)return Type;
	if (relation == Active)return Passive;
	if (relation == Passive)return Active;
	if (relation == Tag)return Tag; //gged
	if (relation == Label)return Labeled;
	if (relation == Labeled)return Label;
//	if(relation==Labels)return LabeledNode;
//	if(relation==LabeledNode)return Label;
	//if(relation==Category)return ;
	if (relation == SubContext)return SuperContext;
	if (relation == SuperContext)return SubContext;

	if (relation->id == _similar)return get(_see_also);
	if (relation->id == _see_also)return get(_similar);// wtf ;)

	if (relation->id == _DOMAIN_MEMBER)return get(_DOMAIN);
	if (relation->id == _DOMAIN)return get(_DOMAIN_MEMBER);

	if (relation->id == _DOMAIN_CATEGORY)return get(_MEMBER_DOMAIN_CATEGORY);
	if (relation->id == _DOMAIN_REGION)return get(_MEMBER_DOMAIN_REGION);
	if (relation->id == _DOMAIN_USAGE)return get(_MEMBER_DOMAIN_USAGE);
	if (relation->id == _MEMBER_DOMAIN_CATEGORY)return get(_DOMAIN_CATEGORY);
	if (relation->id == _MEMBER_DOMAIN_REGION)return get(_DOMAIN_REGION);
	if (relation->id == _MEMBER_DOMAIN_USAGE)return get(_DOMAIN_USAGE);


	if (relation == Derived)return Derived;
//	if (relation == Derived)return Adjective;
//	if (relation == DerivedFromNoun)return Noun;

	if (relation == Equals)return Not;
	if (relation == Greater)return Less;
	if (relation == Less)return Greater;
	if (relation == Much)return Little;
	if (relation == Little)return Much;
	if (has(relation, Antonym, Any, false, false, false, false))
		return has(relation, Antonym);
//	if(contains(relation->name," of "))return getThe(cut_to(relation->name, " of "));
	else return Unknown;// null; //! or relation .name + "OF" ? weight => weight OF
}

// expansive! todo! merge later
Node *getRelation(const char *thing) {
//	N wiki=getWikidataRelation(field);
//	if(wiki)return wiki;
	if (thing[0] == '#') thing++;
	if (eq(thing, "altLabel"))return Label;
	if (eq(thing, "name"))return Label;
	if (eq(thing, "lable"))return Label;
	if (eq(thing, "Lable"))return Label;
	if (eq(thing, "label"))return Label;
	if (eq(thing, "Label"))return Label;
	if (eq(thing, "Name"))return Label;
	if (eq(thing, "prefLabel"))
		return 0;// ignore!


	if (eq(thing, "Image"))return get(-10018);
	if (eq(thing, "Bild"))return get(-10018);

	if (eq(thing, "Topic"))return Topic;

	if (eq(thing, "Item"))
		return Entity;// <http://www.wikidata.org/entity/Q9486626> <#type> <http://www.wikidata.org/ontology#Item> .
//	if (eq(thing, "Item"))return Object;// Entity
	if (eq(thing, "instance")) return Instance;
	if (eq(thing, "Contains")) return Part;
//  if (eq(thing, "broader")) return Unknown;//Any;// Related, See Also SuperClass or Type
	if (eq(thing, "Broader")) return SuperClass;
	if (eq(thing, "Broader topic")) return SuperClass;
//	if (eq(thing, "topic")) return Topic;
	if (eq(thing, "narrower")) return SubClass;
	if (eq(thing, "narrower topic")) return SubClass;
	if (eq(thing, "Unterklasse von"))return SuperClass;
//  if (eq(thing, "Organism type")) return SuperClass;
	//  if (eq(thing, "Species")) return SuperClass;
	//  if (eq(thing, "Breed of")) return SuperClass;
//  Higher Classification
	if (eq(thing, "type")) return Type;// lustiger Typ !!! --------- !!!!!!!!
	if (eq(thing, "has type")) return Type;
	if (eq(thing, "is")) return Type;
	if (eq(thing, "ist")) return Type;
//  	if (eq(thing, "TEILVON")) return PartOf;
	if (eq(thing, "has")) return Member;
	if (eq(thing, "of")) return Owner;
	if (eq(thing, "containedby")) return PartOf;
	if (eq(thing, "partOf")) return PartOf;
	if (eq(thing, "part")) return Part;
	if (eq(thing, "Besteht aus")) return Part;
	if (eq(thing, "by")) return Owner; // creator
	if (eq(thing, "type")) return Type;
	if (eq(thing, "property")) return Attribute; // Property;
	if (eq(thing, "inverseOf"))return Antonym;
	if (eq(thing, "reverse property"))return Antonym;
	if (eq(thing, "true"))return True;
	if (eq(thing, "false")) return False;
	if (eq(thing, "label")) return Label;
	if (eq(thing, "range")) return Range;
	if (eq(thing, "domain")) return Domain;
	if (eq(thing, "inverseOf")) return Antonym;
	if (eq(thing, "Antonym")) return Antonym;
	if (eq(thing, "see")) return See;
	if (eq(thing, "seeAlso")) return See;
	if (eq(thing, "see also")) return See;
	if (eq(thing, "also see")) return See;
	if (eq(thing, "also")) return See;
	if (eq(thing, "similar"))return Similar;
//	if (eq(thing, "next"))return Prede;
	if (eq(thing, "Ist ein(e)")) return Type;// -10031
	if (eq(thing, "Typ")) return Type;
	if (eq(thing, "Art")) return Type;
	if (eq(thing, "sameAs")) return Synonym; // instance of
	if (eq(thing, "Synonym")) return Synonym; // instance of
	if (eq(thing, "subclass of")) return SuperClass;
	if (eq(thing, "subClassOf")) return SuperClass;
	return 0;// no relation
}

// https://www.wikidata.org/wiki/Wikidata:List_of_properties
// WIKIDATA additional useful/structural relations
Node *getWikidataRelation(const char *thing) {
	// e.g. _categorys_main_topic https://www.wikidata.org/wiki/Property:P301
	if (thing[0] == '<')thing++;

	if (eq(thing, "P31")) return Type; // instance of
//	if (eq(thing, "P18")) return getThe("image");
	if (eq(thing, "P279")) return SuperClass;
	if (eq(thing, "P361")) return PartOf;
	if (eq(thing, "P527")) return Part;// Holonym= has-part : transitive property!
	if (eq(thing, "P127")) return Owner;
//	if (eq(thing, "P131")) return In;// 1/PartOf;// 'located in administrative...'
//	if (eq(thing, "P706")) return In;// 1/PartOf;// 'located in geographic'

	if (eq(thing, "P1696")) return Antonym;// Opposite;
	if (eq(thing, "P461")) return Antonym; // AntonymOf == Antonym LOL
	if (eq(thing, "P2888")) return Synonym; //	-12888 Exakte Übereinstimmung

//	if (eq(thing, "P155")) return Previous; //predecessor
//	if (eq(thing, "P156")) return Next;// follows

//	527> <label> "besteht aus"@de .

	if (eq(thing, "P279")) return SuperClass;// ist Unterklasse von
	if (eq(thing, "P523")) return Part;// 	Has part
	if (eq(thing, "P460")) return Synonym; // Eventuell gleichwertig / Als gleichwertig bezeichnet how?


	if (eq(thing, "P585")) return Date;
//	if (eq(thing, "P580")) return Of;// aus in im von
//	if (eq(thing, "P580")) return From;// start time
//	if (eq(thing, "P582")) return To;// end time !
	if (eq(thing, "P560")) return To;// Richtung

	// Unicode character 	P487
	// use 	P366 	Item 	use: main use of the subject
	// facet of 	P1269 	Item
	// different from 	P1889 	Item

	//	551> <altLabel> "wohnt in"@de . < in
//	558> <altLabel> "Einheitensymbol"@de .
//	566> <description> "Synonym eines gültigen wissenschaftlichen Namens, von dem dieser abgeleitet ist"@de .
//	566> <label> "Basionym"@de . NOOO lol
//	571> <altLabel> "Erstellungsdatum"@de . Gründungsdatum etc
//575> <label> "Entdeckungsdatum"@de .
	//	576> <label> "Auflösungsdatum"@de .
//	577> <altLabel> "Erscheinungsdatum"@de .
//	619> <description> "Zeitangabe des Starts eines Raumfahrzeugs"@de . NOOOO LOL

//	585> <altLabel> "Datum"@de .
//	585> <altLabel> "Stand"@de .
//	585> <label> "Zeitpunkt"@de .
//625> <altLabel> "Geokoordinaten"@de .
//	if (eq(thing, "P706")) return Place;// in Veranstaltungsort, liegt in etc !!
//	840> <altLabel> "Ort der Handlung"@de . NOO
//	794> <description> "generischer Qualifikator"@de .
//	794> <label> "als"@de .

//	if (eq(thing, "P856")) return getThe("URL");

//1311> <description> "Datenbank gedeckter Brücken in den Vereinigten Staaten und Kanada"@de . WTF!!!?? LOL

//		if (eq(thing, "P18")) return Image;
//	18> <label> "Bild"@de . Abbildung Foto Grafik
//	58> <altLabel> "Autor"@de .
//	103> <altLabel> "Sprache"@de . Muttersprache
//	364> <altLabel> "Sprache"@de . Sprache des Originalwerks
//373> <label> "Commons-Kategorie"@de . DROP!!!! ohne Category: KEEP! interesting!
//	367> <altLabel> "Symbol"@de .
//	366> <altLabel> "Zweck"@de . usage
	//	136> <label> "Genre"@de .etc
//	248> <altLabel> "steht in"@de . quelle/source
//	1343> <altLabel> "Quelle"@de .
//	P1365 replaces ancestor
//	1366> <altLabel> "ersetztes"@de . predecessor succeeded by
//	if (eq(thing, "P131")) return PartOf;//located in the administrative
//	if (eq(thing, "P706")) return PartOf;// located on terrain

//	<P111> <altLabel> "misst"@de . <_> https://www.wikidata.org/wiki/Property:P1880 gemessen in
//  if (eq(thing, "Klasse")) return SuperClass;
//	if (eq(thing, "reverse_property")) return Owner;
	//	if (eq(thing, "in"))return Loc;
	return 0;
//	return getThe(thing);
}


void initWikiRelations() {
// done above:
	//	addStatement4(current_context,-10279,_SuperClass,_SubClass);//Unterklasse von
	//	addStatement4(current_context,-10027,_SuperClass,_SubClass);//Unterklasse von
	//	addStatement4(current_context,-10031,_SuperClass,_SubClass);//ist ein :(
	//	addStatement4(current_context,-10460,_SuperClass,_Synonym);// Als gleichwertig bezeichnet
	//	addStatement4(current_context,-10361, _SuperClass, _PartOf);// DIRECTLY!

	//	https://www.wikidata.org/wiki/Property:P131
//		addStatement4(current_context,-10131, _SuperClass, In->id);
	addStatement4(current_context, -10131, _SuperClass, _PartOf);//located in the administrative
	addStatement4(current_context, -10150, _SuperClass, _Part);// Untereinheit (administrative Einheit)
	addStatement4(current_context, -10527, _SuperClass, _Part);// 	Has part
	addStatement4(current_context, -10706, _SuperClass, _PartOf);// located on terrain

	addStatement4(current_context, -10035, _synonym, getThe("President")->id);
	addStatement4(current_context, -10035, _synonym, getThe("Präsident")->id);// head of state

	addStatement4(current_context, -10560, _SuperClass, _to);// 	direction

//	if(endsWith(nam, " of"))
//		addStatement4(current_context,-10560, _SuperClass, _PartOf);// 	direction

	//	560> <label> "Richtung"@de .
	//	551> <altLabel> "wohnt in"@de . < in

	//	getEntity( "P131")) return In;//PartOf;// 'located in administrative...'
	//	if (eq(thing, "P706")) return In;// PartOf;// 'located in geographic'
}
