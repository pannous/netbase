/*
 * File:   query.h
 * Author: me
 *
 * Created on March 28, 2012, 6:46 PM
 */
#include "util.hpp"

#ifndef QUERY_H
#define	QUERY_H

extern int resultLimit;
extern int defaultLookupLimit;
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
NodeVector find_all(cchar* name, int context=1, int recurse=0, int limit=resultLimit);
NodeVector& all_instances(Node* type, int recurse, int max= resultLimit,bool includeClasses=true);
NodeVector& allInstances(Node* type);
NodeVector& all_instances(Query& q);

NodeVector& recurseFilter(Node* type, int recurse, int max,NodeVector(*edgeFilter)(Node*, NodeQueue*));

NodeVector filter(Query& q, Node* _filter,int limit=0);
NodeVector filter(Query& q, Statement* filterTree,int limit=0);
NodeVector filter(NodeVector all, cchar* matches);


NodeVector filter(NodeVector all, Node* filterTree);
//NodeVector filter(NodeVector all, Statement* filterTree);

bool enqueue(Node* current, Node* d, NodeQueue* q);
//typedef (NodeVector (*edgeFilter)(Node*)) NodeFilter;
NodeVector findPath(Node* fro, Node* to,NodeVector (*edgeFilter)(Node*,NodeQueue*) );

NodeVector parentFilter(Node* subject,NodeQueue* queue=null);
NodeVector memberFilter(Node* subject,NodeQueue* queue=null);
NodeVector hasFilter(Node* subject,NodeQueue* queue=null);
NodeVector childFilter(Node* subject,NodeQueue* queue=null);
NodeVector ownerFilter(Node* subject,NodeQueue* queue=null);
NodeVector anyFilter(Node* subject,NodeQueue* queue=null,bool includeRelations=true);
NodeVector instanceFilter(Node* subject,NodeQueue* queue=null);//, int max= lookupLimit resultLimit);
NodeVector relationsFilter(Node* subject, NodeQueue * queue=null);

NodeVector parseProperties(const char *data);
NodeVector update(cchar* query);
NodeVector nodeVectorWrap(Node* n);
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
