#include "Mydict.h"
#include "Configuration.h"
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
    Mydict *Mydict::getpInstance() //饿汉模式，线程安全
    {
        return _pInstance;
    }

    void Mydict::destroy()
    {
        if (_pInstance)
        {
            delete _pInstance;
            _pInstance = NULL;
        }
    }

    void Mydict::query(const string &queryWord, priority_queue<MyResult, vector<MyResult>, MyCompare> &_priQue)
    {

        //首先拆分queryWord为单个的字符
        set<int> myset; //存放与查询单词相关的所有下标
        vector<string> singleCharVec = divide(queryWord);
        for (auto &e : singleCharVec)
        {
            //如果索引表中有相关的单词，则加入优先队列
            if (_indexMap.find(e) != _indexMap.end())
            {
                set<int> temp = _indexMap[e];
                for (auto &idx : temp)
                {
                    myset.insert(idx); //汇总所有出现的下标（自动去重）
                }
            }
        }
        for (auto &idx : myset)
        {
            string word = _dict[idx].first;
            int frequency = _dict[idx].second;
            int distance = editDistance(queryWord, word);
            MyResult res(word, frequency, distance);
            _priQue.push(res);
        }
    }

    //初始化中英文字典路径
    void Mydict::initDict(const string &english_dict_path, const string &chinese_dict_path, const string &cache_dir)
    {
        //初始化字典_dict
        SplitTool split;
        DictProducer dictProducer(english_dict_path, chinese_dict_path, &split);
        unordered_map<string, int> dict = dictProducer.getDict();
        vector<pair<string, int>> temp(dict.begin(), dict.end());
        swap(temp, _dict);
        //写入磁盘Cache中
        writeToCache(cache_dir);

        //根据创建好的字典，初始化索引表_indexMap
        for (size_t i = 0; i < _dict.size(); i++)
        {
            //singleCharVec中存放拆分的单个中英文字符集合
            vector<string> singleCharVec = divide(_dict[i].first);
            for (auto &e : singleCharVec)
            {
                _indexMap[e].insert(i);
            }
        }
    }

    //通过磁盘Cache读取字典, 之后的内存Cache回写磁盘Cache也回重新调用
    void Mydict::initDict(const string &cache_dir)
    {
        //清空原_dict 与 _indexMap
        _dict.resize(0);
        _indexMap.erase(_indexMap.begin(), _indexMap.end());

        //初始化字典_dict
        readFromCache(cache_dir);

        //根据创建好的字典，初始化索引表_indexMap
        for (size_t i = 0; i < _dict.size(); i++)
        {
            //singleCharVec中存放拆分的单个中英文字符集合
            vector<string> singleCharVec = divide(_dict[i].first);
            for (auto &e : singleCharVec)
            {
                _indexMap[e].insert(i);
            }
        }
    }

    //中英文字符串分割成单词字符
    vector<string> Mydict::divide(const string &str)
    {
        vector<string> ans;
        string strChar;
        for (int i = 0; str[i] != '\0';)
        {
            char chr = str[i];
            //chr是0xxx xxxx，即ascii码
            if ((chr & 0x80) == 0)
            {
                strChar = str.substr(i, 1);
                ans.push_back(strChar);
                ++i;
            } //chr是1111 1xxx
            else if ((chr & 0xF8) == 0xF8)
            {
                strChar = str.substr(i, 5);
                ans.push_back(strChar);
                i += 5;
            } //chr是1111 xxxx
            else if ((chr & 0xF0) == 0xF0)
            {
                strChar = str.substr(i, 4);
                ans.push_back(strChar);
                i += 4;
            } //chr是111x xxxx
            else if ((chr & 0xE0) == 0xE0)
            {
                strChar = str.substr(i, 3);
                ans.push_back(strChar);
                i += 3;
            } //chr是11xx xxxx
            else if ((chr & 0xC0) == 0xC0)
            {
                strChar = str.substr(i, 2);
                ans.push_back(strChar);
                i += 2;
            }
        }
        return ans;
    }

    void Mydict::writeToCache(const string &file)
    {
        ofstream ofs(file, ofstream::out);
        if (!ofs.good())
        {
            cout << file << "open Cache ERROR!" << endl;
            exit(-1);
        }

        for (auto &e : _dict)
        {
            ofs << e.first << " " << e.second << endl;
        }
        ofs.close();
    }

    void Mydict::readFromCache(const string &file)
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
            _dict.push_back(make_pair(first, second));
        }
        ifs.close();
    }

    int Mydict::editDistance(const string &s1, const string &s2)
    {
        int len1 = s1.size();
        int len2 = s2.size();
        int **d = new int *[len1 + 1];
        for (int i = 0; i <= len1; i++)
            d[i] = new int[len2 + 1];
        int i, j;
        for (i = 0; i <= len1; i++)
            d[i][0] = i;
        for (j = 0; j <= len2; j++)
            d[0][j] = j;
        for (i = 1; i <= len1; i++)
        {
            for (j = 1; j <= len2; j++)
            {
                int cost = s1[i] == s2[j] ? 0 : 1;
                int deletion = d[i - 1][j] + 1;
                int insertion = d[i][j - 1] + 1;
                int substitution = d[i - 1][j - 1] + cost;
                d[i][j] = min(deletion, insertion, substitution);
            }
        }
        // printf("距离为:%d\n", d[len1][len2]);
        int ret = d[len1][len2];
        for (int i = 0; i <= len1; i++)
        {
            delete[] d[i];
        }
        delete[] d;
        return ret;
    }

    int Mydict::min(int a, int b, int c)
    {
        int tmp = a < b ? a : b;
        return tmp < c ? tmp : c;
    }

} // namespace wd