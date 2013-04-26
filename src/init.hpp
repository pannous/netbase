/*
 * File:   init.hpp
 * Author: me
 *
 * Created on April 4, 2012, 11:29 AM
 */

#ifndef INIT_HPP
#define	INIT_HPP
void clearMemory();
void load(bool force=false);
int share_memory();
void fixPointers();
void fixPointers(Context* context);
void fixNodeNames(Context* context, char* oldnodeNames);

#endif	/* INIT_HPP */

