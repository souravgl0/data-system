#include "global.h"

Btree::Btree(int fanout)
{
    this->fanout = fanout;
}

int Btree::insert(int val, int nextPageIndex)
{
    if(tree.find(val) == tree.end())
    {
        tree[val]=nextPageIndex;
        return nextPageIndex;
    }
    return tree[val];
}

int Btree::find(int val)
{
    if(tree.find(val) == tree.end())return -1;
    return tree[val];
}

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
