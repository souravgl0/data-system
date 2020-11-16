class Btree
{
    int fanout;
    map<int, int> tree;
public:

    Btree(int fanout);
    int insert(int val, int nextPageIndex);
    int find(int val);
};


class LinearHash
{
    int buckets;
    unordered_map<int, int> hashmap;
public:

    LinearHash(int buckets);
    int insert(int val, int nextPageIndex);
    int find(int val);
};
