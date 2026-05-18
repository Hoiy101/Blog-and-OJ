cmake_minimum_required(VERSION 3.10)

project(compile_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Drogon_DIR "//usr/local/lib/cmake/Drogon/DrogonConfig.cmake")
set(jsoncpp_DIR "/usr/local/lib/cmake/jsoncpp")

#设置第三方库
find_package(Threads REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(Boost REQUIRED)
find_package(Drogon REQUIRED COMPONENTS json)

#包含头文件目录
include_directories( 
    .
    ../Common
    ../Common/Http/
    ../Common/Util/
    ../Common/Log/
    ../Common/Pool/
)

add_executable(compile_server 
    ../Common/Log/Buffer.cpp
    ../Common/Log/Format.cpp
    ../Common/Log/Logger.cpp
    ../Common/Log/Looper.cpp
    ../Common/Log/Sink.cpp
    ../Common/Log/util.cpp
    ../Common/Util/Tool.cpp
    ../Common/Pool/ThreadPool.cpp
    ./Compile_Run.cpp
    ./Compile_Server.cpp
    ./Compiler.cpp
    ./Runner.cpp
)

#链接库
target_link_libraries(${PROJECT_NAME}
    Threads::Threads
    JsonCpp::JsonCpp
    Boost::headers
    Drogon::Drogon
)