#ifndef TOOL_HPP
#define TOOL_HPP

#include <atomic>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <cppjieba/Jieba.hpp>
#include <boost/algorithm/string.hpp>

#include "../Log/clog.hpp"

/*
    这是我们的工具类，它实现的功能和目的如下
    1:创建相应文件进行编译和暂时储存
    2:提供时间相关的操作用于计算算法消耗的时间
    3:文件的相关操作为编译做准备
    4:字符串的切割用于搜索
*/

//这里要修改一下与日志器有关的操作
inline const char *util_logger_name = "util_logger";

namespace ns_util
{
    //用来储存日志的全局日志器(在main函数中先调用)
    void BuildAsyncLogger(const std::string& logger_name);

    //用来储存临时文件的路径
    const std::string TEMP_PATH = "/home/zz/Desktop/project/Online_Judge/Compile/temp/";
    const std::string DATA_PATH = "/home/zz/Desktop/project/Online_Judge/OJ/Questions/Data/";

    //时间相关的工具类
    class TimeUtil
    {
    public:
        //获取当前时间的时间戳，单位秒
        static std::string GetTimeS()
        {
            struct timeval tv;
            gettimeofday(&tv , nullptr);        //获取当前时间

            
            return std::to_string(tv.tv_sec);
        }

        static std::string GetTimeMS()
        {
            struct timeval tv;
            gettimeofday(&tv , nullptr);        //获取当前时间

            long long ms = tv.tv_sec * 1000 + tv.tv_usec / 1000; //转换为毫秒
            return std::to_string(ms);
        };
    };

    //路径相关的工具类
    class PathUtil
    {
    private:
        //构建文件路径+后缀的完整文件名
        static std::string AddSuffix(const std::string &file_name , const std::string &suffix)
        {
            //把这俩加一块再在前面加一个文件夹
            std::string Complete_File_Name = TEMP_PATH + file_name + suffix;
            return Complete_File_Name;
        }

        //state为0表示该文件应该是输入文件，为1表示该文件应该是输出文件，最后输出文件需要删除，输入文件则不需要删除
        static std::string AddDataPath(const std::string &file_name , const std::string& suffix)
        {
            std::string Data_File_Name = DATA_PATH + file_name + suffix;
            return Data_File_Name;
        }

    public:
        //获取源文件的完整路径
        static std::string Src_Cpp(const std::string &file_name)
        {
            return AddSuffix(file_name , ".cpp");
        }

        //获取可执行文件的完整路径
        static std::string Exe(const std::string &file_name)
        {
            return AddSuffix(file_name , ".exe");
        }

        //编译错误信息文件的完整路径
        static std::string CompileError(const std::string &file_name)
        {
            return AddSuffix(file_name , ".compile_error");
        }

        //构建标准输入文件
        static std::string Stdin(const std::string &file_name)
        {
            return AddDataPath(file_name , ".in");
        }

        //构建标准输出文件
        static std::string Stdout(const std::string &file_name)
        {
            return AddDataPath(file_name , ".out");
        }

        //构建标准错误文件
        static std::string Stderr(const std::string &file_name)
        {
            return AddSuffix(file_name , ".stderr");
        }
    };

    //文件相关的工具类
    class FileUtil
    {
    public:
        //判断文件是否存在
        static bool IsFileExist(const std::string &file_name)
        {
            struct stat st;     //这个结构体用来存文件信息的
            if(stat(file_name.c_str() , &st))    //调用stat函数获取文件信息
            {
                ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(util_logger_name);
                logger->Error("%s is not exist!" , file_name.c_str());
                return false;   //获取失败说明文件不存在
            }
            return true;        //获取成功说明文件存在
        }

        //形成一个文件名，没有后缀和文件夹
        static std::string UniqueFileName()
        {
            static std::atomic<int> id(0);   //静态原子变量，初始值为0
            id++;

            std::string ms = ns_util::TimeUtil::GetTimeMS(); //获取当前时间的毫秒数
            return ms + "_" + std::to_string(id); //用时间戳+原子变量构成文件名
        }

        //将代码写入文件
        static bool WriteFile(const std::string &file_name , const std::string &content)
        {
            std::ofstream ofs(file_name);  //创建输出文件流对象，并打开文件
            if(ofs.is_open() == false)
            {
                ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(util_logger_name);
                logger->Error("%s open error!" , file_name.c_str());
                //如果打开失败
                return false;
            }

            ofs.write(content.c_str() , content.size()); //写入文件
            ofs.close();    //关闭文件
            
            return true;
        }
    
        //读取文件内容
        //file_name:文件名
        //content:用来存储文件内容的字符串指针
        //keep:是否保留文件，默认不保留
        static bool ReadFile(const std::string &file_name , std::string *content , bool keep = false)
        {
            (*content).clear(); //先清空内容

            std::ifstream ifs(file_name , std::ios::binary); //创建输入文件流对象，并打开文件
            if(ifs.is_open() == false)
            {
                ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(util_logger_name);
                logger->Error("%s open error!" , file_name.c_str());

                return false;   //打开失败
            }

            //读取文件内容
            std::string line;
            while(std::getline(ifs , line))
            {
                (*content) += line;
                (*content) += (keep ? "\n" : ""); //是否保留换行符 
            }

            ifs.close();

            return true;
        }
    };

    //字符串相关的工具类
    class StringUtil
    {
    public:
        //切割字符串
        //str:要切割的字符串 ， result:储存的数组 ， sep:分隔符
        static void SplitString(const std::string &str , std::vector<std::string> *result , const std::string sep)
        {
            boost::split((*result), str, boost::is_any_of(sep), boost::algorithm::token_compress_on);
        }
    
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

#endif // TOOL_HPP