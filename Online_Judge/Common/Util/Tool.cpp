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

namespace ns_util
{
    //声明断词的中文词典路径
    const char* const DICT_PATH = "/home/zz/Desktop/library/cppjieba/dict/jieba.dict.utf8";
    const char* const HMM_PATH = "/home/zz/Desktop/library/cppjieba/dict/hmm_model.utf8";    
    const char* const USER_DICT_PATH = "/home/zz/Desktop/library/cppjieba/dict/user.dict.utf8";    
    const char* const IDF_PATH = "/home/zz/Desktop/library/cppjieba/dict/idf.utf8";    
    const char* const STOP_WORD_PATH = "/home/zz/Desktop/library/cppjieba/dict/stop_words.utf8";

    //类外初始化，就是将上面的路径传进去，具体和它的构造函数是相关的，具体可以去看一下源代码
    cppjieba::Jieba JiebaUtil::jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
}

void ns_util::JiebaUtil::CutString(const std::string &src , std::vector<std::string>* output)
{
    //调用CutForSearch函数，第一个参数就是你要对谁进行分词，第二个参数就是分词后的结果存放到哪里
    jieba.CutForSearch(src , *output);
}