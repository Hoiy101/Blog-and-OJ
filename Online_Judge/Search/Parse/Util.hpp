#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include <cppjieba/Jieba.hpp>

//把这个移动到Common/Util.hpp

namespace util
{  
    class FileUtil
    {
        public:
            static bool ReadFile(const std::string& File_Path , std::string* out);
    };

    class StringUtil
    {
        public:
            static void Split(const std::string& target , std::vector<std::string>* out , const std::string& sep);
    };

    class JiebaUtil
    {
        private:
            static cppjieba::Jieba jieba;           //定义静态变量成员
        public:
            //分词函数
            static void CutString(const std::string &src , std::vector<std::string>* output);
    };

};




#endif // UTIL_HPP