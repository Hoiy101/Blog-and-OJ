#ifndef LEVEL_HPP
#define LEVEL_HPP

/*
日志等级类的设计
UNKNOW 0
DEBUG 进⾏debug时候打印⽇志的等级
INFO 打印⼀些⽤⼾提⽰信息
WARN 打印警告信息
ERROR 打印错误信息
FATAL 打印致命信息- 导致程序崩溃的信息
OFF 关闭所有⽇志输出
*/

#include <iostream>

namespace ns_clog
{
    class LogLevel
    {
    public:
        enum class level
        {
            UNKNOW = 0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            OFF
        };

        //转化为字符串类型
        static const char* ToString(LogLevel::level temp)
        {
            switch (temp)
            {
            case LogLevel::level::DEBUG:
                {
                    return "DEBUG";
                    break;
                }
            case LogLevel::level::ERROR:
                {
                    return "ERROR";
                    break;
                }
            case LogLevel::level::FATAL:
                {
                    return "FATAL";
                    break;
                }
            case LogLevel::level::INFO:
                {
                    return "INFO";
                    break;
                }
            case LogLevel::level::OFF:
                {
                    return "OFF";
                    break;
                }
            case LogLevel::level::UNKNOW:
                {
                    return "UNKNOW";
                    break;
                }
            case LogLevel::level::WARN:
                {
                    return "WARN";
                    break;
                }
            default:
                return "UNKNOW";
            }
            return "UNKNOW";
        }
    };
};

#endif