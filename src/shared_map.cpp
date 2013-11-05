#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <boost/unordered_map.hpp>     //boost::unordered_map
#include <functional>                  //std::equal_to
#include <boost/functional/hash.hpp>   //boost::hash


#include "shared_map.hpp"
#include <stdio.h>
////  g++ -I/opt/local/include/ test-boost-unordered_map.cpp

using namespace boost::interprocess;

//static
//managed_shared_memory* segment;
//static
//typedef int  KeyType;
//typedef int  ValueType;
//typedef std::pair<const KeyType, ValueType> KeyValuePair;

//Note that unordered_map<Key, MappedType> allocator must allocate that ValueType pair.
typedef allocator<KeyValuePair, boost::interprocess::managed_shared_memory::segment_manager> ShmemAllocator;

//Alias an unordered_map of ints that uses the previous STL-like allocator.
typedef boost::unordered_map< KeyType, ValueType
, boost::hash<KeyType>  ,std::equal_to<KeyType>
, ShmemAllocator>
mymap;


//mymap *themap;
//typedef class shared_map
//typedef

shared_map::shared_map() {
    //    shared_memory_object::remove("shared_map_memory");
    //    shared_memory_object::remove("shared_map_memory2");
    clear();
    load () ;
}
shared_map::~shared_map(){
    //    shared_memory_object::remove("shared_map_memory");
}

int initial_buckets=10000000;
const char* shared_map_memory= "shared_map_memory";
managed_shared_memory segment(open_or_create, shared_map_memory,4* 1024*1024*1024);// 1 GB limit !?!
static mymap *themap=segment.find_or_construct<mymap>("shared_map")  //object name
(  initial_buckets, boost::hash<KeyType>(), std::equal_to<KeyType>() , segment.get_allocator<KeyValuePair>());


void shared_map::clear(char* shared_map_memory_name){
    try{
        shared_memory_object::remove(shared_map_memory_name);
    }catch(...){}
}

void shared_map::clear(){
    clear("shared_map_memory2");
    clear("shared_map_memory");
//    clear("shared_map_memory2");
//    clear("shared_map_memory3");
//    clear("shared_map_memory4");
//    clear("shared_map_memory5");
//    clear("shared_map_memory6");
//    clear("shared_map_memory7");
//    clear("shared_map_memory8");
    //    shared_memory_object::remove("shared_map_memory2");
//    shared_memory_object::remove(shared_map_memory);
}

ValueType shared_map::operator[](KeyType index){
    if(themap->count(index)){
        return themap->at(index);
    }else{
        return 0;
    }
}
void shared_map::operator()(KeyType index,ValueType val){
    themap->insert(KeyValuePair(index, val));
}
void shared_map::insert(KeyType index,ValueType val){
    themap->insert(KeyValuePair(index, val));
}
void shared_map::insert(KeyValuePair x){
    themap->insert(x);
}
void shared_map::insert(KeyValuePair& x){
    themap->insert(x);
}

void shared_map::test(){
    shared_map x;
    x.insert(1,3);
    x.insert(4260783653535235,3);
    int y=x[1];
    int y2=x[426078365];
    int y4=x[4260783653535235];
    int y3=x[426078365];
}


void shared_map::load ()
{   //Create shared memory
    //    managed_shared_memory segment(open_or_create, "shared_map_memory", 100000000);
    //    managed_shared_memory segment(open_or_create, "shared_map_memory", 1024^3);
    
    //    int initial_buckets=51705469;
    //    int initial_buckets=100000;
    //Construct a shared memory hash map.
    //Note that the first parameter is the initial bucket count and
    //after that, the hash function, the equality function and the allocator
    
    //    managed_shared_memory segment(open_or_create, shared_map_memory, 1024^2);
    //    managed_shared_memory segment2(open_or_create, shared_map_memory, 1024*1024);
    //    segment=&segment2;
    //    clear();
    //    themap=segment2.find_or_construct<mymap>("shared_map")  //object name
    //    (  initial_buckets, boost::hash<KeyType>(), std::equal_to<KeyType>() , segment2.get_allocator<KeyValuePair>());
    
    // for(mymap::iterator i = themap->begin(); i != themap->end(); ++i){
    //      std::cout << i->first << "=" << i->second << std::endl;
    //    }
    //
    
    if(themap->count(1)){
        float v2=themap->at(1);// themap[1]; ???
        std::cout << "got " <<v2<< std::endl;
    }else{
        //Insert data in the hash map
        themap->insert(KeyValuePair((long)1,1));
        std::cout << "charged " << std::endl;
        //        if(themap->count(1)){
        float v2=themap->at(1);// themap[1]; ???
        std::cout << "got " <<v2<< std::endl;
        themap->count(1);
        themap->count(1343252532353325);
        themap->count(426078365);
        themap->count((long)426078365);        themap->count((int)426078365);
    }
}

