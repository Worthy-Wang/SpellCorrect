#include "CacheManager.h"
#include "../Mydict.h"
#include "../Configuration.h"
#include <algorithm>
#include <iostream>
using std::cout;
using std::endl;

namespace wd
{

    CacheManager *CacheManager::getpInstance()
    {
        return _pInstance;
    }

    void CacheManager::insertCache(pthread_t pid, const CachePtr &cache)
    {
        _cacheMap[pid] = cache;
    }

    unordered_map<pthread_t, CachePtr> &CacheManager::getCacheMap()
    {
        return _cacheMap;
    }

    void CacheManager::periodicUpdateCache()
    {
        /*
        将所有的内存Cache汇总到一个内存cache中(相同词选择更高频率)，更新所有的内存Cache，再写回磁盘Cache中
        */
        cout << "开始更新Cache" << endl;
        CachePtr finalCache = new Cache(stoul(Configuration::getpInstance()->getConfigMap()["cache_capacity"]));
        for (auto &e : _cacheMap)
        {
            unordered_map<string, list<pair<string, int>>::iterator> tempMap = e.second->getMap();
            for (auto &n : tempMap)
            {
                int ret = finalCache->get(n.first);
                //如果finalCache中没有该单词
                if (-1 == ret)
                {
                    finalCache->put(n.first, n.second->second);
                }
                //finalCache中有该单词
                else
                {
                    int value = std::max(ret, n.second->second);
                    finalCache->put(n.first, value);
                }
            }
        }

        //得到汇总Cache后，首先更新每一个子线程Cache
        for (auto &e : _cacheMap)
        {
            e.second->update(finalCache);
        }

        //再将汇总Cache写入磁盘Cache中
        finalCache->writeToFile(Configuration::getpInstance()->getConfigMap()["cache_dir"]);

        //更新总字典
        Mydict::getpInstance()->initDict(Configuration::getpInstance()->getConfigMap()["cache_dir"]);
        cout << "Cache及字典已更新" << endl;
        delete finalCache;
    }

    CacheManager::CacheManager() {}

    CacheManager::~CacheManager() {}

} // namespace wd