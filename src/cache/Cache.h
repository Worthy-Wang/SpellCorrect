#ifndef _MY_CACHE_H_
#define _MY_CACHE_H_
#include <unordered_map>
#include <string>
#include <list>
using std::list;
using std::pair;
using std::string;
using std::unordered_map;

namespace wd
{
    //内存Cache 每个线程都拥有一个，每十分钟写回磁盘Cache
    //采用LRU算法进行缓存淘汰
    class Cache
    {
    public:
        Cache(size_t capacity);
        int get(string key);                   //获取单词对应频率, 采用LRU算法
        void put(string key, int value);       //插入新的 <单词，频率>， 采用LRU算法
        void readFromFile(const string &file); //从磁盘Cache读取数据
        void writeToFile(const string &file);  //向磁盘Cache写数据
        void update(const Cache* lhs);               //更新Cache
        unordered_map<string, list<pair<string, int>>::iterator>& getMap();

    private:
        size_t _capacity;
        list<pair<string, int>> _pairList;                                 // 双链表：装着 (key, value) 元组
        unordered_map<string, list<pair<string, int>>::iterator> _hashmap; // 哈希表：key 映射到 (key, value) 在 _pairList 中的位置
    };
} // namespace wd

#endif