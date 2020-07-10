#include "Cache.h"
#include <iostream>
#include <fstream>
#include <sstream>
using std::cout;
using std::endl;
using std::ifstream;
using std::make_pair;
using std::ofstream;
using std::stringstream;

namespace wd
{
    Cache::Cache(size_t capacity)
        : _capacity(capacity), _pairList(), _hashmap()
    {
    }

    int Cache::get(string key)
    {
        auto it = _hashmap.find(key);
        // 访问的 key 不存在
        if (it == _hashmap.end())
            return -1;
        // key 存在，把 (k, v) 换到队头
        pair<string, int> kv = *_hashmap[key];
        _pairList.erase(_hashmap[key]);
        _pairList.push_front(kv);
        // 更新 (key, value) 在 _pairList 中的位置
        _hashmap[key] = _pairList.begin();
        return kv.second; // value
    }

    void Cache::put(string key, int value)
    {

        /* 要先判断 key 是否已经存在 */
        auto it = _hashmap.find(key);
        if (it == _hashmap.end())
        {
            /* key 不存在，判断 _pairList 是否已满 */
            if (_pairList.size() == _capacity)
            {
                // _pairList 已满，删除尾部的键值对腾位置
                // _pairList 和 _hashmap 中的数据都要删除
                auto lastPair = _pairList.back();
                string lastKey = lastPair.first;
                _hashmap.erase(lastKey);
                _pairList.pop_back();
            }
            // _pairList 没满，可以直接添加
            _pairList.push_front(make_pair(key, value));
            _hashmap[key] = _pairList.begin();
        }
        else
        {
            /* key 存在，更改 value 并换到队头 */
            _pairList.erase(_hashmap[key]);
            _pairList.push_front(make_pair(key, value));
            _hashmap[key] = _pairList.begin();
        }
    }

    void Cache::readFromFile(const string &file)
    {
        ifstream ifs(file, ifstream::in);
        if (!ifs.good())
        {
            cout << file << "open ERROR!" << endl;
            exit(-1);
        }

        string lines;
        string first;
        int second;
        while (getline(ifs, lines))
        {
            stringstream ss(lines);
            ss >> first >> second;
            put(first, second);
        }
        ifs.close();
    }

    void Cache::writeToFile(const string &file)
    {
        ofstream ofs(file, ofstream::out);
        if (!ofs.good())
        {
            cout << file << "open ERROR!" << endl;
            exit(-1);
        }

        for (auto &e : _hashmap)
        {
            ofs << e.first << " " << get(e.first) << endl;
        }
        ofs.close();
    }

    //注意此处应该使用深拷贝的方法
    void Cache::update(const Cache* lhs)
    {
        _pairList = lhs->_pairList;
        _hashmap.erase(_hashmap.begin(), _hashmap.end());
        for (auto it = _pairList.begin(); it != _pairList.end(); it++)
        {
            _hashmap[it->first] = it;
        }
    }

    unordered_map<string, list<pair<string, int>>::iterator> &Cache::getMap()
    {
        return _hashmap;
    }

} // namespace wd