#include "global.h"

LinearHash::LinearHash(int buckets)
{
    this->buckets = buckets;
}

int LinearHash::insert(int val, int nextPageIndex)
{
    if(hashmap.find(val) == hashmap.end())
    {
        hashmap[val]=nextPageIndex;
        return nextPageIndex;
    }
    return hashmap[val];
}

int LinearHash::find(int val)
{
    if(hashmap.find(val) == hashmap.end())return -1;
    return hashmap[val];
}
