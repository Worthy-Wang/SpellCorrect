#ifndef _WD_DICTPRODUCER_H_
#define _WD_DICTPRODUCER_H_
#include <string>
#include <unordered_map>
#include <set>
using std::set;
using std::string;
using std::unordered_map;

namespace wd
{
    class SplitTool;
    class DictProducer
    {
    public:
        DictProducer(const string &english_dir, const string &chinese_dir, SplitTool *splitTool); //词典构造函数（中英文）
        unordered_map<string,int>& getDict(); //获取词典
        void show_dict();                                                                         //测试函数

    private:
        void Convert(string &);                        // 将单词中的大写字母转为小写字母
        void build_English_dict(const string &file); //创建英文字典
        void build_Chinese_dict(const string &file);   //创建中文字典
    private:
        string _english_dir;              //英文语料库文件存放的相对路径
        string _chinese_dir;              //中文语料库文件存放的相对路径
        unordered_map<string, int> _dict; //<单词，出现的次数>
        SplitTool *_splitTool;            //中文分词工具
    };

} //end of namespace wd

#endif