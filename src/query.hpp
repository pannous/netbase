/*
 * File:   query.h
 * Author: me
 *
 * Created on March 28, 2012, 6:46 PM
 */
#include "util.hpp";

#ifndef QUERY_H
#define	QUERY_H

static const int defaultLimit = 100;

//string render_query(Query& q);// renderResults!
Query& getQuery(Node* keyword);
NodeVector query(Query& q);
NodeVector query(string s, int limit=defaultLimit);
Node* match(string data);
Node* findMatch(Node* n, const char* match);
Statement* evaluate(string data);
int countInstances(Node* node);
string query2(string s,int limit=defaultLimit);
Query parseQuery(string s,int limit=defaultLimit);
Statement* pattern(Node* subject, Node* predicate, Node* object);
void clearAlgorithmHash();
NodeVector exclude(NodeVector some, NodeVector less);
NodeVector evaluate_sql(string s, int limit) ;
NodeVector filter(Query& q, Node* _filter);
NodeVector filter(Query& q, Statement* filterTree);
NodeVector filter(NodeVector all, char* matches);
NodeVector find_all(char* name, int context=1, int recurse=0, int limit=defaultLimit);
NodeVector& all_instances(Node* type, int recurse, int max= defaultLimit);
NodeVector& all_instances(Node* type);
NodeVector& all_instances(Query& q);
#endif	/* QUERY_H */

