#ifndef __MYTASK_H_
#define __MYTASK_H_
#include "./net/TcpConnection.h"
#include <iostream>
#include <string>
#include <queue>
#include <vector>
using std::priority_queue;
using std::string;
using std::vector;

namespace wd
{
    struct MyResult //查找的单词
    {
        string _word;   //单词
        int _frequency; //词频
        int _distance;  //最小编辑距离
        MyResult(string word, int frequency, int distance) : _word(word), _frequency(frequency), _distance(distance) {}
    };

    //先比较最小编辑距离，若相同则比较词频
    struct MyCompare
    {
        bool operator()(const MyResult &lhs, const MyResult &rhs)
        {
            if (lhs._distance > rhs._distance)
            {
                return false;
            }
            else if (lhs._distance < rhs._distance)
            {
                return true;
            }
            else
            {
                return (lhs._frequency > rhs._frequency);
            }
        }
    };

    class MyTask
    {
    public:
        MyTask(const string &queryWord, const TcpConnectionPtr &ptr);
        void execute(); // 执行查询

    private:
        string jsonPackage();                             // 将优先级队列查找到的单词数据用json格式发送给客户端

    private:
        string _queryWord;                                             //客户端查询的单词
        TcpConnectionPtr _ptr;                                         //对端TCP联络类
        priority_queue<MyResult, vector<MyResult>, MyCompare> _priQue; //查询返回的优先级队列
    };

} // namespace wd

#endif