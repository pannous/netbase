/*
 * File:  init.hpp
 * Author: me
 *
 * Created on April 4, 2012, 11:29 AM
 */

#ifndef INIT_HPP
#define	INIT_HPP
bool clearMemory();
void releaseSharedMemory();
void load(bool force);
bool save();
extern "C" void initSharedMemory(bool relations=true);
extern "C" void allowWipe();
extern "C" void init(bool relations); // synonym ^^
//int semrm(key_t key, int id);
int share_memory();
extern "C" void setMemoryLimit(long maxNodes0,long maxStatements0=-1,long maxChars0=-1);
//char* share_memory(size_t key,long sizeOfSharedMemory,char* root,const void * desired);
void* share_memory(size_t key, long long sizeOfSharedMemory, void* root, const void * desired);
void fixPointers();
void fixPointers(Context* context);
void fixNodeNames(Context* context, char* oldnodeNames);
void loadMemoryMaps();
#endif	/* INIT_HPP */

