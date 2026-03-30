#include "Compiler.hpp"

ns_compile::Compiler::Compiler()
{

}

ns_compile::Compiler::~Compiler()
{

}

bool ns_compile::Compiler::Compile(const std::string &file_name)
{
    pid_t pid = fork(); //创建子进程来编译代码
    if(pid < 0)
    {
        ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(compile_logger_name);
        logger->Error("fork error!");

        return false;
    }
    else if(pid == 0)
    {
        //子进程
        umask(0);   //设置文件权限掩码,设置初始化为0，表示不屏蔽任何权限
        int compile_error_fd = open(ns_util::PathUtil::CompileError(file_name).c_str() , O_CREAT | O_WRONLY , 0664);        //先打开这个错误文件
        if(compile_error_fd < 0)
        {
            ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(compile_logger_name);
            logger->warn("has no error file\n");

            exit(1);        //使用exit结束整个进程
        }

        //重新定向error文件
        dup2(compile_error_fd , 2);
        
        //程序切换不影响进程的文件描述符
        //子进程:调用编译器进行代码的编译工作
        execlp("g++" , "g++" , "-o" , ns_util::PathUtil::Exe(file_name).c_str() , ns_util::PathUtil::Src_Cpp(file_name).c_str() , "-D" , "COMPILER_ONLINE", "-std=c++11" , nullptr);

        //如果编译失败
        ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(compile_logger_name);
        logger->Error("启动编译器错误\n");
    }
    else
    {
        waitpid(pid , nullptr , 0);         //等待子进程结束并回收资源

        //判断是否可生成可执行文件
        if(ns_util::FileUtil::IsFileExist(ns_util::PathUtil::Exe(file_name)))
        {
            ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(compile_logger_name);
            logger->info("%s编译成功!" , ns_util::PathUtil::Src_Cpp(file_name));
            return true;
        }

        ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(compile_logger_name);
        logger->debug("编译失败，没有可执行文件");
        return false;
    }

    return false;
}