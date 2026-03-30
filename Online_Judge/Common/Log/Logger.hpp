#ifndef LOGGER_HPP
#define LOGGER_HPP

/*
    我们对于不同业务日志的需求是复杂的，所以我们抽象出一个日志器来帮助我们生产符合·1我们需求的日志
    基于策略模式(Strategy Pattern)和工厂模式(Factory Pattern)
    我们设计了可配置的日志器架构，通过组合不同的格式化策略、落地策略和同步策略，实现了面向不同业务场景的定制化日志解决方案
*/

#include "Sink.hpp"
#include "util.hpp"
#include "Looper.hpp"
#include "Format.hpp"
#include "Message.hpp"
#include <mutex>
#include <atomic>
#include <cstdarg>
#include <cassert>
#include <functional>
#include <unordered_map>

namespace ns_clog
{
    enum class LoggerType
    {
        LOG_SYNC = 0,
        LOG_ASYNC
    };

    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;
        Logger(const std::string& logger_name , LogLevel::level level , Formatter::ptr formatter , std::vector<LogSink::ptr>& sinks);
        
        std::string GetLoggerName();

        // 构造出日志消息对象，并进行格式化，格式化成消息字符串后再落地输出
        void debug(const std::string& file , size_t len , const std::string& ftm , ...);         //...表示可以接受可变参数
        void info(const std::string& file , size_t len , const std::string& ftm , ...);
        void warn(const std::string& file , size_t len , const std::string& ftm , ...);
        void Error(const std::string& file , size_t len , const std::string& ftm , ...);
        void fatal(const std::string& file , size_t len , const std::string& ftm , ...);

    protected:
        void serialize(LogLevel::level level , const std::string& file , size_t len , const std::string& str);
        //根据日志器类型不同来派生不同子类
        virtual void log(const char *data , int len) = 0;

    protected:
        std::mutex mutex;
        std::string Logger_Name;
        std::atomic<LogLevel::level>Level;
        Formatter::ptr formatter;
        std::vector<LogSink::ptr>Sinks;
    };

    
    // 同步日志器  将格式化后的日志消息直接通过日志落地模块句柄进行落地
    class SyncLogger : public Logger
    {
    public:
        SyncLogger(const std::string& logger_name , LogLevel::level level , Formatter::ptr formatter , std::vector<LogSink::ptr>& sinks);
    
    protected:
        void log(const char *data , int len) override;    
    };

    // 异步日志器--业务线程将数据写入缓冲区后，异步工作线程从缓冲区中拿取数据写入磁盘
    class AsyncLogger : public Logger
    {
    public:
        AsyncLogger(const std::string& Logger_Name , LogLevel::level level , Formatter::ptr formatter , std::vector<LogSink::ptr>&sinks , AsyncType looper_type);
    protected:
        void log(const char *data , int len) override;
        //对缓冲区拿到的数据进行处理
        void reallog(Buffer& buffer);

    private:
        AsyncLooper::ptr looper;
    };

    /*
        1.抽象出一个日志器建造者基类
        1.设置日志器类型（同步&&异步）
        2.实现各个接口，完成一个日志器各个零部件的构建
        3.将不同类型的日志器放在同一个建造者类中实现
    */

    //我们之前那个日志器构造过于复杂，此时我们用一个专门建造日志器的工厂来建造它可以简化不步骤
    class LoggerBuilder
    {
    public:
        LoggerBuilder()
        :Logger_Type(LoggerType::LOG_SYNC)
        ,Level(LogLevel::level::DEBUG)
        ,Looper_Type(AsyncType::ASYNC_SAVE)
        {

        }

        void BuildEnableUnsaveAsync() { Looper_Type = AsyncType::ASYNC_UNSAVE; }
        void BuildLoggerType(LoggerType Type) { Logger_Type = Type; }
        void BuildLoggerName(const std::string &name) { Logger_Name = name; }
        void BuildLoggerLevel(LogLevel::level level) { Level = level; }
        void BuildFormatter(const std::string& Pattern) { formatter = std::make_shared<Formatter>(Pattern); }
        
        template<typename SinkType , typename... Args>
        void BuildSink(Args &&...args)
        {
            LogSink::ptr Ptr_Sink = std::make_shared<SinkType>(std::forward<Args>(args)...);
            Sinks.push_back(Ptr_Sink);
        }

        virtual Logger::ptr Build() = 0;

    protected:
        LoggerType Logger_Type;    
        std::string Logger_Name;
        LogLevel::level Level;
        Formatter::ptr formatter;
        std::vector<LogSink::ptr> Sinks;
        AsyncType Looper_Type;
    };

    //具体建造者类
    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr Build() override;
    };

    //全局日志器建造者--将日志器添加到单例对象中去
    class GlobalLoggerBuilder : public LoggerBuilder
    {
    public:
        using ptr = std::shared_ptr<GlobalLoggerBuilder>;
        Logger::ptr Build() override;
    };

    //使用日志管理器来管理日志对象
    class LoggerManager
    {
    private:
        LoggerManager();
    public:
        //使用单例模式
        LoggerManager(LoggerManager& instance) = delete;
        const LoggerManager &operator=(const LoggerManager& instance) = delete;
        static LoggerManager& GetInstance()
        {
            static LoggerManager instance;
            return instance;
        }

        //获取日志器
        Logger::ptr getLogger(const std::string& name);
        //检查是否有这个日志器
        bool hashLooper(const std::string& name);
        void addLooper(const std::string& name , const Logger::ptr logger);
        Logger::ptr getRootLooger();
    private:
        std::mutex mutex;
        //默认日志器
        Logger::ptr root_logger;
        std::unordered_map<std::string , Logger::ptr>loggers;       //用来储存日志器
    };

};

#endif