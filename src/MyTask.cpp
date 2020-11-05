#include "MyTask.h"
#include "Mydict.h"
#include "Configuration.h"
#include "./cache/CacheManager.h"
#include <json/json.h>
#include <algorithm>

namespace wd
{
    MyTask::MyTask(const string &queryWord, const TcpConnectionPtr &ptr)
        : _queryWord(queryWord), _ptr(ptr)
    {
    }

    void MyTask::execute() //执行查询
    {
        CachePtr cache = CacheManager::getpInstance()->getCacheMap()[pthread_self()];

        int frequency = cache->get(_queryWord);
        //cache中没有该查询词
        if (-1 == frequency)
        {
            cache->put(_queryWord, 1);
        }
        //有该查询词;
        else
        {
            //获取该词，并放入优先级队列
            MyResult res(_queryWord, frequency, 0);            
            //在put函数中更换为频率 + 1;
            cache->put(_queryWord, frequency+1);
        }



        //在大字典dict中查找, 并将结果放入优先级队列中
        Mydict::getpInstance()->query(_queryWord, _priQue);


        //持续出队，直到达到指定容量大小
        while (_priQue.size() > stoul(Configuration::getpInstance()->getConfigMap()["priQue_size"]))
        {
            _priQue.pop();
        }

        //将信息组合发送给客户端
        string msg = jsonPackage();
        _ptr->sendInEventLoop(msg);

        // //测试打印
        // cout << "-----------------------------" << endl;
        // while (!_priQue.empty())
        // {
        //     cout << _priQue.top()._word << ":" << _priQue.top()._frequency << ":" << _priQue.top()._distance << endl;
        //     _priQue.pop();
        // }
    }

    // 将查找到的单词数据用json格式发送给客户端
    string MyTask::jsonPackage()
    {
        // 构建json数组
        Json::Value array;
        Json::Value root;
        Json::Value wordRes;
        vector<MyResult> vec;
        while (!_priQue.empty())
        {
            MyResult res = _priQue.top();
            vec.push_back(res);
            _priQue.pop();
        }
        for (auto it = vec.rbegin(); it != vec.rend(); it++)
        {
            MyResult res = *it;
            wordRes["word"] = res._word;
            wordRes["frequency"] = res._frequency;
            wordRes["min_distance"] = res._distance;
            root.append(wordRes);
        }
        array["array"] = Json::Value(root);
        return array.toStyledString();
    }


} // namespace wd