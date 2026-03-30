#include "Tool.hpp"

void ns_util::BuildAsyncLogger(const std::string& logger_name)
{
    std::unique_ptr<ns_clog::LoggerBuilder> util_logger_bd(new ns_clog::GlobalLoggerBuilder());
    util_logger_bd->BuildLoggerName(logger_name.c_str());
    util_logger_bd->BuildFormatter("[%d{%H:%M:%S}][%c][%p][%f:%l][%m]%n");
    util_logger_bd->BuildLoggerLevel(ns_clog::LogLevel::level::DEBUG);
    util_logger_bd->BuildLoggerType(ns_clog::LoggerType::LOG_ASYNC);
    std::string file_path = "/home/zz/Desktop/project/Online_Judge/LogFile/";
    file_path = file_path + logger_name.c_str() + ".log";
    util_logger_bd->BuildSink<ns_clog::FileSink>(file_path);
    util_logger_bd->BuildSink<ns_clog::StdoutSink>();
    util_logger_bd->Build();
}