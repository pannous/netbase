#define _FILE_OFFSET_BITS 64
#include "netbase.hpp"
#include "util.hpp"
#include <sys/stat.h> // mkdir




bool save() {
  Context* c = getContext(current_context);
	mkdir((path+"data").c_str(),0777);
	string data_path=path+"data/";

	p("saving context blob to "+data_path);
	p("This can take a few minutes");
  FILE *fp;
  fp = fopen((data_path+"statements.bin").data(), "wb");
  fwrite(c->statements, (unsigned long int) sizeof (Statement),(unsigned long int) (c->statementCount), fp);
  fclose(fp);

	fp = fopen((data_path+"nodes.bin").data(), "wb");
//	fwrite(c->nodes-propertySlots, sizeof (Node), maxNodes, fp);
	fwrite(c->nodes-propertySlots, sizeof (Node), max(c->nodeCount,c->lastNode), fp);
//	fwrite(c->nodes-propertySlots, sizeof (Node), findLastNode(), fp);// or fix nodeCount / lastNode
//	fwrite(c->nodes-propertySlots, sizeof (Node), c->lastNode, fp);
//	fwrite(c->nodes-propertySlots, sizeof (Node), c->nodeCount, fp);
  fclose(fp);

  fp = fopen((data_path+"names.bin").data(), "wb");
  fwrite(c->nodeNames, sizeof (char), c->currentNameSlot + 100, fp);
  fclose(fp);

  fp = fopen((data_path+"contexts.bin").data(), "wb");
  fwrite(contexts, sizeof (Context), maxContexts, fp);
  fclose(fp);

//  fp = fopen((path+"wordnet.bin").data(), "wb");
//  fwrite(c, sizeof (Context), 1, fp);
//  fclose(fp);
//
  fp = fopen((data_path+"abstracts.bin").data(), "wb");
	fwrite(abstracts, sizeof (Ahash), maxNodes*2, fp);// todo
  fclose(fp);

  ps("context blob saved!");
	printf("All %ld nodes and %d registered nodes dumped to ./data/ folder \n",maxNodes,c->nodeCount);

  //  fp=fopen("test.bin", "wb");
  //  fwrite(test, sizeof(char), 100, fp);
  //  fclose (fp);
	return true;
}

// what for??
bool export_csv(){
  Context* c = context;
  FILE *fp;
	// To write numbers in octal, precede the value with a 0. Thus, 023 is 238 (which is 19 in base 10).
	mkdir((path+"export").c_str(),0777);
  fp = fopen((path+"export/nodes.csv").data(), "w");
	if(!fp){
		ps("CANT ACCESS "+(path+"export/nodes.csv"));
		return false;
	}
  for (int i = 0; i < minimum((long)c->nodeCount,maxNodes); i++) {
    Node* n= &c->nodes[i];
		if(!checkNode(n,i))continue;
    fprintf(fp,"%d\t%s\n",n->id,n->name);
//    fprintf("%d\t%s\t%d",n->id,n->name,n->kind);//n->context
  }
  fp = fopen((path+"export/statements.csv").data(), "w");
  for (int i = 0; i < c->statementCount; i++) {
    Statement* n= &c->statements[i];
		if(!checkStatement(n,true,false))continue;
    fprintf(fp,"%d\t%d\t%d\n",n->subject,n->predicate,n->object);//,n->kind);//n->context
  }

  // what for?
  // for 'debugging' ?
  // only makes sense with 'no ambiguity names' apple_(tree), in_(location), ...
  fp = fopen((path+"export/statement-names.csv").data(), "w");
  for (int i = 0; i < c->statementCount; i++) {
    Statement* n= &c->statements[i];
		if(!checkStatement(n,true,true))continue;
		if(n->Predicate()==Instance)continue;
    fprintf(fp,"%d\t%d\t%d\t%s\t%s\t%s\n",n->subject,n->predicate,n->object,n->Subject()->name,n->Predicate()->name,n->Object()->name);//,n->kind);//n->context
  }
  fclose(fp);

  ps("context CSV saved!");
	printf("%d nodes dumped to ./data/ folder ",c->nodeCount);
	return true;
}

