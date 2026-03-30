#include "Logger.hpp"

ns_clog::Logger::Logger(const std::string& logger_name , LogLevel::level level , Formatter::ptr formatter_ , std::vector<LogSink::ptr>& sinks)
:Logger_Name(logger_name)
,Level(level)
,formatter(formatter_)
,Sinks(sinks.begin() , sinks.end())
{

}

std::string ns_clog::Logger::GetLoggerName()
{
    return Logger_Name;
}

void ns_clog::Logger::debug(const std::string& file , size_t len , const std::string& ftm , ...)
{
    //通过传入的参数来创建一个日志对象，并格式化后落地输出
    //1.判断日志等级->2创建日志对象

    if(LogLevel::level::DEBUG < Level)
    {
        return;
    }

    //创建日志对象(使用va系列的函数来使用可变参数)
    va_list ap;
    va_start(ap , ftm);         //初始化ap变量并且指向ftm第一个字符
    char *str;
    //自动分配内存并且忘str里面写东西
    int ret = vasprintf(&str , ftm.c_str() , ap);
    if(ret == -1)
    {
        std::cout << "vasprintf failed" << std::endl;
        return;
    }
    va_end(ap);     //清理va_list变量
    serialize(LogLevel::level::DEBUG , file , len , str);
    //避免内存泄漏
    free(str);
}

void ns_clog::Logger::info(const std::string& file , size_t len , const std::string& ftm , ...)
{
    //通过传入的参数来创建一个日志对象，并格式化后落地输出
    //1.判断日志等级->2创建日志对象

    if(LogLevel::level::INFO < Level)
    {
        return;
    }

    //创建日志对象(使用va系列的函数来使用可变参数)
    va_list ap;
    va_start(ap , ftm);         //初始化ap变量并且指向ftm第一个字符
    char *str;
    //自动分配内存并且忘str里面写东西
    int ret = vasprintf(&str , ftm.c_str() , ap);
    if(ret == -1)
    {
        std::cout << "vasprintf failed" << std::endl;
        return;
    }
    va_end(ap);     //清理va_list变量
    serialize(LogLevel::level::INFO , file , len , str);
    //避免内存泄漏
    free(str);
}

void ns_clog::Logger::warn(const std::string& file , size_t len , const std::string& ftm , ...)
{
    //通过传入的参数来创建一个日志对象，并格式化后落地输出
    //1.判断日志等级->2创建日志对象

    if(LogLevel::level::WARN < Level)
    {
        return;
    }

    //创建日志对象(使用va系列的函数来使用可变参数)
    va_list ap;
    va_start(ap , ftm);         //初始化ap变量并且指向ftm第一个字符
    char *str;
    //自动分配内存并且忘str里面写东西
    int ret = vasprintf(&str , ftm.c_str() , ap);
    if(ret == -1)
    {
        std::cout << "vasprintf failed" << std::endl;
        return;
    }
    va_end(ap);     //清理va_list变量
    serialize(LogLevel::level::WARN , file , len , str);
    //避免内存泄漏
    free(str);
}

void ns_clog::Logger::Error(const std::string& file , size_t len , const std::string& ftm , ...)
{
    //通过传入的参数来创建一个日志对象，并格式化后落地输出
    //1.判断日志等级->2创建日志对象

    if(LogLevel::level::ERROR < Level)
    {
        return;
    }

    //创建日志对象(使用va系列的函数来使用可变参数)
    va_list ap;
    va_start(ap , ftm);         //初始化ap变量并且指向ftm第一个字符
    char *str;
    //自动分配内存并且忘str里面写东西
    int ret = vasprintf(&str , ftm.c_str() , ap);
    if(ret == -1)
    {
        std::cout << "vasprintf failed" << std::endl;
        return;
    }
    va_end(ap);     //清理va_list变量
    serialize(LogLevel::level::ERROR , file , len , str);
    //避免内存泄漏
    free(str);
}

void ns_clog::Logger::fatal(const std::string& file , size_t len , const std::string& ftm , ...)
{
    //通过传入的参数来创建一个日志对象，并格式化后落地输出
    //1.判断日志等级->2创建日志对象

    if(LogLevel::level::FATAL < Level)
    {
        return;
    }

    //创建日志对象(使用va系列的函数来使用可变参数)
    va_list ap;
    va_start(ap , ftm);         //初始化ap变量并且指向ftm第一个字符
    char *str;
    //自动分配内存并且忘str里面写东西
    int ret = vasprintf(&str , ftm.c_str() , ap);
    if(ret == -1)
    {
        std::cout << "vasprintf failed" << std::endl;
        return;
    }
    va_end(ap);     //清理va_list变量
    serialize(LogLevel::level::FATAL , file , len , str);
    //避免内存泄漏
    free(str);
}

//格式化的函数
void ns_clog::Logger::serialize(LogLevel::level level , const std::string& file , size_t len , const std::string& str)
{
    LogMsg msg(level , len , file , Logger_Name , str);
    //进行格式化
    std::stringstream ss;
    formatter->format(ss , msg);
    //输出
    log(ss.str().c_str() , ss.str().size());
}

ns_clog::SyncLogger::SyncLogger(const std::string& logger_name , LogLevel::level level , Formatter::ptr formatter_ , std::vector<LogSink::ptr>& sinks)
:Logger(logger_name , level , formatter_ , sinks)
{

}

void ns_clog::SyncLogger::log(const char *data , int len)
{
    std::unique_lock<std::mutex>lock(mutex);
    //上锁
    if(Sinks.empty())
        return;
    for(auto& sink : Sinks)
    {
        sink->log(data , len);
    }
}

ns_clog::Logger::ptr ns_clog::LocalLoggerBuilder::Build()
{
    //必须要有名字
    assert(!Logger_Name.empty());
    if(formatter == nullptr)
    {
        formatter = std::make_shared<Formatter>();
    }
    if(Sinks.empty())
    {
        BuildSink<StdoutSink>();
    }
    if(Logger_Type == LoggerType::LOG_ASYNC)
    {
        return std::make_shared<AsyncLogger>(Logger_Name , Level , formatter , Sinks , Looper_Type);
    }

    return std::make_shared<SyncLogger>(Logger_Name , Level , formatter , Sinks);
}

ns_clog::AsyncLogger::AsyncLogger(const std::string& Logger_Name_ , LogLevel::level level , Formatter::ptr formatter , std::vector<LogSink::ptr>&sinks , AsyncType looper_type)
:Logger(Logger_Name_ , level , formatter , sinks)
,looper(std::make_shared<AsyncLooper>(std::bind(&AsyncLogger::reallog, this, std::placeholders::_1), looper_type))
{

}

//输出到缓冲区
void ns_clog::AsyncLogger::log(const char*data , int len)
{
    looper->push(data , len);
}

//对缓冲区的数据处理
void ns_clog::AsyncLogger::reallog(Buffer& buffer)
{

    if(Sinks.empty())
    {
        return;
    }

    for(auto& sink : Sinks)
    {
        sink->log(buffer.Begin() , buffer.ReadAbleSize());
    }
}

ns_clog::LoggerManager::LoggerManager()
{
    //建造root日志器
    std::unique_ptr<LoggerBuilder>lb(new LocalLoggerBuilder());
    lb->BuildLoggerName("root");
    lb->BuildLoggerType(LoggerType::LOG_SYNC);
    root_logger = lb->Build();
    loggers.insert(std::make_pair("root" , root_logger));
}

//以下设计到unmap里面内容的都需要加锁
ns_clog::Logger::ptr ns_clog::LoggerManager::getLogger(const std::string& name)
{
    std::unique_lock<std::mutex>lock(mutex);
    auto it = loggers.find(name);
    if(it == loggers.end())
    {
        //std::cout << "获取了空指针" << std::endl;
        return Logger::ptr();
    }
    return it->second;
}

bool ns_clog::LoggerManager::hashLooper(const std::string& name)
{
    std::unique_lock<std::mutex>lock(mutex);
    auto it = loggers.find(name);
    if(it == loggers.end())
    {
        return false;
    }
    return true;
}

void ns_clog::LoggerManager::addLooper(const std::string& name , const Logger::ptr logger)
{
    std::unique_lock<std::mutex>lock(mutex);
    loggers.insert(std::make_pair(name , logger));
}

ns_clog::Logger::ptr ns_clog::LoggerManager::getRootLooger()
{
    std::unique_lock<std::mutex>lock(mutex);
    return root_logger;
}

ns_clog::Logger::ptr ns_clog::GlobalLoggerBuilder::Build()
{
    assert(!Logger_Name.empty());
    //准备数据
    if(formatter.get() == nullptr)
    {
        formatter = std::make_shared<Formatter>();
    }
    if(Sinks.empty())
    {
        //创建一个对象
        BuildSink<StdoutSink>();
    }

    Logger::ptr logger;
    //创建日志器
    if(Logger_Type == LoggerType::LOG_ASYNC)
    {
        logger = std::make_shared<AsyncLogger>(Logger_Name , Level , formatter , Sinks , Looper_Type);
    }
    else
    {
        logger = std::make_shared<SyncLogger>(Logger_Name , Level , formatter , Sinks);
    }

    //加到map里
    LoggerManager::GetInstance().addLooper(Logger_Name , logger);
    return logger;
}