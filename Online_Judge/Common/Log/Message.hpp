#ifndef MESSAGE_HPP
#define MESSAGE_HPP

//这个是封装日志类的hpp文件这里我们要储存的内容为
/*
1.日志时间戳信息  用来过滤日志输出时间
2.日志等级        用来进行日志过滤分析
3.源文件文件名
4.源文件的行号    用来定位出现错误的代码位置
5.线程ID         用来过滤出错的线程
*/

#include "Level.hpp"
#include "util.hpp"
#include <string>
#include <thread>

namespace ns_clog
{
    struct LogMsg
    {
        time_t time;
        LogLevel::level Level;
        size_t line;
        std::string File;       //文件名
        std::thread::id tid;    //线程ID
        std::string Logger;     //日志器
        std::string PayLoad;    //主体信息

        LogMsg(LogLevel::level Level_ , size_t line_ , const std::string file_ , const std::string Logger_ , const std::string PayLoad_)
        :time(ns_clog::Util::Data::now())
        ,Level(Level_)
        ,line(line_)
        ,File(file_)
        ,tid(std::this_thread::get_id())
        ,Logger(Logger_)
        ,PayLoad(PayLoad_)
        {
        }
    };
};

#endif