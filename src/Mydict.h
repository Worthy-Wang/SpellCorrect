#ifndef __WD_MYDICT_H_
#define __WD_MYDICT_H_
#include "DictProducer.h"
#include "SplitTool.hpp"
#include "MyTask.h"
#include "./threadpool/MutexLock.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>
using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;

namespace wd
{
    class Mydict
    {
    public:
        static Mydict *getpInstance();
        static void destroy();

        void query(const string &queryWord, priority_queue<MyResult, vector<MyResult>, MyCompare> &priQue);
        void initDict(const string &english_dict_path, const string &chinese_dict_path, const string &cache_dir); //第一次读取语料库并写入磁盘Cache
        void initDict(const string &cache_dir);                                                                   //读取磁盘Cache

    private:
        Mydict() {}
        ~Mydict() {}
        vector<string> divide(const string &);
        int min(int a, int b, int c);
        int editDistance(const string &s1, const string &s2);
        void writeToCache(const string &file);
        void readFromCache(const string &file);

    private:
        static Mydict *_pInstance;
        vector<pair<string, int>> _dict; //<单词，词频>
        map<string, set<int>> _indexMap; //<单个中英文字符，该字符出现在_dict中的下标集合>, 类似nike, 那么分别记录n、i、k、e、的相近单词
        MutexLock _mutexlock;  
    };
} // namespace wd

#endif