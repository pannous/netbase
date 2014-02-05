/*
 * File:   init.hpp
 * Author: me
 *
 * Created on April 4, 2012, 11:29 AM
 */

#ifndef INIT_HPP
#define	INIT_HPP
bool clearMemory();
void load(bool force=false);
extern "C" void init(bool relations=true);
//int semrm(key_t key, int id);
int share_memory();
//char* share_memory(size_t key,long sizeOfSharedMemory,char* root,const void * desired);
void* share_memory(size_t key, long sizeOfSharedMemory, void* root, const void * desired);
void fixPointers();
void fixPointers(Context* context);
void fixNodeNames(Context* context, char* oldnodeNames);

#endif	/* INIT_HPP */

