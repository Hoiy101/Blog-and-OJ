/*#include <unistd.h>
#include "clog.hpp"
 
    /*
    日志落地模块扩展：以时间为间隔进行文件的滚动的日志落地模块
    实际上以时间为间隔进行文件滚动，就是以时间段进行滚动
    实现思想：
    1.用当前系统时间取模规定好的时间段大小，判断当前系统时间处于哪个时间段
    2.判断与当前文件所在的时间段是否一致，不一致进行文件切换
    3.文件命名规则：basename+时间信息+.log
    4.时间信息格式：年月日时分秒连续数字表示
 
enum timeGap
{
    GAP_SECOND,
    GAP_MINUTE,
    GAP_HOUR,
    GAP_DAY
};
 
class RollByTimeSink : public clog::LogSink
{
public:
    using ptr = std::shared_ptr<RollByTimeSink>;
    // 传入文件名，打开文件，将操作句柄管理起来
    RollByTimeSink(const std::string &basename, timeGap gaptype)
        : _basename(basename)
    {
        switch (gaptype)
        {
        case GAP_SECOND:
            _gap_size = 1;
        case GAP_MINUTE:
            _gap_size = 60;
        case GAP_HOUR:
            _gap_size = 3600;
        case GAP_DAY:
            _gap_size = 3600 * 24;
        }
        std::string filename = creatNewFile();
        // 当时间段大小为1时要注意取模的结果
        _cur_gap = _gap_size == 1 ? clog::Util::Data::now() : (clog::Util::Data::now() % _gap_size); // 获得当前系统所在的时间段
        // 创建目录
        clog::Util::File::CreateDirector(clog::Util::File::Path(filename));
        // 创建文件
        _ofs.open(filename, std::ios::binary | std::ios::app);
        assert(_ofs.is_open());
    }
 
    void log(const char *data, int len) override
    {
        time_t cur_time = clog::Util::Data::now();
        if ((cur_time % _gap_size) != _cur_gap)
        {
            _ofs.close();
            // 创建新文件
            _ofs.open(creatNewFile(), std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }
        _ofs.write(data, len);
        if (_ofs.good() == false)
        {
            std::cout << "日志文件输出失败！\n";
        }
    }
 
private:
    std::string creatNewFile()
    {
        // 获取系统时间
        time_t t = clog::Util::Data::now();
        struct tm tl;
        localtime_r(&t, &tl);
        std::stringstream ss;
        ss << _basename;
        ss << tl.tm_year + 1900;
        ss << tl.tm_mon + 1;
        ss << tl.tm_mday;
        ss << tl.tm_hour;
        ss << tl.tm_min;
        ss << tl.tm_sec;
        ss << ".log";
        return ss.str();
    }
    std::string _basename;
    std::ofstream _ofs; // 为了避免每次对文件写入都要打开文件，对操作句柄进行管理
    size_t _cur_gap;    // 当前系统所在的时间段
    size_t _gap_size;   // 时间段大小
};
 
int main()
{
    const char *logger_name = "async_logger";
    std::unique_ptr<clog::LoggerBuilder> bd(new clog::GlobalLoggerBuilder());
    bd->BuildLoggerName(logger_name);
    bd->BuildFormatter("[%c][%p][%f:%l][%m]%n");
    bd->BuildLoggerLevel(clog::LogLevel::level::WARN);
    bd->BuildLoggerType(clog::LoggerType::LOG_ASYNC);
    bd->BuildSink<clog::StdoutSink>();
    bd->BuildSink<clog::FileSink>("./logfile/async.log");
    bd->BuildSink<RollByTimeSink>("./logfile/roll-by-time-async.log-", timeGap::GAP_SECOND);
    clog::Logger::ptr logger = bd->Build();
    size_t cur = clog::Util::Data::now();
    while (clog::Util::Data::now() < cur + 5)
    {
        logger->fatal("这是一条日志消息");
        usleep(1000);
    }
 
    return 0;
}
*/
/*
扩展
#include <unistd.h>
#include "clog.hpp"
 

日志落地模块扩展：以时间为间隔进行文件的滚动的日志落地模块
实际上以时间为间隔进行文件滚动，就是以时间段进行滚动
实现思想：
1.用当前系统时间取模规定好的时间段大小，判断当前系统时间处于哪个时间段
2.判断与当前文件所在的时间段是否一致，不一致进行文件切换

以时间为间隔进行文件的滚动的日志落地模块，实际上以时间为间隔进行文件滚动，就是以时间段进行滚动

 
enum timeGap
{
    GAP_SECOND,
    GAP_MINUTE,
    GAP_HOUR,
    GAP_DAY
};
 
class RollByTimeSink : public clog::LogSink
{
public:
    using ptr = std::shared_ptr<RollByTimeSink>;
    // 传入文件名，打开文件，将操作句柄管理起来
    RollByTimeSink(const std::string &basename, timeGap gaptype)
        : _basename(basename)
    {
        switch (gaptype)
        {
        case GAP_SECOND:
            _gap_size = 1;
        case GAP_MINUTE:
            _gap_size = 60;
        case GAP_HOUR:
            _gap_size = 3600;
        case GAP_DAY:
            _gap_size = 3600 * 24;
        }
        std::string filename = creatNewFile();
        // 当时间段大小为1时要注意取模的结果
        _cur_gap = _gap_size == 1 ? clog::Util::Data::now() : (clog::Util::Data::now() % _gap_size); // 获得当前系统所在的时间段
        // 创建目录
        clog::Util::File::CreateDirector(clog::Util::File::Path(filename));
        // 创建文件
        _ofs.open(filename, std::ios::binary | std::ios::app);
        assert(_ofs.is_open());
    }
 
    void log(const char *data, int len) override
    {
        time_t cur_time = clog::Util::Data::now();
        if ((cur_time % _gap_size) != _cur_gap)
        {
            _ofs.close();
            // 创建新文件
            _ofs.open(creatNewFile(), std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }
        _ofs.write(data, len);
        if (_ofs.good() == false)
        {
            std::cout << "日志文件输出失败！\n";
        }
    }
 
private:
    std::string creatNewFile()
    {
        // 获取系统时间
        time_t t = clog::Util::Data::now();
        struct tm tl;
        localtime_r(&t, &tl);
        std::stringstream ss;
        ss << _basename;
        ss << tl.tm_year + 1900;
        ss << tl.tm_mon + 1;
        ss << tl.tm_mday;
        ss << tl.tm_hour;
        ss << tl.tm_min;
        ss << tl.tm_sec;
        ss << ".log";
        return ss.str();
    }
    std::string _basename;
    std::ofstream _ofs; // 为了避免每次对文件写入都要打开文件，对操作句柄进行管理
    size_t _cur_gap;    // 当前系统所在的时间段
    size_t _gap_size;   // 时间段大小
};
 
int main()
{
    const char *logger_name = "async_logger";
    std::unique_ptr<clog::LoggerBuilder> bd(new clog::GlobalLoggerBuilder());
    bd->BuildLoggerName(logger_name);
    bd->BuildFormatter("[%c][%p][%f:%l][%m]%n");
    bd->BuildLoggerLevel(clog::LogLevel::level::WARN);
    bd->BuildLoggerType(clog::LoggerType::LOG_ASYNC);
    bd->BuildSink<clog::StdoutSink>();
    bd->BuildSink<clog::FileSink>("./logfile/async.log");
    bd->BuildSink<RollByTimeSink>("./logfile/roll-by-time-async.log-", timeGap::GAP_SECOND);
    clog::Logger::ptr logger = bd->Build();
    size_t cur = clog::Util::Data::now();
    while (clog::Util::Data::now() < cur + 5)
    {
        logger->fatal("这是一条日志消息");
        usleep(1000);
    }
 
    return 0;
}*/

//测试
/*
#include "clog.hpp"
#include <vector>
#include <thread>
#include <chrono>
 
void bench(const std::string &logger_name,size_t thr_count,size_t msg_count,size_t msg_len)
{
    // 获取指定名称的日志器
    clog::Logger::ptr logger = clog::GetLogger(logger_name);
    if (logger.get() == nullptr)
    {
        return;
    }
    std::cout << "测试日志 :  " << msg_count << "条, 总大小 : " << msg_count * msg_len / 1024 << "KB\n";
    // 组织指定长度的日志消息
    std::string msg(msg_len - 1, 'A'); // 留一个位置方便换行显示
    // 创建指定数量的线程
    std::vector<std::thread> threads;
    // 记录每个线程所花费的时间
    std::vector<double> cost_arry(thr_count);
    // 每个线程打印的日志条数
    size_t msg_per_thr = msg_count / thr_count;
    for (int i = 0; i < thr_count; i++)
    {
        //i传值传参保证每个线程各有一份i
        threads.emplace_back([&,i](){
            //线程函数内部开始计时
            auto start = std::chrono::high_resolution_clock::now();
            //开始循环写日志
            for(int j = 0; j < msg_per_thr; j++)
            {
                logger->fatal("%s",msg.c_str());
            }
            //线程函数内部结束计时
            auto end = std::chrono::high_resolution_clock::now();
            auto cost = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
            //记录每个线程执行时间
            cost_arry[i]=cost.count();
            std::cout << "\t线程" << i << " : " << "\t输出数量 : " << msg_per_thr << ", 耗时 : " <<cost.count() << "s" << std::endl;
        });
    }
    // 回收线程
    for (int i = 0; i < thr_count; i++)
    {
        threads[i].join();
    }
    // 计算总耗时：因为在多线程中，是并发执行的，因此总耗时就是耗费最高的时间的那个线程所消耗的时间
    double max_cost = cost_arry[0];
    for (int i = 0; i < thr_count; i++)
    {
        max_cost = max_cost < cost_arry[i] ? cost_arry[i] : max_cost;
    }
    // 每秒输出的条数
    size_t msg_per_sec = msg_count / max_cost;
    // 每秒输出的大小
    size_t size_per_sec = msg_count * msg_len / (max_cost * 1024);
    // 输出结果
    std::cout << "\t总耗时 : " << max_cost << "s\n";
    std::cout << "\t每秒输出日志数量 : " << msg_per_sec << "条\n";
    std::cout << "\t每秒输出日志大小 : " << size_per_sec << "KB\n";
}
 
void bench_sync()
{
    const char *logger_name = "sync_logger";
    // std::unique_ptr<clog::LoggerBuilder> bd(new clog::GlobalLoggerBuilder());
    // bd->buildloggername(logger_name);
    // bd->buildformatter("[%m]%n");
    // bd->buildloggertype(clog::LoggerType::LOG_SYNC);
    // bd->buildsink<clog::FileSink>("./logfile/sync.log");
    // bd->build();
 
    // bench(logger_name,1,1000000,100);
    // //bench(logger_name,3,1000000,100);
 
    clog::GlobalLoggerBuilder::ptr lbp(new clog::GlobalLoggerBuilder);
    lbp->BuildLoggerName(logger_name);
    lbp->BuildFormatter("%m%n");
    lbp->BuildSink<clog::FileSink>("./logs/sync.log");
    lbp->BuildLoggerType(clog::LoggerType::LOG_SYNC);
    lbp->Build();
    //bench(logger_name, 1, 1000000, 100);
    bench(logger_name, 5, 1000000, 100);
 
}
 
 
 
void bench_async()
{
    const char *logger_name = "async_logger";
    std::unique_ptr<clog::LoggerBuilder> bd(new clog::GlobalLoggerBuilder());
    bd->BuildLoggerName(logger_name);
    bd->BuildFormatter("%m%n");
    bd->BuildEnableUnsaveAsync();  //启动非安全模式，忽略实际落地的时间
    bd->BuildLoggerType(clog::LoggerType::LOG_ASYNC);
    bd->BuildSink<clog::FileSink>("./logfile/async.log");
    bd->Build();
 
    bench(logger_name,1,1000000,100);
    //bench(logger_name,5,1000000,100);
 
}
 
int main()
{
    //bench_sync();
    bench_async();
    return 0;
}*/


#include <unistd.h>
#include "clog.hpp"
 
void test_log(const std::string &name)
{
    INFO("%s", "测试开始");
 
    std::string test = "test start";
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(name);
    logger->debug( "%s", "测试日志");
    logger->info("%s", "测试日志");
    logger->warn( "%s%s", "测试日志" , test.c_str());
    logger->Error("测试日志");
    logger->fatal("测试日志 %s" , test.c_str());
    
    INFO("%s", "测试结束");
}
 
int main()
{
    const char *logger_name = "async_logger";               //日志器名称
    std::unique_ptr<ns_clog::LoggerBuilder> bd(new ns_clog::GlobalLoggerBuilder());       //创建日志器构造器类型
    bd->BuildLoggerName(logger_name);                       //设置日志器名称
    bd->BuildFormatter("[%c][%p][%f:%l][%m]%n");            //设置日志格式
    bd->BuildLoggerLevel(ns_clog::LogLevel::level::DEBUG);  //设置日志器日志级别
    bd->BuildLoggerType(ns_clog::LoggerType::LOG_ASYNC);    //设置日志器类型
    bd->BuildSink<ns_clog::StdoutSink>();                   //添加日志落地模块-控制台
    bd->BuildSink<ns_clog::FileSink>("/home/zz/Desktop/project/Online_Judge/Common/Log/logfile/async.log");//添加日志落地模块-文件
    bd->BuildSink<ns_clog::RollSink>("/home/zz/Desktop/project/Online_Judge/LogFile/roll-by-size-async.log-", 1024 * 1024); //添加日志落地模块-文件滚动
    bd->Build();                                            //构建日志器                           
    test_log(logger_name);
 
    return 0;
}
