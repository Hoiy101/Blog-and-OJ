cmake_minimum_required(VERSION 3.10)

project(compile_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#设置第三方库
find_package(Threads REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(Boost REQUIRED)
find_package(Drogon REQUIRED COMPONENTS json)

#包含头文件目录
include_directories(compile_server 
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Compile/
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Http/
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Util/
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Pool/
)

add_executable(compile_server 
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/Buffer.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/Format.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/Logger.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/Looper.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/Sink.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/util.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Util/Tool.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Pool/ThreadPool.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Compile/Compile_Run.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Compile/Compile_Server.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Compile/Compiler.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Compile/Runner.cpp
)

#链接库
target_link_libraries(${PROJECT_NAME}
    Threads::Threads
    JsonCpp::JsonCpp
    Boost::headers
    Drogon::Drogon
)