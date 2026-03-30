#ifndef COMPILE_RUN_HPP
#define COMPILE_RUN_HPP

#include <future>
#include <signal.h>
#include <unistd.h>
#include <jsoncpp/json/json.h>

#include "Runner.hpp"
#include "Compiler.hpp"
#include "../Common/Log/clog.hpp"
#include "../Common/Util/Tool.hpp"

//一个对象对应一个线程对应着一个运行结果和运行逻辑

namespace ns_compile_run
{
    inline const char *compile_run_logger_name = "compile_run_logger";

    enum class UserStatus
    {
        TEST = 0,
        SUMMIT = 1
    };

    enum class JudgeStatus
    {
        AC = 0,     // Accepted - 答案正确/通过
        WA = 1,     // Wrong Answer - 答案错误
        RTE = 2,    // Runtime Error - 运行时错误
        TLE = 3,    // Time Limit Exceeded - 时间超限
        MLE = 4,    // Memory Limit Exceeded - 内存超限
        CE = 5,     // Compile Error - 编译错误
        RE = 6,     // Runtime Error - 运行时错误（某些判题系统与RTE同义或细分）
        UN = 7,     // 其他错误
    };

    class CompileRun
    {
    public:
        CompileRun();
        ~CompileRun();

    public:
        //清除临时文件
        static void RemoveTempFile(const std::string& file_name , const std::string&id);
        static std::string CodeToDesc(int code , const std::string file_name);
        void Start(const std::string& in_json , std::string* out_json);

        //将字符串转换为枚举类型
        UserStatus StatusToEnum(bool status);
        JudgeStatus CodeToJudgeStatus(int code);



    private:
        ns_runner::Runner runner;

    };

};

#endif