/*
 * File:   relations.cpp
 * Author: me
 *
 * Created on March 28, 2012, 10:25 PM
 */
#include "util.hpp"
#include "relations.hpp"
// DONT depend on netbase!

Node* Unknown;
Node* Antonym;
Node* Parent;
Node* SuperClass; // Parent
// Node* IsA;// Parent
Node* SubClass;
Node* Cause;
Node* Entailment; // Implication
Node* Owner; //Owner inverse Member   (Prince of Persia) <=> (Persia has Prince)
Node* Member;
Node* Part;
Node* Substance;
Node* Synonym;
Node* Domain;

Node* Weight;
Node* Type;
Node* Instance;

Node* Active;
Node* Passive;
Node* Tag;
Node* Label;
Node* BackLabel;
Node* Comment;
Node* Labels;
Node* LabeledNode;
Node* Category;
Node* SubContext;
Node* SuperContext;

// Types
Node* Internal;
Node* _Node;
Node* Thing;
Node* Item;
Node* Abstract; // Thing
Node* Class;
Node* Object;
Node* Relation;
Node* Reification;
Node* Pattern; // temporary

Node* Any;
// Semantics
Node* Attribute; //ok, if  Node* Attribute declared in netbase.cpp
// Node* Is; // danger: obama is president etc

Node* Person;
Node* Adjective;
Node* Noun;
Node* Verb;
Node* Adverb;

Node* Derived;// Adjective^-1
Node* DerivedFromNoun;

Node* Number;
Node* String;
Node* Date;
Node* Float;
Node* Integer;
Node* Range;
Node* Unit;

Node* Translation;
Node* Plural;
Node* And;
Node* Or;
Node* Not;

//There are about 150 prepositions in English
Node* Of; // ==owner
Node* In; // ==location,...
Node* To; // ==direction,<object>
Node* From;
Node* By;
Node* For;
Node* On;


Node* Equals;
Node* Greater;
Node* Less; //smaller
Node* Between;
Node* Circa;
Node* Much;
Node* Little;
Node* Very;
Node* Contains;
Node* StartsWith;
Node* EndsWith;

Node* addRelation(int id, const char* name,bool transitive=false) {
	string keep = name;
	const char* what;
	what = keep.c_str();
	Node* n = add_force(wordnet, id, what, _internal);
//	if(transitive)???  baked into Algorithms, at least four standard relations?
	return n;
}
int _see=0,//50 also, 40 similar
	_Hypernym=1,//SuperClass
	_hyponym=2,//SubClass
	_Type=3,
	_instance=4,
	_ENTAILMENT=21,// implies
	_Owner=13,//??
	_Member=14,
	_PartOwner=11,
	_Part=12,
	_SubstanceOwner=15,
	_Substance=16,
	_PARTICIPLE_OF_VERB=71,
	_PERTAINYM=80,
	_antonym=30,
	_attribute=60,
	_cause=23,
	_derived=81,
//	_derived_from_noun=81,//?
	_synonym=21,//32?
	_DOMAIN_CATEGORY=91,
	_DOMAIN_REGION=93,
	_DOMAIN_USAGE=95,
	_MEMBER_DOMAIN_CATEGORY=92,
	_MEMBER_DOMAIN_REGION=94,
	_MEMBER_DOMAIN_USAGE=96,
	_VERB_GROUP=70;
//int _see=0,
//	_antonym=1,
//	_attribute=2,
//	_cause=3,
//	_derived=4,
//	_derived_from_noun=5,
//	_DOMAIN_CATEGORY=6,
//	_DOMAIN_REGION=7,
//	_DOMAIN_USAGE=8,
//	_ENTAILMENT=9,// implies
//	_Hypernym=10,//SuperClass
//	_hyponym=11,//SubClass
//	_Owner=12,
//	_Member=13,
//	_MEMBER_DOMAIN_CATEGORY=14,
//	_MEMBER_DOMAIN_REGION=15,
//	_MEMBER_DOMAIN_USAGE=16,
//	_PartOwner=17,
//	_Part=18,
//	_PARTICIPLE_OF_VERB=19,
//	_PERTAINYM=20,
//	_synonym=21,//32?
//	_SubstanceOwner=22,
//	_Substance=23,
//	_VERB_GROUP=24,
//	_Type=33,
//	_instance=34;




;
void initRelations() {
	Unknown = addRelation(_see, "see");
	Antonym = addRelation(_antonym, "antonym");
//	Part = addRelation(1, "part"); USA etc BUG!?!!
	Attribute = addRelation(_attribute, "attribute"); // tag
	bool is_transitive=true;
	Cause = addRelation(_cause, "cause",is_transitive); //
	Derived =addRelation(_derived, "derived from adjective"); //
//	DerivedFromNoun =addRelation(_derived_from_noun, "derived from noun"); //
	//        DOMAIN_OF_SYNSET_CATEGORY =
	addRelation(_DOMAIN_CATEGORY, "usage context"); // # sheet @ maths   // think of!! OWNER -> Part
	//    DOMAIN_OF_SYNSET_REGION =
	addRelation(_DOMAIN_REGION, "usage region"); // mate @ australia
	//    DOMAIN_OF_SYNSET_USAGE =
	Domain=addRelation(_DOMAIN_USAGE, "usage domain"); // #bitch @ colloquialism  || fin de siecle @ French   # fuck(vulgar)
	//    ENTAILMENT =
	addRelation(_ENTAILMENT, "entails",is_transitive); //ENTAILMENT  jump implies come down
	SuperClass = addRelation(_Hypernym, "superclass",is_transitive); //Parent ,"Hypernym"
	Parent = SuperClass;
	//	IsA = SuperClass;

	SubClass = addRelation(_hyponym, "subclass",is_transitive); // hyponym
	Owner = addRelation(_Owner, "of",is_transitive);
	Member = addRelation(_Member, "has");
	//	MEMBER_OF_THIS_DOMAIN_CATEGORY=
	addRelation(_MEMBER_DOMAIN_CATEGORY, "contextual word"); //aviation: to overfly
	//	MEMBER_OF_THIS_DOMAIN_REGION =
	addRelation(_MEMBER_DOMAIN_REGION, "regional word"); //-r  IN Japan :  Sushi
	//	MEMBER_OF_THIS_DOMAIN_USAGE =
	addRelation(_MEMBER_DOMAIN_USAGE, "domain word"); // colloquialism: bitch
	//    PartOwner =
	addRelation(_PartOwner, "part of",is_transitive);
	Part = addRelation(_Part, "part",is_transitive);
	//	PARTICIPLE_OF_VERB =
	addRelation(_PARTICIPLE_OF_VERB, "PARTICIPLE_OF_VERB");
	//	PERTAINYM =
	addRelation(_PERTAINYM, "PERTAINYM"); //  # cellular(a) \ cell(n) 	equally(adv)-equal(adj)

	Synonym = addRelation(_synonym, "synonym",is_transitive); // similar?? 32??
	//    Similar = addRelation(21, "similar");//synonym ??

	//	SubstanceOwner=
	addRelation(_SubstanceOwner, "substance of",is_transitive);
	Substance = addRelation(_Substance, "substance",is_transitive);
	//	VERB_GROUP=
	addRelation(_VERB_GROUP, "VERB_GROUP");
	//	RELATIONSHIP_COUNT =
//	addRelation(25, "RELATIONSHIP_COUNT");
	addRelation(97, "DOMAIN");
	addRelation(98, "MEMBER");// ??


	Weight = addRelation(31, "weight");
	//	 Synonym = Relation(32, "synonym");// -> 21????? See see | tag
	Type = addRelation(_Type, "type");// is_transitive?? // (instance->class) !=SuperClass
	Instance = addRelation(_instance, "instance",is_transitive);
	Active = addRelation(35, "active");
	Passive = addRelation(36, "passive");
	Tag = addRelation(37, "tag"); // different to 'unknown' !!
	Label = addRelation(38, "label");
	BackLabel = addRelation(39, "label of");
	//	Labels = addRelation(40, "Label");//??
	//	LabeledNode = addRelation(41, "LabeledNode");// ?? ugly!!
	Category = addRelation(43, "category"); // tag
	SubContext = addRelation(44, "subcontext"); // tag
	SuperContext = addRelation(45, "supercontext"); //
	Comment = addRelation(46, "comment");

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
	39 BackLabel
	41 LabeledNode
	42 Attribute
	 */
	//Context= addRelation(99, "context");
	Internal = addRelation(_internal, "internal"); //ok
	_Node = addRelation(_node, "node");
	Abstract = addRelation(_abstract, "abstract");
	Class = addRelation(_clazz, "class");
	Object = addRelation(_object, "object");
	Relation = addRelation(_relation, "relation");
	Pattern = addRelation(_pattern, "pattern");
	Reification = addRelation(_reification, "reification");

	// Thing   = addRelation(101, "thing");
	// Item    = addRelation(101, "item");
	Person = addRelation(_person, "person");
	Adjective = addRelation(adjective, "adjective");
	Noun = addRelation(noun, "noun");
	Verb = addRelation(verb, "verb");
	Adverb = addRelation(adverb, "adverb");
	Number = addRelation(number, "number");
	Unit = addRelation(unit, "unit");

	Plural = addRelation(plural, "plural");
	Translation = addRelation(translation, "translation");

	And = addRelation(_And, "and");
	Or = addRelation(_Or, "or");
	Not = addRelation(_Not, "not");
	Any = addRelation(666, "*");


	Equals = addRelation(_Equals, "=");
	Greater = addRelation(_Greater, ">");
	Less = addRelation(_Less, "<"); //smaller
	Between = addRelation(_Between, "Between");
	Circa = addRelation(_Circa, "Circa");
	Much = addRelation(_Much, "much");
	Little = addRelation(_Much, "little");
	Very = addRelation(_Very, "very");
	Contains = addRelation(_Contains, "Contains");
	StartsWith = addRelation(_StartsWith, "StartsWith");
	EndsWith = addRelation(_EndsWith, "EndsWith");

	String = addRelation(_string, "String");
	Date = addRelation(_date, "Date");
	Float = addRelation(_float, "Float");
	Integer = addRelation(_integer, "Integer");
	Range = addRelation(_range, "Range");

	addStatement(getThe("opposite"),SuperClass,Antonym);
//	addStatement(Antonym,Synonym,getThe("opposite"));

//	addStatement(Number,Synonym,getThe("xsd:decimal"));
//	addStatement(Number,Synonym,getThe("xsd:integer"));
//	addStatement(Date,Synonym,getThe("xsd:date"));
//	addStatement(getThe("xsd:date"),SuperClass,Date);
//	addStatement(getThe("xsd:decimal"),SuperClass,Number);
//	addStatement(getThe("xsd:integer"),SuperClass,Number);
//	addStatement(get("xsd:date"),SuperClass,Date);
//	addStatement(get("xsd:decimal"),SuperClass,Number);
//	addStatement(get("xsd:integer"),SuperClass,Number);
	addStatement(getAbstract("xsd:date"),SuperClass,Date);
	addStatement(getAbstract("xsd:decimal"),SuperClass,Number);
	addStatement(getAbstract("xsd:integer"),SuperClass,Number);
}

Node* invert(Node* relation) {
	if (relation == Unknown)return Unknown;
	if (relation == Antonym)return Antonym;
	if (relation == Parent)return SubClass; //?Instance;
	if (relation == SuperClass)return SubClass;
	if (relation == SubClass)return SuperClass;
	//if(relation==Cause)return ;
	//if(relation==Entailment)return ; // Implication
	if (relation == Owner)return Member;
	if (relation == Member)return Owner;
	if (relation == Part)return Owner;
	if (relation == Substance)return Owner; //?
	if (relation == Synonym)return Synonym;
	//if(relation==Weight)return ??;
	if (relation == Type)return Instance;
	if (relation == Instance)return Type;
	if (relation == Active)return Passive;
	if (relation == Passive)return Active;
	if (relation == Tag)return Tag; //gged
	if (relation == Label)return BackLabel;
	if (relation == BackLabel)return Label;
	if(relation==Labels)return LabeledNode;
	if(relation==LabeledNode)return Label;
	//if(relation==Category)return ;
	if (relation == SubContext)return SuperContext;
	if (relation == SuperContext)return SubContext;


	if (relation == Derived)return Adjective;
//	if (relation == DerivedFromNoun)return Noun;

	if (relation == Equals)return Not;
	if (relation == Greater)return Less;
	if (relation == Less)return Greater;
	if (relation == Much)return Little;
	if (relation == Little)return Much;
	if (has(relation, Antonym, null, false, false, false, false))
		return has(relation, Antonym);
	else return Unknown;// null; //! or relation .name + "OF" ?  weight => weight OF
}