#ifndef FORMAT_HPP
#define FORMAT_HPP

#include "Message.hpp"
#include "Level.hpp"
#include "util.hpp"
#include <vector>
#include <cassert>
#include <memory>
#include <sstream>

namespace ns_clog
{
    //采用策略模式输出日志的各种内容
    class FormatItem
    {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual void format(std::ostream& out , const LogMsg& msg) = 0;
        //我们使用ostream可以保证输出到控制台，文件和网络
    };

    class MesFormatItem : public FormatItem
    {
    public:
        void format(std::ostream& out , const LogMsg& msg) override
        {
            out << msg.PayLoad;
        }
    };

    class TimeFormatItem : public FormatItem
    {
    public:
        TimeFormatItem(const std::string& format_ = "%H:%M:%S")
        :Format(format_)
        {
           if(Format.empty())
           {
                Format = "%H:%M:%S";
           } 
        }

        void format(std::ostream& out , const LogMsg& msg) override
        {
            struct tm t;
            localtime_r(&msg.time , &t);
            char time[128];

            //拼凑字符串
            strftime(time , 127 , Format.c_str() , &t);
            out << time;
        }

    private:
        std::string Format;
    };

    class LevelFormatItem : public FormatItem
    {
    public:
        void format(std::ostream& out , const LogMsg& msg) override
        {
            out << LogLevel::ToString(msg.Level);
        }
    };

    class ThreadFormatItem : public FormatItem
    {
    public:
        void format(std::ostream& out , const LogMsg& msg) override
        {
            out << msg.tid;
        }
    };

    class LineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream& out , const LogMsg& msg) override
        {
            out << msg.line;
        }
    };

    class FileFormatItem : public FormatItem
    {
    public:
        void format(std::ostream& out , const LogMsg& msg) override
        {
            out << msg.File;
        }
    };

    class LoggerFormatItem : public FormatItem
    {
    public:
        void format(std::ostream& out , const LogMsg& msg) override
        {
            out << msg.Logger;
        }
    };

    class NlineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream& out , const LogMsg& msg) override
        {
            out << "\n";
        }
    };

    class TabFormatItem : public FormatItem
    {
    public:
        void format(std::ostream& out , const LogMsg& msg) override
        {
            out << "\t";
        }
    };

    class OtherFormatItem : public FormatItem
    {
    public:
        OtherFormatItem(const std::string& str = "")
        :test(str)
        {

        }

        void format(std::ostream& out , const LogMsg& msg) override
        {
            out << test;
        }
    private:

        std::string test;
    };

    //对已格式化的元素取出来进行分割
    class Formatter
    {
    public:
        using ptr = std::shared_ptr<Formatter>;
        Formatter(const std::string &pattern_ = "[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n");
        /*
            %d{%H:%M:%S}​ - 时间戳，格式为 小时:分钟:秒
            %t​ - 线程ID
            %c​ - 日志器名称
            %f​ - 源文件名
            %l​ - 代码行号
            %p​ - 日志级别（DEBUG/INFO/WARN/ERROR/FATAL）
            %T​ - 制表符（Tab）
            %m​ - 实际的日志消息内容
            %n​ - 换行符
        */

        //两个格式化函数用于对msg进行格式化
        //输出流可以用于输出控制台，string可以用于写入文件
        std::ostream& format(std::ostream& out , const LogMsg& msg);
        std::string format(const LogMsg& msg);

    private:

        bool parsePattern();                //解析格式话字符串
        FormatItem::ptr CreateItem(const std::string& key , const std::string value);       //创建格式化项

    private:
        std::string Pattern;                //储存格式化内容的字符串
        std::vector<FormatItem::ptr>Items;  //格式化项的列表
    };

};



#endif