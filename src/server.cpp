#include "./net/TcpServer.h"
#include "./threadpool/Threadpool.h"
#include "./cache/CacheManager.h"
#include "./timerfd/TimerfdThread.h"
#include "Configuration.h"
#include "SplitTool.hpp"
#include "DictProducer.h"
#include "Mydict.h"
#include "MyTask.h"
#include <iostream>
#include <string.h>
#include <string>
#include <map>
#include <regex>
#include <queue>
#include <functional>
using namespace std;
using namespace wd;

Threadpool *pthreadpool = NULL;                                                       //定义全局变量：线程池
Mydict *Mydict::_pInstance = new Mydict();                                            //饿汉模式，线程安全
Configuration *Configuration::_pInstance = new Configuration("../conf/setting.conf"); //饿汉模式，线程安全
CacheManager *CacheManager::_pInstance = new CacheManager();                          //饿汉模式，线程安全

void ConnectionCallBack(const TcpConnectionPtr &ptr)
{
    cout << ">>client has connected " << ptr->Address() << endl;
    ptr->send("welcome to server");
}

void MessageCallBack(const TcpConnectionPtr &ptr)
{
    cout << "From client:" << ptr->Address() << endl;
    string s = ptr->recv();
    cout << s << endl;
    //查询词的任务交给子线程
    MyTask task(s, ptr);
    pthreadpool->addTask(std::bind(&MyTask::execute, task));
}

void CloseCallBack(const TcpConnectionPtr &ptr)
{
    cout << "client has broken up " << ptr->Address() << endl;
}

/*
初始化字典的模式：
线下模式：offline
读取语料库中的内容，进行创建词典，在进行线上部分之前必须先经过线下部分。

线上模式:online
根据客户端的内容进行计算，并返回结果
*/

void offline()
{
    //读取data中的语料库
    Mydict::getpInstance()->initDict(Configuration::getpInstance()->getConfigMap()["dir"], Configuration::getpInstance()->getConfigMap()["ch_dir"], Configuration::getpInstance()->getConfigMap()["cache_dir"]);
}

void online()
{
    //初始化字典
    Mydict::getpInstance()->initDict(Configuration::getpInstance()->getConfigMap()["cache_dir"]);

    //启动线程池，用于处理任务
    Threadpool threadpool(stoi(Configuration::getpInstance()->getConfigMap()["threadNum"]), stoi(Configuration::getpInstance()->getConfigMap()["TaskQueSize"]));
    pthreadpool = &threadpool;
    pthreadpool->start();

    //启动timerfd线程，每隔一定时间触发事件：内存Cache写回磁盘Cache
    TimerfdThread timerThread(stoi(Configuration::getpInstance()->getConfigMap()["cache_initTime"]), stoi(Configuration::getpInstance()->getConfigMap()["cache_intervalTime"]), std::bind(&CacheManager::periodicUpdateCache, CacheManager::getpInstance()));
    timerThread.start();

    //启动服务器
    TcpServer server(Configuration::getpInstance()->getConfigMap()["IP"], stoi(Configuration::getpInstance()->getConfigMap()["port"]));
    server.setConnectionCallBack(ConnectionCallBack);
    server.setMessageCallBack(MessageCallBack);
    server.setCloseCallBack(CloseCallBack);
    server.start();
}

int main()
{
    online();
    return 0;
}