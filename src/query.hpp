/*
 * File:   query.h
 * Author: me
 *
 * Created on March 28, 2012, 6:46 PM
 */
#include "util.hpp";

#ifndef QUERY_H
#define	QUERY_H

extern int resultLimit;
//string render_query(Query& q);// renderResults!
Query& getQuery(Node* keyword);
NodeVector query(Query& q);
NodeVector query(string s, int limit=resultLimit);
Node* match(string data);
Node* findMatch(Node* n, const char* match);
Statement* evaluate(string data);
int countInstances(Node* node);
string query2(string s,int limit=resultLimit);
Query parseQuery(string s,int limit=resultLimit);
Statement* pattern(Node* subject, Node* predicate, Node* object);
void clearAlgorithmHash(bool all=false);
NodeVector exclude(NodeVector some, NodeVector less);
NodeVector evaluate_sql(string s, int limit) ;
NodeVector find_all(char* name, int context=1, int recurse=0, int limit=resultLimit);
NodeVector& all_instances(Node* type, int recurse, int max= resultLimit);
NodeVector& all_instances(Node* type);
NodeVector& all_instances(Query& q);

NodeVector filter(Query& q, Node* _filter);
NodeVector filter(Query& q, Statement* filterTree);
NodeVector filter(NodeVector all, char* matches);


NodeVector filter(NodeVector all, Node* filterTree);
//NodeVector filter(NodeVector all, Statement* filterTree);
NodeVector filter(Query& q, Statement* filterTree);

bool enqueue(Node* current, Node* d, NodeQueue* q);
//typedef (NodeVector (*edgeFilter)(Node*)) NodeFilter;
NodeVector findPath(Node* fro, Node* to,NodeVector (*edgeFilter)(Node*,NodeQueue*) );

NodeVector parentFilter(Node* subject,NodeQueue* queue=null);
NodeVector memberFilter(Node* subject,NodeQueue* queue=null);
NodeVector hasFilter(Node* subject,NodeQueue* queue=null);
NodeVector childFilter(Node* subject,NodeQueue* queue=null);
NodeVector ownerFilter(Node* subject,NodeQueue* queue=null);
NodeVector anyFilter(Node* subject,NodeQueue* queue=null,bool includeRelations=true);
NodeVector instanceFilter(Node* subject,NodeQueue* queue=null);

//NodeVector parentFilter(Node* subject);
//NodeVector memberFilter(Node* subject);
//NodeVector hasFilter(Node* subject);
//NodeVector childFilter(Node* subject);
//NodeVector ownerFilter(Node* subject);
//NodeVector anyFilter(Node* subject);
//NodeVector instanceFilter(Node* subject);


NodeVector shortestPath(Node* from,Node* to );// any
NodeVector parentPath(Node* from, Node* to);
NodeVector memberPath(Node* from, Node* to);

#endif	/* QUERY_H */
