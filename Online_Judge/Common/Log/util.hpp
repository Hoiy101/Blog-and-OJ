#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <string>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
    通用类为了实现如下功能
    1:获取系统时间
    2:获取文件所在的目录
    3:获取文件大小
    4:创建目录
*/

namespace ns_clog
{
    namespace Util
    {
        class Data
        {
        public:
            //获取此时的系统时间
            static size_t now();
        };

        class File
        {
        public:
            //检查文件是否存在
            static bool exists(const std::string& path);
            //从文件中获取目录的函数
            static std::string Path(const std::string& PathName);
            static void CreateDirector(const std::string& PathName);
        };
    };
};

#endif