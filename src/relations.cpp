/*
 * File:   relations.cpp
 * Author: me
 *
 * Created on March 28, 2012, 10:25 PM
 */
#include "util.hpp"
#include "relations.hpp"


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
Node* Weight;
Node* Type;
Node* Instance;
Node* Active;
Node* Passive;
Node* Tag;
Node* Label;
Node* BackLabel;
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

Node* addRelation(int id, const char* name) {
	string keep = name;
	const char* what;
	what = keep.c_str();
	Node* n = add_force(wordnet, id, what, _internal);
	return n;
}

void initRelations() {
	Unknown = addRelation(0, "see");
	Antonym = addRelation(1, "antonym");
	Attribute = addRelation(2, "attribute"); // tag
	Cause = addRelation(3, "cause"); //
	Derived =addRelation(4, "derived from adjective"); //
	DerivedFromNoun =addRelation(5, "derived from noun"); //
	//        DOMAIN_OF_SYNSET_CATEGORY =
	addRelation(6, "usage context"); // # sheet @ maths   // think of!! OWNER -> Part
	//    DOMAIN_OF_SYNSET_REGION =
	addRelation(7, "usage region"); // mate @ australia
	//    DOMAIN_OF_SYNSET_USAGE =
	addRelation(8, "usage domain"); // #bitch @ colloquialism  || fin de siecle @ French   # fuck(vulgar)
	//    ENTAILMENT =
	addRelation(9, "entails"); //ENTAILMENT  jump implies come down
	SuperClass = addRelation(10, "superclass"); //Parent ,"Hypernym"
	Parent = SuperClass;
	//	IsA = SuperClass;

	SubClass = addRelation(11, "subclass"); // hyponym
	Owner = addRelation(12, "of");
	Member = addRelation(13, "has");
	//	MEMBER_OF_THIS_DOMAIN_CATEGORY=
	addRelation(14, "contextual word"); //aviation: to overfly
	//	MEMBER_OF_THIS_DOMAIN_REGION =
	addRelation(15, "regional word"); //-r  IN Japan :  Sushi
	//	MEMBER_OF_THIS_DOMAIN_USAGE =
	addRelation(16, "domain word"); // colloquialism: bitch
	//    PartOwner =
	addRelation(17, "part of");
	Part = addRelation(18, "part");
	//	PARTICIPLE_OF_VERB =
	addRelation(19, "PARTICIPLE_OF_VERB");
	//	PERTAINYM =
	addRelation(20, "PERTAINYM"); //  # cellular(a) \ cell(n) 	equally(adv)-equal(adj)

	Synonym = addRelation(21, "synonym"); // similar?? 32??
	//    Similar = addRelation(21, "similar");//synonym ??

	//	SubstanceOwner=
	addRelation(22, "substance of");
	Substance = addRelation(23, "substance");
	//	VERB_GROUP=
	addRelation(24, "VERB_GROUP");
	//	RELATIONSHIP_COUNT =
	addRelation(25, "RELATIONSHIP_COUNT");

	Weight = addRelation(31, "weight");
	//	 Synonym = Relation(32, "synonym");// -> 21????? See see | tag
	Type = addRelation(33, "type"); // (instance->class) !=SuperClass
	Instance = addRelation(34, "instance");
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
	Integer = addRelation(_range, "Range");
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