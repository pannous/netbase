typedef long  KeyType;
typedef int  ValueType;
typedef std::pair<const KeyType, ValueType> KeyValuePair;

//typedef class shared_map
#ifndef CLASS_Y_DEFINED
#define CLASS_Y_DEFINED
class shared_map
{
    void clear(char* shared_map_memory_name);
public:
    shared_map();
    ~shared_map();
    void clear();
    ValueType operator[](KeyType index);
    void operator()(KeyType index,ValueType val);
    void insert(KeyType index,ValueType val);
//    void insert(std::pair<long,int> x);
    void insert(KeyValuePair& x);
    void insert(KeyValuePair x);
    void load ();
        void test();
};
#endif