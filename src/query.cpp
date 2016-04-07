#include "netbase.hpp"
#include "util.hpp"
#include "query.hpp"
#include "relations.hpp"

#include <cstdlib>
#include <string.h>
#include <algorithm> // std:reverse std:sort
//#include <multimap> // to sort map


int* enqueued; // 'parents'
NodeVector EMPTY;

string select(string s) {
	if (contains(s, "select"))
		return "";
	else
		return "select * from ";
}

string fixQuery(string s) {
	s = "^"+ s + string(" $");
	s = replace_all(s, "types of ", select(s)); //todo .*types of regex
	s = replace_all(s, "kinds of ", select(s));
	s = replace_all(s, "list of ", select(s));
	s = replace_all(s, "list ", select(s));
	s = replace_all(s, "species of ", select(s));
	s = replace_all(s, "different ", select(s));
	s = replace_all(s, "various ", select(s));
	s = replace_all(s, "show ", select(s));
	s = replace_all(s, "^all ", select(s)); // todo all fields of types that ...
	s = replace_all(s, "^some ", select(s));
	s = replace_all(s, "^the ", select(s));
	s = replace_all(s, "^a ", select(s)); //limit 1
	s = replace_all(s, "^these ", select(s));
	s = replace_all(s, "^those ", select(s));
	s = replace_all(s, "^an ", select(s));
	s = replace_all(s, "^any ", select(s));
	s = replace_all(s, " are ", " ");
	s = replace_all(s, " is ", " ");
	s = replace_all(s, " with ", " where ");
	s = replace_all(s, " which ", " where ");
	s = replace_all(s, " who ", " where ");
	s = replace_all(s, " whose ", " where ");
	s = replace_all(s, " who is ", " where ");
	s = replace_all(s, " that ", " where ");
	s = replace_all(s, " in ", " where "); //location=           ...
	s = replace_all(s, " of ", " where "); // owner= or location=
	s = replace_all(s, " that ", " where ");
	s = replace_all(s, "ies ", "y "); //singular
    if(!contains(s," where "))
        s = replace_all(s, "s ", " "); //singular singular
	s = replace_all(s, " $", ""); //trim hack
	s = replace_all(s, "^", ""); //trim hack
	return s;
}

Facet& findFacet(Query& q, Node* field) {
	if (q.facetMap.find(field) != q.facetMap.end())
		return *q.facetMap[field];
	Facet* f;
	for (int i = 0; i < q.facets.size(); i++) {
		f = q.facets[i];
		if (isA4(f->field, field)) {
			q.facetMap[field] = f;
			return *f;
		}
	}
	//    if q.autoFacet
	Facet& f2 = *new Facet(); // life cycle !?!
	f2.field = field;
	f2.type = get(field->kind);
	//    f2.values = new map<Node*, int>();
	q.facets.push_back(&f2);
	q.facetMap[field] = &f2;
	return f2;
}

int getFieldNr(Query& q, Node* predicate) {
    int nrFields=(int)q.fields.size();
	for (int i = 0; i < nrFields; i++) {
		Node* field = q.fields[i];
		if (field == predicate)
			return i;
	}
	for (int i = 0; i < nrFields; i++) {
		Node* field = q.fields[i];
		if (eq(predicate->name, field->name))
			return i;
		if (isA4(predicate, field, false, false))
			return i;
		if (isA4(field, predicate, false, false))
			return i;
	}
	return -1;
}

void collectFieldsAndFacets(Query& q) {
	NodeVector all = q.instances;
	int nrFields = (int)q.fields.size();
	int size=(int)all.size();
	for (int rowNr = 0; rowNr < size; rowNr++) {
		Node* n = all[rowNr];
		q.values[n] = (NodeList) malloc(sizeof (Node*) * nrFields);
		Statement* s = 0;
		while ((s = nextStatement(n, s, true))) {
            Node* subject=s->Subject();
            Node* predicate = s->Predicate();
            Node* object=s->Object() ;
            Node* value;
			if (subject == n) {
                value=object;
            }else if(object== n){
                //                predicate = invert(predicate);// reverse! -> 0 if none !!
				value = subject;
            }else{
                p("Predicate facets not allowed");
                continue;
            }
            if(predicate==Any)continue; // if reverse failed !
            
            // fieldValues for node
            int fieldNr = getFieldNr(q, predicate);
            if(fieldNr>nrFields)p("fieldNr>nrFields!?!!?");
            NodeList& fieldValues = q.values[n];
            if (fieldNr >= 0)// &&  !checkNode(nl[fieldNr]))// && nl[fieldNr]==0 don't Over wright todo : multiple values?nl[fieldNr]
                fieldValues[fieldNr] = value;
            
            //                addHit(q,predicate)
            Facet& f = findFacet(q, predicate);
            f.hits++;
            map<Node*, int>& values = *(f.values);
            if (values.find(value) != values.end()) {// 4m!=4m !?!?
                values[value] = values[value] + 1;
                f.maxHits = max(f.maxHits, values[value]);
            }//                if (values.find(value)!=values.end())// no
            //                    values[value] = values[value] + 1;
            //                    f->values[value] = f->values->[value] + 1;
            else {
                values[value] = 1;
                if (f.maxHits == 0)f.maxHits = 1;
            }
            //				else
            //					pf("ignoring predicate %d %s\n",predicate->id,predicate->name);
		}
        
	}
}

string renderResults(Query& q) {
	NodeVector all = q.instances;
	int nrFields = (int)q.fields.size();
	stringstream buffer; //(1000000, ' ');
    
	buffer << "<response>\n";
	buffer << "<lst name=\"responseHeader\">\n";
	buffer << "<int name=\"QTime\">0</int>\n";
	buffer << "<lst name=\"params\">\n";
	buffer << " <str name=\"q\">" << q.keyword->name << "</str>\n";
	buffer << " <str name=\"filters\">" << q.filters.size() << "</str>\n";
	buffer << " <str name=\"facets\">" << q.facets.size() << "</str>\n";
	buffer << " <int name=\"limit\">" << q.limit << "</int>\n";
	buffer << "</lst>\n";
	buffer << "</lst>\n";
    
    //	buffer << "<table name=\"response\" numFound=" << q.instances.size() << " start=" << q.start << " rows=" << q.hitsPerPage << ">\n";
	buffer << "<results numFound='" << q.instances.size() << "' start='" << q.start << "' rows='" << q.hitsPerPage << "'>\n";
	//    buffer << "<result name=\"response\" numFound=\"559\" start=\"0\">\n";
    
	// Field header
	buffer << "<th>\n";
    
	buffer << "   <td name=\"id\">id</td>\n";
	//		buffer << "   <td name=\"context\">context</td>\n";
	buffer << "   <td name=\"name\">name</td>\n";
	//		buffer << "   <td name=\"kind\">kind</td>\n";
	buffer << "   <td name=\"statementCount\">statementCount</td>\n";
	for (int columnNr = 0; columnNr < nrFields; columnNr++) {
		Node *f = q.fields[columnNr];
		string kind = "td"; // "str"
		buffer << "   <" << kind << " name=\"" << f->name << "\" field_id=" << f->id << ">" << f->name << "</" << kind << ">\n";
	}
	buffer << "</th>\n";
    
	// results
	for (int rowNr = 0; rowNr < minimum((int) all.size(), q.limit); rowNr++) {
		Node* n = all[rowNr];
		//    buffer << "<doc>\n";
		buffer << "<entity name='" << n->name << "' id='" << n->id << "' statementCount='" << n->statementCount << "'>\n";
		//    buffer << "<doc>\n";
		//		buffer << "<tr name='entity'>\n";
		//		buffer << "    <td name=\"id\">" << n->id << "</td>\n";
		////		buffer << "    <td name=\"context\">" << n->context << "</td>\n";
		//		buffer << "    <td name=\"name\">" << n->name << "</td>\n";
		////		buffer << "    <td name=\"kind\">" << n->kind << "</td>\n";
		//		buffer << "    <td name=\"statementCount\">" << n->statementCount << "</td>\n";
		NodeList values = q.values[n];
		for (int columnNr = 0; columnNr < nrFields; columnNr++) {
			Node *f = q.fields[columnNr];
			if(f==Any)continue;
			Node *v = values[columnNr];
            
			string kind = "field"; // "td"; // "str"
			if (!checkNode(v)) {
				buffer << "    <" << kind << " name='" << f->name << "' missing='true'/>\n";
				continue;
			} else
				buffer << "    <" << kind << " name=\"" << f->name << "\" value_id='" << v->id << "'>" << v->name << "</" << kind << ">\n";
		}
		buffer << "</entity>\n";
		//		buffer << "</tr>\n";
		//    buffer << "</doc>\n";
	}
	//	buffer << "</table>\n";
	buffer << "</results>\n";
    
	// renderFacets
	//	buffer << "<lst name=\"facet_counts\"/>\n";
	//    buffer << "<lst name=\"facet_queries\"/>\n";
	//	buffer << "<lst name=\"facet_fields\"/>\n";
	buffer << "<facets>\n";
	for (int i = 0; i < q.facets.size(); i++) {
		Facet& f = *q.facets[i];
		if (f.field == Synonym)continue;
		if (f.field == Member)continue;
		if (f.field == Derived)continue;
		//		if(f.field==SuperClass)continue;
		buffer << "<facet name=\"" << f.field->name << "\">\n";
		std::multimap<int, Node*> sorted;
		map<Node*, int>::iterator it = f.values->begin();
		while (it != f.values->end()) {
			sorted.insert(std::pair<int, Node*>(it->second, it->first));
			it++;
		}
        
        
		multimap<int, Node*>::iterator ti = sorted.end();
		int max = 0;
		while (ti-- != sorted.begin() && max++<q.maxFacets) {
			Node *slot = ti->second;
			int count = ti->first; // same as (*it).first   (the key value)
			if(count>1)// todo : switch
                buffer << "   <int name=\"" << slot->name << "\">" << count << "</int>\n";
		}
		buffer << "</facet>\n";
	}
	buffer << "</facets>\n";
	buffer << "</response>\n";
	return buffer.str();
}

NodeVector query(string s, int limit/*=resultLimit*/) {
	p(("Executing query "));
	ps(s);
	Query q = parseQuery(s, limit);
//    q.queryType=sqlQuery;// njet!
    NodeVector results=query(q);
    showNodes(results);
	return results;
    //	return evaluate_sql(s, limit);
}


NodeVector sqlTable(Query& q){
    NodeVector all=q.instances;
    NodeVector rows;
    if(q.fields.size()==0)return q.instances;
    if(q.fields.size()==1)
        for (int rowNr = 0; rowNr < minimum((int) all.size(), q.limit); rowNr++){
            Node* n = all[rowNr];
            if(q.values[n]){
                Node *v = q.values[n][0];
                if (checkNode(v))
                    rows.push_back(v);
                else
                    rows.push_back(n);//DEBUG:n Any
            }
            else  rows.push_back(n);// Any
        }
    //copy to dummy result
    if(q.fields.size()>1)
        for (int rowNr = 0; rowNr < minimum((int) all.size(), q.limit); rowNr++) {
            Node* n = all[rowNr];
            Node* dummy=getNew(n->name);
            dummy->kind=Internal->kind;
            rows.push_back(dummy);
            for (int columnNr = 0; columnNr < q.fields.size(); columnNr++) {
                Node *f = q.fields[columnNr];
                if(f==Any)continue;
                Node *v = q.values[n][columnNr];
                //			if (!checkNode(v))addStatement(dummy,f,Null);else
                addStatement(dummy,f,v);
            }
        }
    return rows;
}

NodeVector query(Query& q) {
	NodeVector all = allInstances(q.keyword);
	for (int i = 0; i < q.keywords.size(); i++)
		if (q.keywords[i] != q.keyword)
			mergeVectors(&all, allInstances(q.keywords[i]));
	q.instances = all;
	for (int i = 0; i < q.filters.size(); i++) {
		pf("candidates so far %lu",all.size());
		Statement* _filter = q.filters[i];
		clearAlgorithmHash();
		q.instances = filter(q, _filter);
	}
	pf("candidates: %lu\n",all.size());
    pf("hits: %lu\n",q.instances.size());
	
    collectFieldsAndFacets(q);
    
    if(q.queryType==sqlQuery ){return sqlTable(q);}
    
    return q.instances; // renderResults(q);
}

NodeVector nodeVector(vector<char*> v) {
	NodeVector nv;
	for (int i = 0; i < v.size(); i++) {
		nv.push_back(getThe(v[i]));
	}
	return nv;
}
NodeVector nodeVector(vector<string> v) {
	NodeVector nv;
	for (int i = 0; i < v.size(); i++) {
		nv.push_back(getThe(v[i]));
	}
	return nv;
}

// sentenceToStatement

Statement* parseSentence(string sentence, bool learn = false) {
    
    //	int recurse = 0;
    //	int limit = 5;
	sentence = replace_all(sentence, " a ", " ");
	sentence = replace_all(sentence, " the ", " ");
    char** matches=(char**)malloc(100);
    char* data=editable(sentence.data());
    int count=splitStringC(data,matches, ' ');//matches.size();
    //	vector<char*> matches = splitString(sentence, " ");
	if (count != 3) {
		ps("Currently only triples can be learned. If you have multiple_word nodes combine them with an underscore");
		return 0;
	}
	Node* subject = 0;
	Node* predicate = 0;
	Node* object = 0;
	for (int i = 0; i < count; i++) {
        //		string word = matches[i];
        char* word= matches[i];
		//		word=stem(word);
        //		bool getPredicate = subject&&!predicate;
		int id = atoi(word);
		Node* node;
		if (id > 0)
			node = get(id);
		else{
            if (getRelation(word)) // not here! doch
                node= getRelation(word);
            else
                node = getAbstract(word);
        }
		//		Node* node = getThe(word); TODO getThe for relation!!!
		//		Node* abstract=getAbstract(f);// ,getPredicate?Verb:0
		//		NodeVector& instances=all_instances(abstract, recurse,limit);
		//		if(instances->size()<=0)word=abstract;
		//		else word= instances[0];
		if (!subject)subject = node;
		else if (!predicate)predicate = node; //stem(word);
		else if (!object)object = node; // John smells
	}
	if (!object)object = Any;
	if (!(subject && predicate))
		return 0;
	if (learn)
		return addStatement(subject, predicate, object, true);
	else
		return pattern(subject, predicate, object);
}



Statement *parseFilter(string s) {
    //    if (!contains(s, " "))return addStatement(Any,Any,getAbstract(s.data()));
	if (!contains(s, ".")&&!contains(s, "=")&&!contains(s, "<")&&!contains(s, ">")){
        if (!contains(s, " ")){
            Statement* anyField=addStatement(Any,Equals,getAbstract(s.data()));
            Statement* anyProperty =addStatement(Any,getAbstract(s.data()),Any);
            return addStatement(reify(anyField), Or,reify(anyProperty));
        }
        else
            return parseSentence(s);
    }
    
	Node* subject = Any;
	Node* predicate = Any;
	Node* object;// Any;
	// a.b=>(a,Member,b);
	// a.b=c => (a,b,c);
	if (s.find(".") != s.npos) {
        if(atoi(s.substr(s.find(".")+1).data())==0){// no 3.14
		subject = getThe(s.substr(0, s.find(".")));
		s = s.substr(s.find(".") + 1);
        }
	}
    
	if (contains(s, " is ")) {
		Node* p = getThe(s.substr(0, s.find(" is ")));
		if (subject != Any)predicate = p;
		else {
			subject = p;
			predicate = Equals;
		}
		s = s.substr(s.find(" is ") + 4);
	}
	if (contains(s, "=")) {
		string ding = s.substr(0, s.find("="));
		Node* p = getThe(ding);
		if (subject != Any)
			predicate = p; // a.b=c
		else {
			subject = p; // b=c
			predicate = Equals;
		}
        s = s.substr(s.find("=") + 1);
	}
	if (contains(s, ">")) {
		Node* p = getThe(s.substr(0, s.find(">")));
		if (subject != Any)
			predicate = p;
		else {
			subject = p;
			predicate = Greater;
		}
		s = s.substr(s.find(">") + 1);
	}
	if (contains(s, "<")) {
		Node* p = getThe(s.substr(0, s.find("<")));
		if (subject != Any)predicate = p;
		else {
			subject = p;
			predicate = Less;
		}
		s = s.substr(s.find("<") + 1);
	}
	object = getThe(fixQuotesAndTrim(editable(s.data())));
	return pattern(subject, predicate, object);
}

Query parseQuery(string s, int limit) {
    autoIds=false;
	Query q;
    // static  ???
	char* fields;
	char* type;
	char* match;
	int li = (int)s.find("limit");
	if (li > 0) {
		limit = atoi(s.substr(li + 5).c_str());
		s = s.substr(0, li);
	}
	q.limit = limit;
	p(s);
	s=fixQuery(s);
	if ((int) s.find("from") < 0)
		s = string("select * from ") + s; // why neccessary???
	if ((int) s.find("where") < 0)
		s = s + " where *"; // why neccessary???
	//sscanf(s.c_str(), "select %s from %s where %s", fields, type,match);// NO SPACES :(
	char* ss=modifyConstChar(s.c_str());
	match=cut_to(ss," where "); 
	type=cut_to(ss," from ");
	fields=cut_to(ss,"select ");
    if(!germanLabels && type[strlen(type)-1]=='s')type[strlen(type)-1]=0;// remove plural todo better
	p(ss);
	p(fields);
	p(type);
	p(match);
	q.keyword = getAbstract(type);
	//    q.keywords=nodeVector(splitString(type, ","));
	//    if(q.keywords.size()>0)
	//    q.keyword=q.keywords[0];
    if(!eq(fields,"*"))
		q.fields = nodeVector(splitString(fields, ","));
	if(eq(match,"*"))return q;
	vector<string> matches = splitString(replace_all(match, " and ", ","), ",");
	for (int i = 0; i < matches.size(); i++) {
		string f = matches[i];
		p(f);
		Statement *s = parseFilter(f);
		p(s);
		if (s)q.filters.push_back(s);
	}
	return q;
}

string query2(string s, int limit) {
	s = fixQuery(s);
	Query q = parseQuery(s, limit);
	NodeVector all = query(q);
	string result = renderResults(q);
	p(result);
	return result;
}

// select *,fields from type where attributes match values

NodeVector evaluate_sql(string s, int limit = 20) {//,bool onlyResults=true){
	clearAlgorithmHash();
	NodeVector found;
	static char fields[10000];
	static char type[10000];
	static char where[10000];
	if (s.find("all ") == 0)
		s = s.replace(0, 4, "");
	if ((int) s.find("from") < 0)
		s = string("select * from ") + s; // why (int) neccessary???
	//	s=stem_singular(s)/
	//    s=replace_all(s,"ies","y");//singular
	ps(s);
	sscanf(s.c_str(), "select %s from %s where %[^\n]s", fields, type, where);
	//	makeSingular(type);
	p(fields);
	p(type);
	p(where);
    
//	int batch = limit * 50; // 200000000;//// dont limit wegen filter!! limit + 100;
	//    while (found.size() < limit && batch < 200000000) {
	//        batch = batch * 100; //=> max 9 runs
	// what if all==1000000 but limit==3?
    
	NodeVector all = allInstances(getAbstract(type));
	//  find_all(type, current_context, true, batch); // dont limit wegen filter!!
	pf("%lu so far\n", all.size());
	if (where[0])
		found = filter(all, where); //fields
    
	//    add instance matches
	for (int i = 0; i < all.size(); i++) {
		if (found.size() >= limit)goto good;
		all.clear(); // hack to reset all_instances
		NodeVector all2 = all_instances((Node*) all[i], true, limit);
		if (where[0])
			mergeVectors(&found, filter(all2, where)); //fields)
	}
    
good:
	if (found.size() > limit)// dont deliver too much
		found.erase(found.begin() + limit, found.end());
	showNodes(found);
	return found;
}

// maria.freund=sven
// maria.freund => sven
Statement* evaluate(string data) {
	Statement* pattern = parseFilter(data); //Hypothesis Aim Objective supposition assumption
	// TODO MARK + CLEAR PATTERNS!!!
	Statement* result = findStatement(pattern->Subject(), pattern->Predicate(), pattern->Object());
	if (result){
        deleteStatement(pattern);
        return result;
    }
	else {
        //		return addStatement(s->Subject(), s->Predicate(), s->Object(), true);
        pattern->context=0;// NO LONGER PaTTERN!
        return pattern;
    }
}

Node* match(string data) {
	//    size_type t=data.find("[");
	const char* match = data.substr(data.find("[") + 1, data.find("]") - 1).c_str();
	const char* word = data.substr(0, data.find("[")).c_str();
    return  findMatch(getAbstract(word), match);
}

NodeVector exclude(NodeVector some, NodeVector less) {// bool keep destination unmodified=TRUE
	for (int i = (int)some.size(); i > 0; --i) {
		if (contains(less, (Node*) some[i]))
			some.erase(some.begin() + i);
	}
	return some;
}

int queryContext = _pattern; // hypothesis

// TODO MARK + CLEAR PATTERNS!!!

Statement* pattern(Node* subject, Node* predicate, Node* object) {
	Statement *s = addStatement(subject, predicate, object, false); //todo mark (+reuse?) !
    //#ifdef useContext
	if (checkStatement(s))
		s->context = _pattern;
    //#endif
	Node* pattern = reify(s); // why here?
	pattern->kind = _pattern;
	addStatement(pattern, is_a, Pattern, false);
	return s; // pattern?
}

NodeVector filter(NodeVector all, cchar* matches) {
	if (!matches || strlen(matches) == 0 || all.size() == 0)return all;
    
	Query& q = *new Query();
	q.instances = all;
	static int calls = 0;
	// !! method static declaration will still increase with: !!!
	calls++;
	string buf; // Have a buffer string
	vector<string> tokens; // Create vector to hold our words
	string matchess = matches;
	stringstream ss(matchess); // Insert the string into a stream
	while (ss >> buf)
		tokens.push_back(buf);
    
	for (int y = (int)all.size() - 1; y >= 0; --y) {
		Node* node = (Node *) all[y];
		if (!checkNode(node, 0, 0, 1))continue;
		if (!checkNode(node))continue;
		p("!++++++++++++++++++++++++++++\nfiltering node:");
		//		if (quiet)printf("%s ?",node->name);
		show(node, false);
        
		p("+++++++++++++++++++++++++++++\n");
		bool good = true;
        
		if (!quiet)
			printf("%lu tokens in %s\n", tokens.size(), matches);
        
		// create match tree
		for (int i = 0; i < tokens.size(); i++) {
			string match = tokens[i];
			if (!quiet)
				printf("checking %s\n", match.c_str()); //Illegal instruction/operand if c_str missing!!!
			if (eq(match.c_str(), "and")) {
				p(">>and<<");
				ps(match);
				continue; // continue alone not breakable
			}
			//			printf("%s","checking "+match);
			if (match.find(node->name) == 0)
				match.replace(0, strlen(node->name), "");
			if (match.find(".") == 0)
				match.replace(0, 1, "");
			int comp = (int)match.find("!=");
			if (comp >= 0) {
				Node *s = getThe(match.substr(0, comp).data());
				Node *o = parseValue(match.substr(comp + 2).data());
				filter(q, pattern(s, Not, o));
				continue;
			}
			comp = (int)match.find("=");
			if (comp >= 0) {
				Node *s = getThe(match.substr(0, comp).data());
				Node *o = parseValue(match.substr(comp + 1).data());
				filter(q, pattern(s, Equals, o));
				continue;
			}
			comp = (int)match.find("~");
			if (comp >= 0) {
				Node *s = getThe(match.substr(0, comp).data());
				Node *o = parseValue(match.substr(comp + 1).data());
				filter(q, pattern(s, Circa, o));// todo: approximately
				continue;
			}
			comp = (int)match.find(">");
			if (comp >= 0) {
				Node *s = getThe(match.substr(0, comp).data());
				Node *o = parseValue(match.substr(comp + 1).data());
				filter(q, pattern(s, Greater, o));
				continue;
			}
			comp = (int)match.find("<");
			if (comp >= 0) {
				Node *s = getThe(match.substr(0, comp).data());
				Node *o = parseValue(match.substr(comp + 1).data());
				filter(q, pattern(s, Less, o));
				continue;
			}
			if (!findMatch(node, match.c_str())) {
				good = false;
				//			    break;
			}
		}
		// matches(node,match)
		if (!good) {
			all.erase(all.begin() + y);
			//            p("filtered!");
		} else
			p("passed!");
	}
	if (!quiet)
		printf("%lu nodes passed\n", all.size());
	return all;
}


// filters: x[b] AND x.y=z OR x^z AND x.y>1 AND NOT x.y.z:[a,b] HOW?

//NodeVector filter(NodeVector all, Statement* filterTree) {
//    Query q;
//    q.instances=all;
//    return filter(all,filterTree);
//}
// must assign q.instances=filter(

NodeVector filter(Query& q, Statement* filterTree, int limit) {
	NodeVector hits;
	NodeVector all = q.instances;
	if (limit < 0)limit = q.lookuplimit;
	if (limit == 0)limit = q.limit;
	if (!checkStatement(filterTree))
		p("filterTree broken!\n");
	N subject = filterTree->Subject();
	N predicate = filterTree->Predicate();
	N object = filterTree->Object();
    
	if (predicate == And) {
		NV a = filter(q, subject, q.lookuplimit);
		q.instances = a;
		NV b = filter(q, object, limit); // a h ++
		return b; //intersect(a,b);
	} else if (predicate == Or) {
		NV a = filter(q, subject, q.limit);
		NV b = filter(q, object, q.limit);
		mergeVectors(&a, b);
		return a;
	} else if (predicate == Not) {// todo test
		NV a = all; //.clone()!
		NV b = filter(q, object, (int)all.size());
		return exclude(a, b);
	}
    
	p(filterTree);
	int size = (int)all.size();
	for (int y = size - 1; y >= 0 && hits.size() <= limit; --y) {
		Node* node = (Node *) all.at(y);
        //        if(q.onlyObjects)
        //		if (node->kind == Abstract->id)continue;
//		show(node);
		// cities where city->population->3999
		// cities where *->population->3999
        Statement* found=0;
        if (predicate == Equals) // a=b => n.a:b
			found=findStatement(node, subject, object, q.recursion, q.semantic, false, q.predicatesemantic,q.matchNames);
        else if (isA4(subject, node, q.recursion, q.semantic) || subject == Any){
            found= findStatement(node, predicate, object, q.recursion, q.semantic, false, q.predicatesemantic,q.matchNames);
        }else {
            //  cities where population->equals->3999
			//  match Berlin.population=3999
            
            // free filters:  // cities where population->equals->3999
            has(node, filterTree, q.recursion, q.semantic, false, q.predicatesemantic);
            found=/*TRUE*/filterTree;// hack
        }
        
        if(!found)
            all.erase(all.begin() + y);
        else {
            hits.push_back(node);
            ps("found");
            show(node, false);
            p(y);
        }
	}
	return hits;
}

NodeVector filter(Query& q, Node* _filter, int limit) {
	if (isStatement(_filter))
		return filter(q, isStatement(_filter));
	NodeVector all = q.instances;
	NodeVector hits;
	for (int y = (int)all.size() - 1; y >= 0 && hits.size() <= limit; --y) {
		Node* node = (Node *) all[y];
		if (!findStatement(node, Any, _filter, q.recursion, q.semantic, false, q.predicatesemantic)) {
			all.erase(all.begin() + y);
		} else {
			hits.push_back(node);
		}
	}
	return hits;
}



//char* all="*";

void enqueueClass(Query& q, queue<Node*>& classQueue, Node * c) {
	if (!checkNode(c))return;
	//    if(!contains(classQueue | q.classes, c)){// cyclesave!
	classQueue.push(c);
	q.classes.push_back(c);
	//    }
}

NodeVector & nodesOfDirectType(int kind) {
	NodeVector* all=new NodeVector;
	for (int i = 0; i < context->nodeCount; i++) {
		Node* n = &context->nodes[i];
		if (checkNode(n, i, false, false) && n->kind == kind)
			all->push_back(n);
	}
	return *all;// Reference to the stack memory associated with local variable!
}

//
//// totally redundant !?!
//NodeVector & all_instances(Query & q) {
//	queue<Node*> classQueue;
//	p(q);
//	enqueueClass(q, classQueue, q.keyword);
//	q.instances.push_back(q.keyword); // really?? joain. joa!
//	for (int i = 0; i < q.keywords.size(); i++)
//		//		if(!classQueue.contains)
//		classQueue.push(q.keywords[i]);
//	Node* c;
//	int j = 0;
//	while (classQueue.size() > 0) {
//		c = classQueue.front();
//		classQueue.pop();
//		if (yetvisited[c])continue;
//		yetvisited[c] = true;
//		if (!checkNode(c))continue;
//		pf("all %d %s *%d\n", c->statementCount, c->name, c->id);
//		Statement* s = 0;
//		while (s = nextStatement(c, s)) {
//			//		for (int i = 0; i < c->statementCount; i++) {
//			//			Statement* s = getStatementNr(c, i);
//			//			if (!checkStatement(s))continue;
//			if (q.instances.size() >= resultLimit)
//				return q.instances;
//			if (j++ >= q.lookuplimit) {
//				//				p("lookuplimit reached");
//				break;
//			}
//			//			showStatement(s);
//			if (s->Subject == q.keyword) {
//				if (contains(q.instances, s->Object))continue;
//				if (isA4(s->Predicate, Instance, false, false)) {
//					q.instances.push_back(s->Object);
//					//					if(q.depth>0)// q.depth-- >0 ==7  ? todo !
//					enqueueClass(q, classQueue, s->Object);
//					continue; // found one!
//				}
//				//            if (isA4(s->Predicate, Instance, false, false))if (!contains(q.instances, s->Object))q.instances.push_back(s->Object);
//				if (isA4(s->Predicate, SubClass, false, false))enqueueClass(q, classQueue, s->Object);
//				if (isA4(s->Predicate, Plural, false, false))enqueueClass(q, classQueue, s->Object);
//				if (isA4(s->Predicate, Synonym, false, false))enqueueClass(q, classQueue, s->Object);
//			} else {
//				if (contains(q.instances, s->Subject))continue;
//				if (isA4(s->Predicate, Type, false, false)) {
//					q.instances.push_back(s->Subject);
//					pf("%d %s\n", s->Subject->id, s->Subject->name);
//					//					if(q.depth>0)
//					enqueueClass(q, classQueue, s->Subject);
//					continue; // found one!
//				}
//				if (isA4(s->Predicate, SuperClass, false, false))enqueueClass(q, classQueue, s->Subject);
//				if (isA4(s->Predicate, Plural, false, false))enqueueClass(q, classQueue, s->Subject);
//				if (isA4(s->Predicate, Synonym, false, false))enqueueClass(q, classQueue, s->Subject);
//			}
//		}
//	}
//	NodeVector allOfType0 = nodesOfDirectType(q.keyword->id);
//	addRange(q.instances, allOfType0, false);
//	//    q.instances = addRange(q.instances, q.classes); //nee
//	return q.instances;
//}


// todo?: EXCLUDING classes and direct instances on demand!
// WDYM direct instances ???
NodeSet* all_instances3(Node* type, int recurse, int max, bool includeClasses) {
	static NodeSet* all=new NodeSet;
	if (type == 0) {
		all->clear(); // hack!!!
		return 0;
	}
	if (recurse > 0)recurse = recurse + 2; // recurse++; dont descend too deep! todo: as iterator anyways!
	if (yetvisited[type])
		return all;
	yetvisited[type] = true;
	if (recurse > maxRecursions)return all;
	runs++;
	if (runs > maxNodes)return all; // no infinite loops!
	NodeVector subtypes;
	bool is_abstract=isAbstract(type);
	// todo : via instanceFilter see below
	Statement* s = 0;
	while ((s = nextStatement(type, s)) && all->size() <= max && subtypes.size() <= max) {
		//	for (int i = 0; i < type->statementCount; i++) {
		//		Statement* s = getStatementNr(type, i);
		if (!checkStatement(s))continue;
		if (s->Predicate() == type)continue; // NO Predicate matches!!
		//		if (s->subject == 613424) {
		//			showStatement(s);
		//			show((Node*) (all.end() - 1).base());
		//		}
		//    	po
		p(s);
		if (s->Subject() == type) {// todo contains SLOW!!!
			if (is_abstract && isA4(s->Predicate(), Instance, false, false))if (!contains(subtypes, s->Object()))subtypes.push_back(s->Object());
			if (!is_abstract && isA4(s->Predicate(), Instance, false, false))if (!contains(all, s->Object()))all->insert(s->Object());
			if (isA4(s->Predicate(), SubClass, false, false))if (!contains(subtypes, s->Object()))subtypes.push_back(s->Object());
			if (isA4(s->Predicate(), Plural, false, false))if (!contains(subtypes, s->Object()))subtypes.push_back(s->Object());
			if (isA4(s->Predicate(), Synonym, false, false))if (!contains(subtypes, s->Object()))subtypes.push_back(s->Object());
			if (isA4(s->Predicate(), Translation, false, false))if (!contains(subtypes, s->Object()))subtypes.push_back(s->Object());
		} else {
			if (isA4(s->Predicate(), Type, false, false))all->insert(s->Subject());
			if (isA4(s->Predicate(), SuperClass, false, false))subtypes.push_back(s->Subject());
			if (isA4(s->Predicate(), Plural, false, false))if (!contains(subtypes, s->Subject()))subtypes.push_back(s->Subject());
			if (isA4(s->Predicate(), Synonym, false, false))if (!contains(subtypes, s->Subject()))subtypes.push_back(s->Subject());
			if (isA4(s->Predicate(), Translation, false, false))if (!contains(subtypes, s->Subject()))subtypes.push_back(s->Subject());
		}
	}
	p(all->size());
	//	subtypes.push_back(type); NOT AGAIN
	if (recurse)
		for (int i = 0; i < subtypes.size(); i++) {// subtypes
			if (all->size() >= max)
				return all;
			Node* x = (Node*) subtypes[i];
			pf("all %d %s *%d\n", x->statementCount, x->name, x->id);
			if (!checkNode(x))continue; // how??
			//			if (recurse)
			//				more = all_instances(x, recurse, max - all.size());// why sooo slooow ???
			//			else
			lookupLimit=max;
			NodeVector more = instanceFilter(x);//,null,max); //   all_instances(x, recurse-1, max);
			mergeVectors(all, more);
		}
	subtypes.push_back(type);
	if (includeClasses)
		mergeVectors(all, subtypes); // ja?
	p(all->size());
	return all;
}

NodeVector & all_instances(Node* type, int recurse, int max, bool includeClasses) {
//	RECURE BROKEN! use instanceFilter
	NodeSet* a=all_instances3(type, recurse, max, includeClasses) ;
	if(!a)return EMPTY;
	//	NodeVector v(a->begin(),a->end());
//	static NodeVector v(a->begin(),a->end());
//	static NodeVector v(a->size());
	NodeVector* v=new NodeVector(a->size());// LEAK!
	std::copy(a->begin(), a->end(), v->begin());
//	p(v.size());
	return *v;
}
NodeVector & all_instances2(Node* type, int recurse, int max, bool includeClasses) {
	static NodeVector& all = *new NodeVector; // empty before!
//	static NodeSet& alles=*new NodeSet;
	NodeVector v;
	if (type == 0) {
		all.clear(); // hack!!!
		return EMPTY;
	}
	if (recurse > 0)recurse = recurse + 2; // recurse++; dont descend too deep! todo: as iterator anyways!
	if (yetvisited[type])
		return all;
	yetvisited[type] = true;
	if (recurse > maxRecursions)return all;
	runs++;
	if (runs > maxNodes)return all; // no infinite loops!
	NodeVector subtypes;
    bool is_abstract=isAbstract(type);
	// todo : via instanceFilter see below
    Statement* s = 0;
	while ((s = nextStatement(type, s)) && all.size() <= max && subtypes.size() <= max) {
        //	for (int i = 0; i < type->statementCount; i++) {
        //		Statement* s = getStatementNr(type, i);
		if (!checkStatement(s))continue;
		if (s->Predicate() == type)continue; // NO Predicate matches!!
		//		if (s->subject == 613424) {
		//			showStatement(s);
		//			show((Node*) (all.end() - 1).base());
		//		}
		//    	po
        p(s);
		if (s->Subject() == type) {// todo contains SLOW!!!
			if (is_abstract && isA4(s->Predicate(), Instance, false, false))if (!contains(subtypes, s->Object()))subtypes.push_back(s->Object());
			if (!is_abstract && isA4(s->Predicate(), Instance, false, false))if (!contains(all, s->Object()))all.push_back(s->Object());
			if (isA4(s->Predicate(), SubClass, false, false))if (!contains(subtypes, s->Object()))subtypes.push_back(s->Object());
			if (isA4(s->Predicate(), Plural, false, false))if (!contains(subtypes, s->Object()))subtypes.push_back(s->Object());
			if (isA4(s->Predicate(), Synonym, false, false))if (!contains(subtypes, s->Object()))subtypes.push_back(s->Object());
			if (isA4(s->Predicate(), Translation, false, false))if (!contains(subtypes, s->Object()))subtypes.push_back(s->Object());
		} else {
			if (isA4(s->Predicate(), Type, false, false))
				all.push_back(s->Subject());
			if (isA4(s->Predicate(), SuperClass, false, false))subtypes.push_back(s->Subject());
			if (isA4(s->Predicate(), Plural, false, false))if (!contains(subtypes, s->Subject()))subtypes.push_back(s->Subject());
			if (isA4(s->Predicate(), Synonym, false, false))if (!contains(subtypes, s->Subject()))subtypes.push_back(s->Subject());
			if (isA4(s->Predicate(), Translation, false, false))if (!contains(subtypes, s->Subject()))subtypes.push_back(s->Subject());
		}
        
        
	}
	p(all.size());
	//	subtypes.push_back(type); NOT AGAIN
	if (recurse)
		for (int i = 0; i < subtypes.size(); i++) {// subtypes
			if (all.size() >= max)
				return all;
			Node* x = (Node*) subtypes[i];
			pf("all %d %s *%d\n", x->statementCount, x->name, x->id);
			if (!checkNode(x))continue; // how??
			NodeVector more;
			//			if (recurse)
			//				more = all_instances(x, recurse, max - all.size());// why sooo slooow ???
			//			else
			lookupLimit=max;
			more = instanceFilter(x);//,null,max); //   all_instances(x, recurse-1, max);
			mergeVectors(&all, more);
		}
	subtypes.push_back(type);
	if (includeClasses)
		mergeVectors(&all, subtypes); // ja?
	p(all.size());
	return all;
}

NodeVector & recurseFilter(Node* type, int recurse, int max, NodeVector(*edgeFilter)(Node*, NodeQueue*)) {
	static NodeVector& all = *new NodeVector; // empty before!
	if (type == 0) {
		all.clear(); // hack!!!
		return EMPTY;
	}
	if (recurse > 0)recurse = recurse + 2; // recurse++; dont descend too deep! todo: as iterator anyways!
	if (yetvisited[type])
		return all;
	yetvisited[type] = true;
	if (recurse > maxRecursions)return all;
	runs++;
	if (runs > maxNodes)return all; // no infinite loops!
    
	NodeVector more = edgeFilter(type, null);
	mergeVectors(&all, more);
    
	for (int i = 0; i < more.size(); i++) {// subtypes
		if (all.size() >= max)return all;
		Node* x = (Node*) more[i];
		if (!checkNode(x))continue; // how??
		pf("all %d %s *%d\n", x->statementCount, x->name, x->id);
		recurseFilter(x, recurse, max, edgeFilter); // recurse++ above OK, adds to 'all'
		//			mergeVectors(all,recurseFilter(x,recurse, max,edgeFilter));// recurse++ above
	}
	all.push_back(type);
	return all;
}

Query & getQuery(Node * keyword) {
	Query& q = *new Query();
	q.keywords.clear(); // =new NodeVector();
	q.keyword = keyword;
	q.classes.clear();
	//    q.recursion = recurse;
	q.depth = maxRecursions; //ja?
	//    q.limit = max;
	q.runs = 0;
	return q;
}

void clearAlgorithmHash(bool all) {
	runs = 0;
	yetvisited.erase(yetvisited.begin(), yetvisited.end()); //erase yetvisited
	yetvisited.clear();
	//			only if low mem!!~:::
	yetvisitedIsA.erase(yetvisitedIsA.begin(), yetvisitedIsA.end()); //erase yetvisited
	yetvisitedIsA.clear();
	all_instances(0, 0); //hack! //			all.clear();
	recurseFilter(0, 0, 0, 0);
}

NodeVector find_all(cchar* name, int context, int recurse, int limit) {
	clearAlgorithmHash();
	// if(context> -1)search subcontexts also?
	// if(context==-1)search all
	// if(context==-2)context=current_context;
    //	Context* c = getContext(context);
	NodeVector all;
	if (recurse > 0)recurse++;
	if (recurse > maxRecursions)return all;
    
    //	int max = minimum((long) c->nodeCount, maxNodes);
	all.push_back(getAbstract(name));
    
	//    for (int i = 0; i < max; i++) {// inefficient^2 use word->instance->... instead
	//        Node* n = &c->nodes[i];
	//        if (n == null || n->name == null || name == null)
	//            continue;
	//        if (eq(n->name, name)) {
	//            all.push_back(n);
	//            if (all.size() > limit)return all;
	//            // printf("found node %s in context %d\n",word,context);						//
	//            // show(n);
	//        }
	//    }
	int alle = (int)all.size();
	if (recurse)
		for (int i = 0; i < alle; i++) {
			all.clear(); // hack to reset all_instances
			Node* node = (Node*) all[i];
			NodeVector& neu = all_instances(node, true, limit);
			mergeVectors(&all, neu);
		}
	return all;
}

Node * findMatch(Node* n, const char* match) {//
	char a[10000];
	char b[10000];
	char c[10000];
	b[0] = 0; //otherwise reused!?!
	c[0] = 0;
	bool matching; // leider immer !? --
	string smatch = replace_all(match, "=", " =");
	matching = sscanf(smatch.c_str(), "%s =%s", a, b);
	if (!quiet)
		printf("test findMatch %s %s\n", n->name, match);
	flush();
	if (!matching)
		p("scanf not matching");
	p(a);
	p(b);
	if (b[0] != 0 && !eq(b, "")) {
		p("n[a=b]");
		if (!quiet)
			printf("show(findStatement(%s,%s,%s))", n->name, a, b);
		Statement* statement = findStatement(n, getAbstract(a), getAbstract(b));
		if (statement) {
			showStatement(statement);
			return n; //statement->Subject;
		} else
			return null;
	} else {
		p("n[match]");
		if (!quiet)
			printf("show(findMember(%s,%s))", n->name, a);
		Node* member = findMember(n, a);
		//		show(member);
		if (member)return n; //findMember(n,a);
		else return 0;
	}
	return 0;
}

// expensive
int countInstances(Node * node) {
	int j = (int)instanceFilter(node).size();
	int i = (int)allInstances(node).size();
	setValue(node, the(direct instance count), value(0, j));
	setValue(node, the(total instance count), value(0, i));
	show(node, false);
	ps("statement count");
	p(node->statementCount);
	ps("direct instance count");
	p(i);
	ps("total instance count");
	p(j);
	return i;
}

// put as callback into findPath for recursion
// NodeVector findPath(Node* fro, Node* to, NodeVector(*edgeFilter)(Node*, NodeQueue*))
// edgeFilter:
//childFilter == instanceFilter (+SubClass or not?)
// todo:  deduplicate code: childFilter=filter<Instance,SubClass>
bool INCLUDE_LABELS=false;
bool INCLUDE_CLASSES=false;
NodeVector instanceFilter(Node* subject, NodeQueue * queue){// chage all + edgeFilter!! for , int max) {
	NodeVector all;	int i = 0;
	Statement* s = 0;
	while (i++<lookupLimit * 2 && (s = nextStatement(subject, s, false))) {// true !!!!
		bool subjectMatch = (s->Subject() == subject || subject == Any);
		bool predicateMatch = (s->Predicate() == Instance)|| (INCLUDE_CLASSES && s->Predicate() == SubClass);
		predicateMatch = predicateMatch || s->predicate==-10301;// Hauptartikel in der Kategorie
		predicateMatch = predicateMatch || s->predicate==-10373;//  	Commons-Kategorie

		bool subjectMatchReverse = s->Object() == subject;
		bool predicateMatchReverse = s->Predicate() == Type;
		predicateMatchReverse = predicateMatchReverse || (INCLUDE_CLASSES && s->Predicate() == SuperClass);
		predicateMatchReverse = predicateMatchReverse || (INCLUDE_LABELS  && s->Predicate() == Label) ; // || inverse
		predicateMatchReverse = predicateMatchReverse || s->predicate==-10373;
		predicateMatchReverse = predicateMatchReverse || (s->Predicate() == Label && contains(s->Subject()->name, subject->name));// Frankfurt (Oder)

		if (queue) {
			if (subjectMatch&& predicateMatch)enqueue(subject, s->Object(), queue);
			if (subjectMatchReverse&& predicateMatchReverse)enqueue(subject, s->Subject(), queue);
		} else {
			if (subjectMatch && predicateMatch)all.push_back(s->Object());
			if (subjectMatchReverse && predicateMatchReverse)all.push_back(s->Subject());
		}
	}
	return all;
}


// put as callback into findPath for recursion
NodeVector subclassFilter(Node* subject, NodeQueue * queue){
	NodeVector all;	int i = 0;
	Statement* s = 0;
	while (i++<lookupLimit * 2 && (s = nextStatement(subject, s, false))) {// true !!!!
		bool subjectMatch = (s->Subject() == subject || subject == Any)  && !eq(s->Object()->name,"◊");;
		bool predicateMatch = (s->Predicate() == SubClass);

		bool subjectMatchReverse = s->Object() == subject && !eq(s->Subject()->name,"◊");
		bool predicateMatchReverse = s->Predicate() == SuperClass; // || inverse
		if(s->subject==23||s->object==23)
			printf("");
		if (queue) {
			if (subjectMatch&&predicateMatch)
				enqueue(subject, s->Object(), queue);
			if (subjectMatchReverse&& predicateMatchReverse)
				enqueue(subject, s->Subject(), queue);
		} else {
			if (subjectMatch && predicateMatch)all.push_back(s->Object());
			if (subjectMatchReverse && predicateMatchReverse)all.push_back(s->Subject());
		}
	}
	return all;
}

// put as callback into findPath for recursion
NodeVector relationsFilter(Node* subject, NodeQueue * queue){//, int max) {
	NodeVector all;
//    vector<int> relations;
	int i = 0;
	Statement* s = 0;
	while (i++<lookupLimit * 2 && (s = nextStatement(subject, s, false))) {
        if(!contains(all,s->Predicate()))
           all.push_back(s->Predicate());
	}
	return all;
}


// how to find paths with property predicates??
// put as callback into findPath for recursion
NodeVector memberFilter(Node* subject, NodeQueue * queue) {
	NodeVector all;
	int i = 0;
	Statement* s = 0;
	while (i++ < 1000 && (s = nextStatement(subject, s, false))) {// true !!!!
		//		if (s->Object->id < 100)continue; // adverb,noun,etc bug !!
		if (subject->id == 213112)
			//		if(s->id==467484)
			p(s);
		if (s->Predicate() == subject)break;
		if (s->Object() == Adjective)continue;
		if (s->Object() == Adverb)continue;
		if (s->Object() == Noun)continue;
		if (s->Predicate() == PERTAINYM)continue;
		if (s->Predicate() == Derived)continue;
		//		if (s->Predicate==DerivedFromNoun)continue;
		if (s->Predicate() == get(_attribute))continue;
		if (s->Predicate() == get(40))continue; // similar
		if (s->Predicate() == get(50))continue; // also
		if (s->Predicate() == get(91))continue; // also bug !!
		if (s->Predicate() == get(92))continue; // also bug !!
		bool subjectMatch = (s->Subject() == subject || subject == Any);
		bool predicateMatch = (s->Predicate() == Member);
		predicateMatch = predicateMatch || s->Predicate() == Part;
		predicateMatch = predicateMatch || s->Predicate() == Attribute;
		predicateMatch = predicateMatch || s->Predicate() == Substance;
		predicateMatch = predicateMatch || s->Predicate() == Active;
		predicateMatch = predicateMatch || s->Predicate() == To;
		predicateMatch = predicateMatch || s->Predicate() == For;
		// include Parents!
		predicateMatch = predicateMatch || s->Predicate() == Type;
		predicateMatch = predicateMatch || s->Predicate() == SuperClass;
		predicateMatch = predicateMatch || s->Predicate() == Synonym;
		predicateMatch = predicateMatch || s->Predicate() == Plural;
		predicateMatch = predicateMatch || s->Predicate()->id == _MEMBER_DOMAIN_CATEGORY;
		predicateMatch = predicateMatch || s->Predicate()->id == _MEMBER_DOMAIN_REGION;
		predicateMatch = predicateMatch || s->Predicate()->id == _MEMBER_DOMAIN_USAGE;
		//		predicateMatch = predicateMatch || isA4(s->Predicate,)
        
		bool subjectMatchReverse = s->Object() == subject;
		bool predicateMatchReverse = s->Predicate() == Owner; // || inverse
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == By;
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == From;
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == PartOf;
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == UsageContext;
		predicateMatchReverse = predicateMatchReverse || s->Predicate()->id == _DOMAIN_CATEGORY;
		predicateMatchReverse = predicateMatchReverse || s->Predicate()->id == _DOMAIN_REGION;
		predicateMatchReverse = predicateMatchReverse || s->Predicate()->id == _DOMAIN_USAGE;
		// MORE :
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == Plural;
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == Synonym;
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == Translation;
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == SubClass;
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == Instance;
		//		predicateMatchReverse = predicateMatchReverse || isA4(s->Predicate)
		if (queue) {
			if (subjectMatch && predicateMatch)enqueue(subject, s->Object(), queue);
			if (subjectMatchReverse && predicateMatchReverse)enqueue(subject, s->Subject(), queue);
		} else {
			if (subjectMatch && predicateMatch)all.push_back(s->Object());
			if (subjectMatchReverse && predicateMatchReverse)all.push_back(s->Subject());
			//			if (subjectMatch && s->Predicate->id>1000)all.push_back(s->Object);// properties how to?
		}
	}
	if (queue)// already enqueued
		return EMPTY; //hack
	else
		return all;
}


NodeVector parentFilter2(Node* subject, NodeQueue * queue, bool backInstances) {
	NodeVector all;
	int i = 0;
	Statement* s = 0;
	int type_lookup_limit=10;// type statements should be at the very beginning
	while (i++ < type_lookup_limit && (s = nextStatement(subject, s, false))) {// true !!!!
        //#ifdef useContext
		if (s->context == _pattern)continue;// important!!//        else it always matches!!!
        //#endif
		if (s->Object() == Adjective)continue; // bug !!
		if (s->Predicate() == PERTAINYM)continue;
		if (s->Predicate() == Derived)continue;
		//		if (s->Predicate==DerivedFromNoun)continue;
		if (s->Predicate() == get(_attribute))continue;
		if(filterWikiType(s->object))continue;

		//		if(s->Predicate==Instance && !eq(s->Object->name,subject->name) )break;// needs ORDER! IS THE FIRST!!
		//		if(s->Predicate==Type&&s->Object==subject)break;// todo PUT TO END TOO!!!
		bool subjectMatch = (s->Subject() == subject || subject == Any);
		bool predicateMatch = (s->Predicate() == Type);
		predicateMatch = predicateMatch || s->Predicate() == SuperClass;
		predicateMatch = predicateMatch || s->Predicate() == Synonym;
		predicateMatch = predicateMatch || s->Predicate() == Translation;
		predicateMatch = predicateMatch || s->Predicate() == Plural;
        
		bool subjectMatchReverse = s->Object() == subject;
		bool predicateMatchReverse = s->Predicate() == Instance && backInstances; // || inverse
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == Plural;
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == Synonym;
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == Translation;
		predicateMatchReverse = predicateMatchReverse || s->Predicate() == SubClass;
        
		if (queue) {
			if (subjectMatch && predicateMatch)enqueue(subject, s->Object(), queue);
			if (subjectMatchReverse && predicateMatchReverse)enqueue(subject, s->Subject(), queue);
		} else {
			if (subjectMatch && predicateMatch)all.push_back(s->Object());
			if (subjectMatchReverse && predicateMatchReverse)all.push_back(s->Subject());
		}
	}
	if (queue)// already enqueued
		return EMPTY; //hack
	else
		return all;
}


// put as callback into findPath for recursion.
// Currently same concept as topicFilter, with filterWikiType in findPath
NodeVector parentFilter(Node* subject, NodeQueue * queue) {
	return parentFilter2(subject,queue,true);
}

NodeVector topicFilter(Node* subject, NodeQueue * queue) {
	return parentFilter2(subject,queue,false);
}

// todo : memory LEAK NodeVector ?
// todo : enqueue instances?
// put as callback into findPath for recursion
NodeVector anyFilter(Node* subject, NodeQueue * queue, bool includeRelations) {
	if (!includeRelations && subject->id < 1000)return EMPTY;
	NodeVector all;
	int i = 0;
	Statement* s = 0;
	while (i++ < 10000 && (s = nextStatement(subject, s, false))) {
		if (!checkStatement(s)) {
			badCount++;
			continue;
		}
		bool subjectMatch = (s->Subject() == subject || subject == Any);
		bool subjectMatchReverse = s->Object() == subject;
		if (queue) {
			if (subjectMatch)enqueue(subject, s->Object(), queue);
			if (subjectMatchReverse)enqueue(subject, s->Subject(), queue);
		} else {
			if (subjectMatch)all.push_back(s->Object());
			if (subjectMatchReverse)all.push_back(s->Subject());
		}
	}
	if (queue)// already enqueued
		return EMPTY; //hack
	else
		return all;
}

NodeVector anyFilterNoKinds(Node* subject, NodeQueue * queue) {
	return anyFilter(subject, queue, false);
}

NodeVector anyFilterRandom(Node* subject, NodeQueue * queue) {
	return anyFilter(subject, queue, true);
}

NodeVector reconstructPath(Node* from, Node * to) {
	Node* current = to;
	NodeVector all;
//	bool ok = true;
	//	p("++++++++ FOUND PATH ++++++++++++++");
	while (current && current != from) {
		all.push_back(current);
		int id = enqueued[current->id];
		if (id <= 0) {
//			ok = false;
			break;
		}
		current = get(id);
		//		show(current,false);
		if (contains(all, current))break; //LOOOOOP
	}
	all.push_back(from); // done
	std::reverse(all.begin(), all.end());
	return all;
}

bool enqueue(Node* current, Node* d, NodeQueue * q) {
	if (!d || enqueued[d->id+propertySlots])return false; // already done -> continue;
	//	printf("? %d %s\n",d->id, d->name);
	// todo if d==to stop here!
	enqueued[d->id+propertySlots] = current->id;
	q->push(d);
	runs++;
	return true;
}
#define min(x,y) x<y?x:y

Node* getFurthest(Node* fro, NodeVector(*edgeFilter)(Node*, NodeQueue*)) {
	enqueued = (int*) malloc(maxNodes * sizeof (int)); //context->nodeCount * 2
	int* depths	 = (int*) malloc(maxNodes * sizeof (int)); //context->nodeCount * 2
	if (enqueued == 0) throw "out of memory for findPath";
	//	memset(enqueued, 0, min(context->nodeCount,maxNodes) * sizeof (bool)); // NOT neccessary?
	NodeQueue q;
	q.push(fro);
	depths[fro->id+propertySlots]=0;
	N furthest=fro;
	int deepest=0;
	// NOT neccessary for anyPath , ...
	NodeVector instances;
	if (isAbstract(fro) && edgeFilter != anyFilterNoKinds && edgeFilter != instanceFilter && edgeFilter != anyFilterRandom) // && edgeFilter!=
		instances = allInstances(fro);// only in first step! (i.e. sublcasses of abstract)
	for (int i = 0; i < instances.size(); i++) {
		Node* d = instances[i];
		enqueued[d->id+propertySlots] = fro->id;
		depths[d->id+propertySlots]=1;
		q.push(d);
		pf("instance %d %s\n", d->id, d->name);
	}
	Node* current;
	NodeSet all;
	while ((current = q.front())) {
		//		if(enqueued[current->id+propertySlots])continue;
		//		enqueued[current->id+propertySlots]=true;// +propertySlots DANGER HERE!!!
		all.insert(current);
		if(all.size()>resultLimit)break;
		if (q.empty())break;
		q.pop();
		if (!checkNode(current, 0, true /*checkStatements*/, true /*checkNames*/,true /*report*/)||(current->name[0]<'A'))
			continue;
		if(startsWith(current->name,"http"))
			continue;
		N pa=get(enqueued[current->id+propertySlots]);
		int depth=depths[current->id+propertySlots]+1;
		if(depth>deepest)furthest=current;
		if(!pa)pa=Unknown;// Error;// Nil;
		//		printf("%d	%s	≈ %d	%s\r\n",current->id,current->name,pa->id,pa->name);
		printf("%s	Q%d	=> %s	Q%d\r\n",current->name,current->id,pa->name,pa->id);
		if(q.size()<lookupLimit)// bad (?) : refill after pop()
			NodeVector more = edgeFilter(current, &q);// enqued => empty!
		//		show(more);
		//		mergeVectors(&all, more);
	}
	free(enqueued);
	free(depths);
	return furthest;
}


// i.e. findAll(a(Person),subclassFilter)
NodeSet findAll(Node* fro, NodeVector(*edgeFilter)(Node*, NodeQueue*)) {
//	bool* enqueued = (bool*) malloc(maxNodes * sizeof (bool)); //context->nodeCount * 2
	enqueued = (int*) malloc(maxNodes * sizeof (int)); //context->nodeCount * 2
	if (enqueued == 0) throw "out of memory for findPath";
//	memset(enqueued, 0, min(context->nodeCount,maxNodes) * sizeof (bool)); // NOT neccessary?
	NodeQueue q;
	q.push(fro);
	// NOT neccessary for anyPath , ...
	NodeVector instances;
	if (isAbstract(fro) && edgeFilter != anyFilterNoKinds && edgeFilter != instanceFilter && edgeFilter != anyFilterRandom) // && edgeFilter!=
		instances = allInstances(fro);// only in first step! (i.e. sublcasses of abstract)
	for (int i = 0; i < instances.size(); i++) {
		Node* d = instances[i];
		enqueued[d->id+propertySlots] = fro->id;
		q.push(d);
		pf("instance %d %s\n", d->id, d->name);
	}
	Node* current;
	NodeSet all;
	while ((current = q.front())) {
//		if(enqueued[current->id+propertySlots])continue;
//		enqueued[current->id+propertySlots]=true;// +propertySlots DANGER HERE!!!
		all.insert(current);
		if(all.size()>resultLimit)break;
		if (q.empty())break;
		q.pop();
		if (!checkNode(current, 0, true /*checkStatements*/, true /*checkNames*/,true /*report*/)||(current->name[0]<'A'))
			continue;
		if(startsWith(current->name,"http"))
			continue;
		N pa=get(enqueued[current->id+propertySlots]);
		if(!pa)pa=Unknown;// Error;// Nil;
//		printf("%d	%s	≈ %d	%s\r\n",current->id,current->name,pa->id,pa->name);
		printf("%s	Q%d	=> %s	Q%d\r\n",current->name,current->id,pa->name,pa->id);
		if(q.size()<lookupLimit)// bad (?) : refill after pop()
			NodeVector more = edgeFilter(current, &q);// enqued => empty!
//		show(more);
//		mergeVectors(&all, more);
	}
	free(enqueued);
	return all;
}

//NodeVector NodeVectorFrom(NodeSet& input){
NodeVector setToVector(NodeSet& input){
	NodeVector neu;
	std::copy(input.begin(), input.end(), std::back_inserter(neu));
	return neu;
}
NodeVector nodeSetToNodeVector(NodeSet& input){
	NodeVector neu;
	std::copy(input.begin(), input.end(), std::back_inserter(neu));
	return neu;
}


// ok to return NodeVector, not &NodeVector !! see S.O.
NodeVector findAllSubclasses(Node *fro){
	NodeSet all=findAll(fro, subclassFilter);
	return setToVector(all);
}

#define DROP true
#define KEEP false
bool filterWikiType(int object){
	if(object==4167410)return DROP; // Wikimedia-Begriffsklärungsseite
	if(object==4167836)return DROP; // Wikimedia-Kategorie
	if(object==6671777)return DROP;//Struktur	Q6671777
	if(object==5962346)return DROP;//, "topic":"Klassifikation",
	if(object==58778)return DROP;//=> System	Q58778
	if(object==80071)return DROP;// Symbol
	if(object==19361238)return DROP;//=x Metaklasse
	if(object==35120)return DROP; // Entität
	if(object==223557)return DROP; // 	"topicid":223557, "topic":"Körper",
	if(object==5127848)return DROP; // Gruppe
	if(object==27948)return DROP; // Liste
	if(object==827335)return DROP; // Abstrakter Datentyp
	if(object==1979154)return DROP; // 	Modell
	if(object==386724)return DROP; //	Werk	Q386724	=> Produkt	Q2424752
	if(object==28877)return DROP; //	Gut	Q28877	=> Produkt	Q2424752
	if(object==7184903)return DROP; //	Abstraktes Objekt
	if(object==853614)return DROP; //	 Identifikator
	if(object==2221906)return DROP; //		Standort
	if(object==9158768)return DROP; //		Speicher
	return KEEP;
}

// ONE path! See findAll for all leaves
NodeVector findPath(Node* fro, Node* to, NodeVector(*edgeFilter)(Node*, NodeQueue*)) {
	enqueued = (int*) malloc(maxNodes * sizeof (int)); //context->nodeCount * 2
	if (enqueued == 0)throw "out of memory for findPath";
	memset(enqueued, 0, context->nodeCount * sizeof (int));// Necessary?
	ps("LOAD!");
	NodeQueue q;
	q.push(fro);
	runs = 0;
    
	// NOT neccessary for anyPath , ...
	NodeVector instances;
	if (edgeFilter != anyFilterNoKinds && edgeFilter != instanceFilter && edgeFilter != anyFilterRandom) // && edgeFilter!=
		instances = allInstances(fro);
	for (int i = 0; i < instances.size(); i++) {
		Node* d = instances[i];
		enqueued[d->id] = fro->id;
		q.push(d);
		pf("FROM %d %s\n", d->id, d->name);
	}
	pf("TO %d %s\n", to->id, to->name);
	//	p(to);
	p("GO!");
    
	Node* current;
	NodeVector path=EMPTY;
	while ((current = q.front())) {
		if(filterWikiType(current->id))continue;
		if (q.empty())break;
		q.pop();
		//		if(current->id==230608)
		//			pf("?? %d %s\n",current->id,current->name);
		if (to == current){// GOT ONE!
			path=reconstructPath(fro, to); // shortcut
			break;
		}
		if (!checkNode(current, 0, true))
			continue;
		NodeVector all = edgeFilter(current, &q);
		if (all != EMPTY)// no queue
			for (int i = 0; i < all.size(); i++) {
				Node* d = (Node*) all[i];
				if (to == current){// GOT ONE!
					path=reconstructPath(fro, to); // shortcut
					break;
				}
				enqueue(current, d, &q);
			}
	}
	free(enqueued);
//	pf("NO PATH FOUND! Touched nodes: %d\n", runs);
	return path;
}

NodeVector memberPath(Node* from, Node * to) {
	NodeVector all = findPath(from, to, memberFilter);
	showNodes(all, false, true);
	return all;
}

NodeVector parentPath(Node* from, Node * to) {
	NodeVector all = findPath(from, to, parentFilter);
	if (all.size() > 0)p("+++++++++++++++++++++++++++++++++");
	showNodes(all, false, true);
	return all;
}

NodeVector shortestPath(Node* from, Node * to) {
	NodeVector all = findPath(from, to, anyFilterNoKinds);
	if (all.size() == 0)all = findPath(from, to, anyFilterRandom);
	showNodes(all, false, true);
	return all;
}


NodeVector nodeVectorWrap(Node* n) {
	NodeVector r;
	r.push_back(n);
	return r;
}
NodeVector nodeVectorWrap(Statement* n) {
	NodeVector r;
	r.push_back(n->Subject());
	return r;
}

NodeVector update(cchar* query){
    autoIds=true;
	char* data=modifyConstChar(query);
	if(startsWith(data, "update "))data+=7;
	if(startsWith(data, ":update "))data+=8;
    char* i=strstr(data, " set ");
    if(!i)throw "SYNTAX: UPDATE * SET x=y";// NOT OK;
    i[0]=0;
    string expression=i+5;
    NV all;
    if(contains(data,"."))
        all= parseProperties(data);
    else
        all=nodeVectorWrap(getThe(data));
    for (int i=0; i < all.size(); i++) {
        Node* n=(Node*) all[i];
        learn(itoa(n->id)+"."+expression);
    }
    return all;
}



NodeVector parseProperties(char *data) {
	lookupLimit=10000;
	char *thing=0;//=(char *) malloc(1000);
	char *property;//=(char *) malloc(1000);
	thing=strstr(data, " of ");
	if(!thing)thing=strstr(data, " by ");
	if(!thing)thing=strstr(data, " in ");
	if(!thing)thing=strstr(data, " von ");
	if(thing){
		property=data;
		thing[0]=0;
		thing=thing+4;
		//		sscanf(data, "%s of %[^\n]", property, thing);
		//		sscanf(data, "%s of %s", property, thing);
	}
	else if (contains(data, ":")) {
		//			sscanf(data,"%s:%s",property,thing);
		char** splat=splitStringC(data, ':');
		thing=splat[1];// can't free no more
		property=splat[0];
        //		bool inverse=1;
	} else if (contains(data, ".")) {
		//			sscanf(data,"%s.%s",thing,property);
		char** splat=splitStringC(data, '.');
		thing=splat[0];
		property=splat[1];
	}

	// OK: opponent+of+barack_obama
    // todo : birth_place of james -> %[a-zA-Z _] or splitStringC
	// sscanf is EVIL PERIOD!
	
	if (!property) {
		char** splat=splitStringC(data, ' ');// leeeeak!
		thing=splat[0];
		property=splat[2];//  pointer being freed was not allocated
	}
    
	pf("does %s have %s?\n", thing, property);
	NodeVector all=findProperties(thing, property,false);
	if (all.size()==0&& property[strlen(property) - 1] == 's'){
		property[strlen(property) - 1]=0;// http://netbase.pannous.com/html/South%20Park.Seasons -> http://netbase.pannous.com/html/South%20Park.Season
		all=findProperties(thing, property);
	}
	if (all.size()==0)all=findProperties(thing, property,true);// INVERSE!!
//	if(malloc(ed)
//	free(property);
//	free(thing);
	showNodes(all);
	return all;
}
NodeVector parseProperties(const char *data) {return parseProperties(editable(data));}
bool containsSubstring(vector<char*>& words, char* sub) {
	for (int j=0; j < words.size(); j++) {
		char* word=words[j];
		if (contains(word, sub,true/*ignoreCase*/)){
			if(eq(word,sub))
				continue;// only true substrings!
			else
				return true;
		}
	}
	return false;
}

NV filterCandidates(NV all){
	VC words;
	int size=(int)all.size();
	for(int i=size-1;i>=0;i--)
		words.push_back(all[i]->name);

	for(int i=size-1;i>=0;i--){
		N entity=all[i];
		if(containsSubstring(words, entity->name)){
			all.erase(all.begin() +i);
//			all.erase(&all[i]);
//			all[i]=0;
		}
	}
//	all.shrink_to_fit();
	for(int i=0;i<size;i++){
		N entity=all[i];
		if(isAbstract(entity)){
			NV more=allInstances(entity);
			mergeVectors(&all, more);
		}
	}
	return all;
}



//vector<cchar*>
//N
map<int,bool> loadBlacklist(bool reload/*=false*/){
	N blacklist=getAbstract("entity blacklist");
	map<int,bool> forbidden; // int: wordhash
	// todo

	FILE *infile=open_file("blacklist.csv");
	char line[1000];// Relativ geschwind!
	while (fgets(line, sizeof(line), infile) != NULL) {
		forbidden[wordhash(line)]=true;
	}
	if(!reload && blacklist->statementCount>1000)return forbidden;
//	static vector<cchar*> forbidden;// Reloaded in every query ... how to avoid?
//	if(forbidden.size()>0)return forbidden;

	bool check=blacklist->statementCount>1000;

	while (fgets(line, sizeof(line), infile) != NULL) {
		if(check&& forbidden[wordhash(line)])continue;// already loaded
		forbidden[wordhash(line)]=true;
			//contains(blacklist,line,true/*ignoreCase*/))
		fixNewline(line);
		addStatement(blacklist, Part, getAbstract(line),false);
		addStatement(blacklist, Part, getAbstract(concat(line,"e")),false);// sein -> seine  Berg -> Berge
		addStatement(blacklist, Part, getAbstract(concat(line,"en")),false);// sein -> seinen Berg -> Bergen
//		forbidden.push_back(editable(line));
	}
	return forbidden;
//	return blacklist;
}

// Amerika => http://de.netbase.pannous.com:81/html/828
NV findEntites(cchar* query0){
	char* query=modifyConstChar(query0);
	query=replaceChar(query,'.',' ');
	query=replaceChar(query,'?',' ');
	query=replaceChar(query,'!',' ');
	query=replaceChar(query,'(',' ');
	query=replaceChar(query,')',' ');
	query=replaceChar(query,'%',' ');
	NV all;
	NV entities;// Merkel
	NV classes; // Politiker
	NV topics;	// Politik
//	vector<cchar*> forbidden=loadBlacklist();
//	N forbidden=loadBlacklist();
	map<int,bool> forbidden=loadBlacklist();

//	if(hasWord(query0)&&!forbidden[wordhash(query0)])
//		all.push_back(getAbstract(query0));// quick
	int max_words=6;// max words per entity: 'president of the United States of America' == 7
	//	int min_chars=4;//
	int min_chars=2;// VW ? :(
	int len=(int)strlen(query);
	char* start=query;
	char* last=query;
	char* end=&query[len];
	char* mid=strstr(start," ");
	if(!mid)mid=end;// 1 word queries
	while(start<end){
		int words=1;
		while(mid<=end && words<max_words && mid-start>=min_chars){
			mid[0]=0;// Artificial cut
//			if(!forbidden[ wordhash(start)]){
//			p(start);
			N entity=hasWord(start);
			if(!entity && endsWith(start, "s")){ //!germanLabels &&
				mid[-1]=0; // ^^ Minimum stemming
				entity=hasWord(start);// abstract OK
				mid[-1]='s';// HAHA HAxk! ;)
			}
			if(!entity && germanLabels && endsWith(start, "e")){ // Berge -> Berg
				mid[-1]=0; // ^^ Minimum stemming
				entity=hasWord(start);// abstract OK
				mid[-1]='e';// HAHA HAxk! ;)
			}
			// the United https://www.wikidata.org/wiki/Q7771566
			// 239790	United				9 statements
			if(atoi(start))entity=0;// no numbers hack
			if(entity){
				//				p(entity);
//				if(!contains(forbidden,entity->name,true/*ignoreCase*/))
				if(!forbidden[ wordhash(entity->name)]){
					all.push_back(entity);
					if(!isAbstract(entity)){
						entity->kind=abstractId;
						insertAbstractHash(entity,true);// fix bug!
					}
					string ename=string(start)+" "+last;
					if(start!=last&& !forbidden[ wordhash(ename.data())]){
						entity=hasWord(ename.data());
						if(entity)all.push_back(entity);
					}

				}else{
					pf("blacklisted: %s\n",entity->name);
				}
			}
			mid[0]=' ';// fix
//		}
			if(mid==end)break;
			mid=strstr(mid+1," ");// expand
			if(!mid)mid=end;
			words++;
		}
		last=start;
		while(start[0]!=' ' && start!=end) start++;
		start[0]=0;// cut last word!
		start++; // skip ' '
		if(start>=end)break;
		mid=strstr(start," ");// first sub-word
		if(!mid)mid=end;
	}
	free(query);
	return filterCandidates(all);
}
//
//// ignore space!
//NV findSubEntites(cchar* query0){
//	char* query=modifyConstChar(query0);
//	NV all;
//	NV entities;// Merkel
//	NV classes; // Politiker
//	NV topics;	// Politik
////	vector<cchar*> forbidden=
////	N forbidden=loadBlacklist();
//
//	map<int,bool> forbidden=loadBlacklist();
//	int min_chars=4;// inefficient!
//	int max_chars=40;
//	int len=(int)strlen(query);
//	char* start=query;
//	char* end=&query[len];
//	char* mid=start+min_chars;
//	while(start<end){
//		mid=start+min_chars;
//		while(mid<=end && mid-start<=max_chars && mid-start>=min_chars){
//			mid[0]=0;// Artificial cut
//			p(start);
//			N entity=hasWord(start);
//			mid[0]=' ';
//			if(entity && !contains(forbidden,entity->name,true/*ignoreCase*/)){
//				all.push_back(entity);
//			}
//			if(mid==end)break;
//			mid=mid+1;
//		}
//		start++; // skip ' '
//		if(start>=end)break;
//	}
//	free(query);
//	return filterCandidates(all);
//}
bool stopTopic(N t){
	if(t->id==5)return true;
	return false;
}
NV sortTopics(NV topics,N entity){
	deque<Node*> sorted;
	for(int i=0;i<(int)topics.size();i++){
		N topic=topics[i];
		if(!checkNode(topic))continue;
		if(eq(topic->name,"◊"))continue;
		if(eq(topic->name,entity->name))continue;
//		if(contains(entity->name, getText(topic))) // a great politician , politician
//		sorted.push_back(topic);
//		else
		sorted.push_front( topic);
		if(stopTopic(topic))
			break;
	}
//	topics.clear();
	for (int i=0; i<sorted.size(); i++) {
		topics[i]=sorted[i];
	}
	return topics;
}


extern "C"
Node* getType(Node* n){
	return getProperty(n,Type,1000);
//	Statement* s=findStatement(n,Type,Any);
//	if(!checkStatement(s))return 0;// n
//	return s->Object();
}

N getClass(N n){
	N p=0;
	if(!p)p=getProperty(n,SuperClass,1000);// more specific: Transportflugzeug
	if(!p)p=getProperty(n,get(-10031),1000);//  	Ist ein(e) :(
	if(!p)p=getProperty(n,Type,1000); // Typ Flugzeug
	if(!p && n->kind>0)
		return get(n->kind);
	if(!p){
//		if(!p)p=getProperty(n,Type,1500);
//		if(p)addStatement(n, Type, p,false);
//		else
		{
			if(n->statementCount>3 && !atoi(n->name))
			pf("Unknown type: %s %d\n",n->name,n->id);
			return Entity;// i.e. President #7241205 (kind: entity #-104), 1 statements --- Single von IAMX
		}
	}
	return p;
}

N getTopic(N node){
//	NV all=getTopics(node);
//	return getFurthest(node,parentFilter);
	return getFurthest(node,topicFilter);
//	if(all.size()==0)return Entity;
//	return all[all.size()-1];// best?
//	return all[0];// best?
//	return getClass(n);
}

N getTopic2(N n){// n-Titty = entity
	return Error;
	if(isAbstract(n))
		return Abstract;
	//		n=getThe(n);// best!
	//		NV classes=findStatements(n->id, _SuperClass, _any);
	NV papas=findProperties(n,SuperClass);
	if(papas.size()==0)
		papas=findProperties(n,Type);
	if(papas.size()>0){
		papas=sortTopics(papas, n);
		N p=papas[0];
		//		if(p->id==134556)return 0;// Single : ignore!
		//		if(p->id==11424)return 0;// Film 'Eine Familie' wtf : ignore!
		if(eq(p->name,"◊"))return Entity;
		if(p->id!=4167836)// Wikimedia-Kategorie
			return p;
	}else
		pf("Unknown type: %s\n",n->name);
	return Entity;// i.e. President #7241205 (kind: entity #-104), 1 statements --- Single von IAMX
}

NV getTopics(N entity){
	NodeSet all=findAll(entity, parentFilter);
	NV topics=	setToVector(all);
	topics=sortTopics(topics,entity);
	return topics;
}

NV getTopics(NV entities){
	NV topics;
	for(int i=0;i<(int)entities.size();i++){
		N entity=entities[i];
		N topic=getTopic(entity);
		topics.push_back(topic);
		}
	return topics;
}

NV showTopics(NV entities){
	NV all= getTopics(entities);
	bool splitAbstract=false;
	for(int i=0;i<(int)all.size();i++){
	N entity=all[i];
	printf("====================================\n");
	if(isAbstract(entity)){
		if(!splitAbstract)continue;
		NV instances=allInstances(entity);
		//			all_instances3(Node* type, int recurse, int max, bool includeClasses)
		for(int j=0;j<(int)instances.size();j++){
			entity=instances[j];
			NV topics=getTopics(entity);
			if(topics.size()<=2)continue;// self + abstract
			printf("------------------------------------\n");
			show(topics);
		}
	}else{
		NV topics=getTopics(entity);
		//		show(topics);
	}
	}
	return all;
}


// see getProperty
Node* findProperty(Node* n , Node* m,bool allowInverse,int limit){
	Statement* s=0;
	int count=0;
	while ((s=nextStatement(n,s))) {
		if(limit && count++>limit)break;
		if(s->Predicate()==m){
			return s->Object();
		}
		if (allowInverse && s->Predicate()==invert(m)) {
			return s->Subject();
		}
	}
	return 0;
}

// see findProperty
Node * getProperty(Node* node, Node* key,int limit) {
	S s=findStatement(node, key, Any ,0,0,0,true,limit);
	if(!checkStatement(s))return 0;
	return s->Object()==node ? s->Subject() : s->Object();// inverse
}

// see findProperty
Node * getProperty(Node* node, cchar* key,int limit) {
	// int recurse = false, bool semantic = useSemantics, bool symmetric = false,bool semanticPredicate=useSemantics, bool matchName=false,int limit=lookupLimit
	S s=findStatement(node, getThe(key), Any ,0,0,0,true,limit);
	//findStatement(node, getAbstract(key), Any); // todo? egal
	if(!checkStatement(s))return 0;
	return s->Object();
}

// see getProperty
Node* findProperty(Node* n , const char* m,bool allowInverse,int limit){
	Statement* s=0;
	int count=0;
	while ((s=nextStatement(n,s))) {
		if(limit && count++>limit)break;
		if(eq(s->Predicate()->name,m,true))
			if(allowInverse||eq(s->Subject(),n)){
//				p("TODO!?!");
				return s->Object();// whooot??? a.typ=b  b.typ!=a !?!?!?!?!
			}
	}
	return 0;
}

NodeVector findProperties(Node* n, const char* m,bool allowInverse/*=true*/){
	if(!m)return EMPTY;// !checkNode(n)
	if(isInteger(m)) m=getThe(m)->name;
	NodeVector good;
	Statement* s=0;
	while ((s=nextStatement(n,s))) {// todont: lookuplimit
		if(eq(s->Predicate()->name,m,true)){
			if(s->Object()==n&&allowInverse)// wrong semantics egal  makes of mazda  "1991 Mazda 323 Hatchback		Make		Mazda"
				good.push_back(s->Subject());
			else if (!contains(good, s->Object(), false))
				if(allowInverse||eq(s->Subject(),n))
					good.push_back(s->Object());
			if(good.size()>resultLimit)return good;
		}
	}
	return good;
}


NodeVector findProperties(Node* n , Node* m,bool allowInverse/*=true*/){
	NodeVector good;
	if(!m){
		pf("Warning: empty property null for node %d\t%s",n->id,n->name);
		return good;
	}
	NV all;
	if(isAbstract(n))
		all=instanceFilter(n);// need big lookuplimit here :( todo: filter onthe fly!
	//    if(!isAbstract(n))
	all.push_back(n);// especially for freebase singletons!
	// OR//    findStatement(Node *subject, Node *predicate, Node *object)
	for (int i=0; i<all.size(); i++){
		mergeVectors(&good, findProperties(all[i],m->name,false));
		if(good.size()>resultLimit)return good;
	}
	if(allowInverse)
		for (int i=0; i<all.size(); i++){
			mergeVectors(&good, findProperties(all[i],m->name,true));
			if(good.size()>resultLimit)return good;
		}
	return good;
}

NodeVector findProperties(const char* n, const char* m,bool allowInverse){
	if(isInteger(n)) return findProperties(getAbstract(n),m,allowInverse);
	if(isInteger(m)) return findProperties(getAbstract(n),getThe(m),allowInverse);
	NodeVector good;
	N a=getAbstract(n);
	NV all=    instanceFilter(a);// need big lookuplimit here :( todo: filter onthe fly!
	all.push_back(a);// especially for freebase singletons!
	for (int i=0; i<all.size(); i++){
		NV more=findProperties(all[i],m,allowInverse);
		mergeVectors(&good, more);
		if(good.size()>resultLimit)return good;
	}
	return good;// dedup(good);
}


Node * has(Node* n, Node * m) {
	clearAlgorithmHash(true);
	int tmp=resultLimit;
	resultLimit=1;
	NodeVector all=memberPath(n, m);
	resultLimit=tmp;
	if (all.size() > 0) return all.front();

	// how to find paths with property predicates?? so:
	clearAlgorithmHash();
	Node *no=0;
	if (!no) no=has(n, m, Any); // TODO: test
	return no; // others already done!!

	// deprecated:
	//	if (m->value.text != 0)// hasloh population:3000
	//		no = has(n, m, m->value); // TODO: test
	//    findPath(n,m,hasFilter);// Todo new algoritym
	if (!no) no=has(n, Part, m);
	if (!no) no=has(n, Attribute, m);
	if (!no) no=has(n, Substance, m);
	if (!no) no=has(n, Member, m);
	if (!no) no=has(n, UsageContext, m);
	if (!no) no=has(n, get(_MEMBER_DOMAIN_CATEGORY), m);
	if (!no) no=has(n, get(_MEMBER_DOMAIN_REGION), m);
	if (!no) no=has(n, get(_MEMBER_DOMAIN_USAGE), m);
	//inverse
	if (!no) no=has(m, Owner, n);
	if (!no) no=has(m, PartOf, n);
	if (!no) no=has(m, get(_DOMAIN_CATEGORY), n);
	if (!no) no=has(m, get(_DOMAIN_REGION), n);
	if (!no) no=has(m, get(_DOMAIN_USAGE), n);

	//    if(!n)n=has(n,Predicate,m);// TODO!
	//	if (!no)no = has(save, Any, m); //TODO: really?
	return no;
}

Statement * findRelations(Node* from, Node * to) {
	Statement* s=findStatement(from, Any, to, false, false, false, false);
	if (!s) return findStatement(to, Any, from, false, false, false, false);
	else return s;
}

Node * findRelation(Node* from, Node * to) {	// todo : broken Instance !!!
	Statement* s=findStatement(from, Any, to, false, false, false, false);
	if (!s) s=findStatement(to, Any, from, false, false, false, false);
	if (s) {
		if (s->Subject() == from) return s->Predicate();
		if (s->Object() == to) return s->Predicate();
		if (s->Subject() == to) return invert(s->Predicate());
		if (s->Object() == from) return invert(s->Predicate());
		//		if(s->Subject==from)return s->Predicate;
		//		else if(s->Object==from) return invert(s->Predicate);
		//		if(s->Object==to)return s->Predicate;
		//		else if(s->Subject==to) return invert(s->Predicate);
	}
	return null;
}

bool hasValue(Node * n) {
	return (*(int*) &n->value) != 0;
}

//bool isA4(Node* n, string match, bool recurse, bool semantic){
// return	isA4(n, match, recurse?1:0, semantic);
//}
bool isA4(Node* n, string match, int recurse, bool semantic) {

	if (!checkNode(n)) return false;
	if (eq(n->name, match.c_str())) return true; //&& n->name==match
	if (get(n->kind) && eq(get(n->kind)->name, match.c_str())) return true;

	if (recurse > 0) recurse++;
	else recurse=maxRecursions;
	if (recurse > maxRecursions) return false;

	if (semantic && has(n, "synonym", match, false, false, true)) return true;
	if (semantic && has(n, "plural", match, false, false, true)) return true;
	if (semantic && has(n, "type", match, false, false, false)) return true;
	if (semantic && has(n, "parent", match, false, false, false)) return true;
	//    if(semantic && has(n,"kind",match,false,false,false))return true;//?
	if (semantic && has(n, "name", match, false, false, false)) return true;
	if (semantic && has(n, "label", match, false, false, false)) return true;
	if (semantic && has(n, "is", match, false, false, false)) return true; // --

	if (semantic && n->kind == Abstract->id) { //|| isA(n,List)
		Statement* s=0;
		while ((s=nextStatement(n, s, false))) {
			if (s->Predicate() == Instance) if (isA4(s->Object(), match, recurse, semantic)) return true;
			if (s->Predicate() == Type) if (isA4(s->Subject(), match, recurse, semantic)) return true;
		}
		//		for (int i = 0; i < n->statementCount; i++) {// todo : iterate nextStatement(n,current)
		//			Statement* s = getStatementNr(n, i);
		//			if (s->Predicate == Instance)
		//				if (isA4(s->Object, match, recurse, semantic))
		//					return true;
		//		}
	}
	return false;
}


bool isA4(Node* n, Node* match, int recurse, bool semantic, bool matchName) {
	if (n == match) return true;
	if (!n || !n->name || !match || !match->name) return false; //!!
	if (n->kind == match->id) return true; //
	//	if (n->id < 100 && match->id < 100)return false; // danger!
	if (get(n->kind) && eq(get(n->kind)->name, match->name, true))return true;	// danger: instance, noun
	if (n->id == match->id) return true; // how so??? "Type" overwritten by "kind" !!!!
	if(isAbstract(match))matchName=true;
	if (matchName&&eq(n->name, match->name, true)) return true;// only sometimes !!!
	long badHash=n->id + match->id * 10000000;
	if (useYetvisitedIsA) {
		if (yetvisitedIsA[badHash] == -1) return false;
		if (yetvisitedIsA[badHash] == 1) return true;
	}
	//        else if (yetvisitedIsA[n]==null)yetvisitedIsA[n]=true;
	//        else yetvisitedIsA[n]++;

	if (recurse > 0) recurse++;
	//    else recurse=maxRecursions;
	if (recurse > maxRecursions) return false;
	runs++;

	if (hasValue(n)) {	// false else?
		//        if(isA4(n->kind,match->kind))
		if (n->kind == match->kind) {
			if (n->value.number == match->value.number) return true;
			if (n->value.datetime == match->value.datetime) return true;
			if (n->value.statement == match->value.statement) return true;
			else return false;
		}
	}
	//        if (recurse > maxRecursions / 3)
	//            semantic = false;


	// todo:semantic true (level1)
	bool quickCheckSynonym=recurse == maxRecursions; // todo !?!??!
	if (quickCheckSynonym && findStatement(n, Synonym, match, false, false, true)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}

	bool semantic2=semantic && recurse > 5; // && ... ?;

	if (semantic && findStatement(n, Synonym, match, recurse, semantic2, true)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}
	//    if(semantic && has(n,Plural,match,false,false,true))return true;
	if (semantic && has(n, SuperClass, match, recurse, semantic2, false)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}
	if (semantic && has(n, Type, match, recurse, semantic2, false)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}
	if (semantic && has(match, Instance, n, recurse, semantic2, false)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}
	if (semantic && has(match, SubClass, n, recurse, semantic2, false)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}
	if (semantic && has(n, Label, match, false, false, false)) {
		yetvisitedIsA[badHash]=true;
		return true;
	}
	if (semantic && recurse > 0 && findStatement(n, Plural, match, maxRecursions - 1, semantic2)) {
		yetvisitedIsA[badHash]=true;
		return true;
	} //(n,Plural,match,0,true,true))return true;
	if (semantic && recurse > 0 && findStatement(match, Plural, n, maxRecursions - 1, semantic2)) {
		yetvisitedIsA[badHash]=true;
		return true;
	} //(n,Plural,match,0,true,true))return true;

	//    if(isA(n,match->name,false,false))return true;// compare by name
	if (isAbstract(n) && recurse>0 && recurse < 3) { //|| isA(n,List)
		Statement* s=0;
		map<Statement*, bool> visited;
		while ((s=nextStatement(n, s, false))) {
			if (visited[s]) return 0;
			visited[s]=1;
			if (s->Predicate() == Instance) if (recurse && isA4(s->Object(), match, recurse, semantic)) {
				yetvisitedIsA[badHash]=true;
				return true;
			}
		}
	}
	if(useYetvisitedIsA)
		yetvisitedIsA[badHash]=-1;

	return false;
}

// todo: a.b=c findMember(b,b) error
Node * findMember(Node* n, string match, int recurse, bool semantic) {
	if (!n) return 0;
	if (recurse > 0) recurse++;
	if (recurse > maxRecursions) return 0;
	//	if (debug&&!eq(match.data(),"wiki_image"))
	//		show(n);
	for (int i=0; i < n->statementCount; i++) {
		Statement* s=getStatementNr(n, i); // Not using instant gap
		if (!s) {
			bad();
			continue;
		}
		//		if (debug)showStatement(s);
		if (isA4(s->Predicate(), match, recurse, semantic)){
			if (s->Subject() == n) return s->Object();
			else return s->Subject();
		}
		if (isA4(s->Object(), match, recurse, semantic)) return s->Object();

		if (isA4(s->Subject(), match, recurse, semantic)) return s->Subject();
	}
	return null;
}


//NodeVector & allInstances(Node * type) {
NodeVector allInstances(Node * type, int recurse, int max, bool includeClasses) {
	clearAlgorithmHash();
	NodeVector all; 	// COMPARE: !!
	INCLUDE_CLASSES=includeClasses;// todo: param!
	if(!recurse)
		all = instanceFilter(type);
	//	all = all_instances(getQuery(type));
	else
		all = all_instances(type, recurse, max, includeClasses);
	//	all = recurseFilter(type,true,resultLimit,instanceFilter);
	return all;
}
