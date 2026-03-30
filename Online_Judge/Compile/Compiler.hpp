#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "../Common/Log/clog.hpp"
#include "../Common/Util/Tool.hpp"

#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

//这里也要修改一下与日志器有关的操作

namespace ns_compile
{
    inline const char *compile_logger_name = "compile_logger";
    //用来编译代码
    //当我们传入没有文件后缀的文件的时候会生成三个文件:1.cpp 2.compile_error 3.exe
    class Compiler
    {
    public:
        Compiler();
        ~Compiler();

        static bool Compile(const std::string &file_name);
    };
};


#endif // COMPILER_HPP      