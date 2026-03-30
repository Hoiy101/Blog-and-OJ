#ifndef SINK_HPP
#define SINK_HPP

/*
日志落地模块设计
1.抽象出不同落地子类的基类
2.派生出不同的落地子类
3.使用工厂模式进行创建与表示的分离
*/
//这个模块可以使日志输出到控制台，普通持久化文件和滚动日志文件这三种

#include "util.hpp"
#include <fstream>
#include <sstream>
#include <cassert>
#include <memory>

namespace ns_clog
{
    class LogSink
    {
    public:
        LogSink() 
        {

        }

        ~LogSink()
        {

        }

        using ptr = std::shared_ptr<LogSink>;
        virtual void log(const char *data , int len) = 0;
    };

    //标准输出
    class StdoutSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<StdoutSink>;

        StdoutSink() = default;         //使用默认的构造函数
        void log(const char *data , int len) override
        {
            std::cout.write(data , len);
        }
    };

    //写入普通文件
    class FileSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<FileSink>;
        
        FileSink(const std::string& pathname);
        void log(const char *data , int len) override;

    private:
        std::string PathName;
        std::ofstream ofs;               //用来打开文件的句柄
    };

    //创建滚动文件  
    class RollSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<FileSink>;
        RollSink(const std::string& basename , int max_fsize);
        void log(const char *data , int len) override;

    private:
        //判断文件大小，超过文件大小那么创建新文件
        std::string CreateNewFile();
        
    private:
        // 文件名=基础文件名+以文件大小进行切换的不同扩展文件名

        std::string BaseName;
        std::ofstream ofs;
        size_t MAX_FILE_SIZE;
        size_t Cur_File_Size;
        size_t Name_Count;
    };

    class SinkFactory
    {
    public:
        //SinkType是生产方式类型，Args可以接受任意数据
        template<typename SinkType , typename... Args>
        static LogSink::ptr Create(Args &&...args)
        {
            return std::make_shared<SinkType>(std::forward<Args>(args)...);
        }
    };

};

#endif