#ifndef __ns_CLOG_H__
#define __ns_CLOG_H__
 
#include "Logger.hpp"
 
/*
全局接口的设置
1.提供获取指定日志器的全局接口（避免用户自己创建单例对象）
2.使用宏函数对日志器的接口进行代理（代理模式）
3.提供宏函数，直接通过默认日志器进行标准输出的打印（不需要获取日志器）
*/
 
namespace ns_clog
{
    // 提供获取日志器的全局接口
    //Logger::ptr GetLogger(const std::string &name)
    //{
    //    return ns_clog::LoggerManager::GetInstance().getLogger(name);
    //}
 
    //Logger::ptr GetRootLogger()
    //{
    //    return ns_clog::LoggerManager::GetInstance().getRootLooger();
    //}
 
    // 使用宏函数对日志器的接口进行代理
    //fmt:格式化字符串如:%之类的
    //##__VA_ARGS__:可变参数
    //##​ 是预处理器的粘贴运算符，它用于将宏参数与其他代码片段连接在一起。在这里，它的作用是确保当可变参数为空时，不会产生多余的逗号。
    #define debug(fmt, ...) debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define info(fmt, ...) info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define warn(fmt, ...) warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define Error(fmt, ...) Error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define fatal(fmt, ...) fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
 
    // 使用宏函数，直接通过默认日志器进行标准输出的打印
    #define DEBUG(fmt, ...) ns_clog::LoggerManager::GetInstance().getRootLooger()->debug(fmt, ##__VA_ARGS__)
    #define INFO(fmt, ...) ns_clog::LoggerManager::GetInstance().getRootLooger()->info(fmt, ##__VA_ARGS__)
    #define WARN(fmt, ...) ns_clog::LoggerManager::GetInstance().getRootLooger()->warn(fmt, ##__VA_ARGS__)
    #define ERROR(fmt, ...) ns_clog::LoggerManager::GetInstance().getRootLooger()->Error(fmt, ##__VA_ARGS__)
    #define FATAL(fmt, ...) ns_clog::LoggerManager::GetInstance().getRootLooger()->fatal(fmt, ##__VA_ARGS__)
 
    
}
 
#endif