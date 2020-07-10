#include "DictProducer.h"
#include "SplitTool.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <string.h>
using std::cout;
using std::endl;
using std::ifstream;
using std::make_pair;
using std::pair;
using std::regex;
using std::regex_replace;
using std::stringstream;
using std::vector;

namespace wd
{
    DictProducer::DictProducer(const string &english_dir, const string &chinese_dir, SplitTool *splitTool) //词典构造函数（中英文）
        : _english_dir(english_dir), _chinese_dir(chinese_dir), _splitTool(splitTool)
    {
        build_English_dict(_english_dir);
        build_Chinese_dict(_chinese_dir);
    }

    unordered_map<string, int> &DictProducer::getDict()
    {
        return _dict;
    }

    void DictProducer::show_dict()
    {
        for (auto &e : _dict)
        {
            cout << e.first << ":" << e.second << endl;
        }
        cout << "total Number:" << _dict.size() << endl;
    }

    void DictProducer::Convert(string &word)
    {
        for (auto &e : word)
        {
            if (e >= 'A' && e <= 'Z')
            {
                e += 32;
            }
        }
    }

    void DictProducer::build_English_dict(const string &file)
    {
        ifstream ifs(file, ifstream::in);
        if (!ifs.good())
        {
            cout << file << "open ERROR!" << endl;
            exit(-1);
        }

        string lines;
        string word;
        while (getline(ifs, lines))
        {
            lines = regex_replace(lines, regex("[^a-zA-Z0-9]"), " "); //正则表达式，将标点符号转换为空格
            stringstream ss(lines);
            while (ss >> word)
            {
                Convert(word); // 将大写字母转为小写字母
                _dict[word]++;
            }
        }
        ifs.close();
    }

    void DictProducer::build_Chinese_dict(const string &file)
    {
        ifstream ifs(file, ifstream::in);
        if (!ifs.good())
        {
            cout << file << "open ERROR!" << endl;
            exit(-1);
        }

        string lines;
        while (getline(ifs, lines))
        {
            vector<string> ans = _splitTool->cut(lines);
            for (auto &e : ans)
            {
                _dict[e]++;
            }
        }

        ifs.close();
    }

} // namespace wd