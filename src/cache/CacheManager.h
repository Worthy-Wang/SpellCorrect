#ifndef __MY_CACHE_MANAGER_H_
#define __MY_CACHE_MANAGER_H_
#include "Cache.h"
#include <vector>
#include <unordered_map>
#include <string>
using std::string;
using std::unordered_map;
using std::vector;

namespace wd
{
    //CacheManager设置为单例模式
    using CachePtr = Cache *;
    class CacheManager
    {
    public:
        static CacheManager *getpInstance();
        void insertCache(pthread_t, const CachePtr &cache); //向_cacheMap中插入新的cache
        unordered_map<pthread_t, CachePtr> &getCacheMap(); //返回cache列表
        void periodicUpdateCache(); //将所有更新的内存Cache，并写回磁盘Cache中 

    private:
        CacheManager();
        ~CacheManager();

    private:
        unordered_map<pthread_t, CachePtr> _cacheMap; //<线程号，线程对应Cache>, Cache列表，总数等于线程的个数
        static CacheManager *_pInstance;
    };
} // namespace wd

#endif