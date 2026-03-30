#include "Compile_Run.hpp"

ns_compile_run::CompileRun::CompileRun()
{

}

ns_compile_run::CompileRun::~CompileRun()
{

}

void ns_compile_run::CompileRun::RemoveTempFile(const std::string& file_name , const std::string&id)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(compile_run_logger_name);
    //要判断一下,每个文件是否存在我们不确定
    std::string Src = ns_util::PathUtil::Src_Cpp(file_name);
    if(ns_util::FileUtil::IsFileExist(Src)) 
        unlink(Src.c_str());  //文件存在就删除

    std::string Compile_error = ns_util::PathUtil::CompileError(file_name);
    if(ns_util::FileUtil::IsFileExist(Compile_error))
        unlink(Compile_error.c_str());

    std::string Execute = ns_util::PathUtil::Exe(file_name);
    if(ns_util::FileUtil::IsFileExist(Execute))
        unlink(Execute.c_str());

    /*std::string Stdin = ns_util::PathUtil::Stdin(file_name);
    if(ns_util::FileUtil::IsFileExist(Stdin))
        unlink(Stdin.c_str());*/

    std::string Stdout = ns_util::PathUtil::Stdout("Judge/" + id);
    /*if(ns_util::FileUtil::IsFileExist(Stdout))
    {
        unlink(Stdout.c_str());
    }
    else
        logger->Error("标准输出文件%s不存在",Stdout.c_str());*/

    /*std::string Stderr = ns_util::PathUtil::Stderr(file_name);
    if(ns_util::FileUtil::IsFileExist(Stderr))
        unlink(Stderr.c_str());*/
}

// 将信号转化成为报错的原因
// code > 0 : 进程收到了信号导致异常错误
// code < 0 : 整个过程非运行报错（代码为空，编译报错等）
// cod == 0 : 整个过程全部完成
std::string ns_compile_run::CompileRun::CodeToDesc(int code , const std::string file_name)
{
    std::string desc;
    switch (code)
    {
    case 0:
        desc = "编译运行成功";
        break;
    case -1:
        desc = "用户提交的代码是空";
        break;
    case -2:
        desc = "未知错误";
        break;
    case -3: // 代码编译的时候发生了错误
        ns_util::FileUtil::ReadFile(ns_util::PathUtil::CompileError(file_name), &desc, true); // 读取编译报错的文件
        break;
    case SIGABRT: // 信号6
        desc = "内存超过范围";
        break;
    case SIGXCPU: // 信号24
        desc = "CPU使用超时";
        break;
    case SIGFPE: // 信号8
        desc = "浮点数溢出";
        break;
    default:
        desc = "未知" + std::to_string(code);
        break;
    }

    return desc;
}

ns_compile_run::UserStatus ns_compile_run::CompileRun::StatusToEnum(bool is_test)
{
    if(is_test)
        return UserStatus::TEST;
    
    return UserStatus::SUMMIT;
}

ns_compile_run::JudgeStatus ns_compile_run::CompileRun::CodeToJudgeStatus(int code) 
{
    switch (code) 
    {
    case 0:
        return ns_compile_run::JudgeStatus::AC;  // 编译运行成功，可以继续判断答案
    
    case -1:  // 用户提交的代码是空
    case -2:  // 未知错误
        return ns_compile_run::JudgeStatus::UN;
    
    case -3:  // 编译错误
        return ns_compile_run::JudgeStatus::CE;  // 明确区分编译错误
    
    // 时间超限
    case SIGXCPU:
    case SIGALRM:
    case SIGVTALRM:
    case SIGPROF:
        return ns_compile_run::JudgeStatus::TLE;
    
    // 内存超限
    case SIGABRT:
    case SIGSEGV:
    case SIGBUS:
        return ns_compile_run::JudgeStatus::MLE;
    
    // 运行时错误
    case SIGFPE:
    case SIGILL:
    case SIGSYS:
    case SIGTRAP:
        return ns_compile_run::JudgeStatus::RTE;
    
    // 外部终止
    case SIGTERM:
    case SIGKILL:
    case SIGQUIT:
    case SIGINT:
        return ns_compile_run::JudgeStatus::UN;
    
    default:
        if (code > 0) 
        {
            return ns_compile_run::JudgeStatus::RTE;
        } 
        return ns_compile_run::JudgeStatus::UN;
    }
}

void ns_compile_run::CompileRun::Start(const std::string& in_json , std::string* out_json)
{
    static ns_pool::ThreadPool thread_pool;

    //将数据序列化
    Json::Value in_value;
    Json::Reader reader;
    reader.parse(in_json , in_value);
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(compile_run_logger_name);

    //拿到我们需要的数据
    //在json里面选出用户的提交还是测试
    std::string code = in_value["code"].asString();
    std::string input = in_value["input"].asString();
    std::string number = in_value["number"].asString();
    UserStatus user_status = StatusToEnum(in_value["is_self_test"].asBool());
    int test_count = in_value["test_count"].asInt();
    int cpu_limit = in_value["cpu_limit"].asInt();
    int mem_limit = in_value["mem_limit"].asInt();
    
    //logger->debug("user_status = %s , input:%s",user_status == UserStatus::TEST ? "TEST" : "SUMMIT" , input.c_str());
    //logger->debug("code = %s",code.c_str());

    //创建返回数据的json
    Json::Value out_value;
    std::vector<int>commit_status;
    int status_code = 0;        //状态码
    int run_result = 0;         //运行结果
    std::string output_file = "Judge/" + number;
    std::string file_name;      //文件名称

    file_name = ns_util::FileUtil::UniqueFileName();
    //这里以后改成从json里获取的id
    if(code.size() == 0)
    {
        //代码为空
        status_code = -1;
    }

    if(!ns_util::FileUtil::WriteFile(ns_util::PathUtil::Src_Cpp(file_name) , code))
    {
        //写入文件失败
        status_code = -2;
    }
    //编译
    if(!ns_compile::Compiler::Compile(file_name))
    {
        //如果编译失败
        status_code = -3;
    }
    //logger->debug("编译成功");

    //接受数据
    std::string Stdout;
    std::string Stderr;

    //logger->debug("开始执行代码");
    //运行代码
    switch(user_status)
    {
    case UserStatus::TEST:
        //logger->debug("开始测试");
        //测试的时候先尝试读取文件内容
        Stdout = runner.Run_one_test(file_name , input , status_code , cpu_limit , mem_limit);
        logger->debug("test Stdout = %s",Stdout.c_str());
        out_value["stdout"] = Stdout;

        break;
    case UserStatus::SUMMIT:
        //提交代码
        commit_status = runner.Run(file_name , number , test_count , cpu_limit , mem_limit);
        Stdout = "测试结果如下\n";
        for(auto status : commit_status)
            Stdout += std::to_string(status) + " ";
        
        out_value["stdout"] = Stdout;
        
        logger->debug("提交代码测试结果:%s",Stdout.c_str());
        break;
    default:
        logger->Error("未知的用户状态");
        out_value["stdout"] = "用户状态错误，请联系管理员";
        break;
    }


    if(run_result < 0)
    {
        //内部错误
        status_code = -2;
    }
    else if(run_result > 0)
    {
        //运行错误
        status_code = run_result;
    }
    else 
    {
        //运行成功
        status_code = 0;
    }

    //之后实现例如0:正确，23按照数字搞不同的string然后返回给前端
    if(status_code == 0 && user_status == UserStatus::SUMMIT)
    {
        //这里声明一个枚举类表示最后的结果
    }
    
    out_value["status"] = status_code;
    out_value["reason"] = CodeToDesc(status_code , file_name);      //获取错误信息
    //logger->debug("status_code = %d" , status_code);

    //序列化
    Json::StyledWriter writer;
    *out_json = writer.write(out_value);

    //清除临时文件
    thread_pool.enqueue(RemoveTempFile , file_name , number);
}