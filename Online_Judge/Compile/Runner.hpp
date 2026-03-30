#ifndef RUNNER_HPP
#define RUNNER_HPP

#include <vector>
#include <string>
#include <future>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "../Common/Log/clog.hpp"
#include "../Common/Util/Tool.hpp"
#include "../Common/Pool/ThreadPool.hpp"

namespace ns_runner
{
    inline const char * running_logger_name = "running_logger";
    
    class Runner
    {
    public:
        Runner();
        ~Runner();
    public:
        //run需要改为静态函数，然后设置进程池

        //设置客户代码占用资源的大小程度
        void SetProcLimit(int cpu_limit , int mem_limit);
        std::vector<int> Run(const std::string& file_name , const std::string&id , int test_count , int cpu_limit , int mem_limit);
        std::string Run_one_test(const std::string&file_name , const std::string&data , int& staus_code, int cpu_limit , int mem_limit);
        bool Check_Output(std::string ans_path , std::string stdout_text);
        
    };
    
    enum class Run_out
    {
        //将注释写成汉语
        AC = 0, //答案通过 
        WA = 1, //答案错误
        TLE = 2, //时间超限
        MLE = 3, //内存超限
        RE = 4,  //运行时错误
        CE = 5, //编译错误
    };

};


#endif