cmake_minimum_required(VERSION 3.10)

project(compile_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#设置第三方库
find_package(Threads REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(Boost REQUIRED)
find_package(Drogon REQUIRED COMPONENTS json)

#获取环境变量
set(ROOT_DIR $ENV{ONLINE_JUDGE})

#包含头文件目录
include_directories(compile_server 
    ${ROOT_DIR}/Compile/
    ${ROOT_DIR}/Common/Http/
    ${ROOT_DIR}/Common/Util/
    ${ROOT_DIR}/Common/Log/
    ${ROOT_DIR}/Common/Pool/
)

#添加源文件
set(SOURCES
    ${ROOT_DIR}/Common/Log/Buffer.cpp
    ${ROOT_DIR}/Common/Log/Format.cpp
    ${ROOT_DIR}/Common/Log/Logger.cpp
    ${ROOT_DIR}/Common/Log/Looper.cpp
    ${ROOT_DIR}/Common/Log/Sink.cpp
    ${ROOT_DIR}/Common/Log/util.cpp
    ${ROOT_DIR}/Common/Util/Tool.cpp
    ${ROOT_DIR}/Common/Pool/ThreadPool.cpp
    ${ROOT_DIR}/Compile/Compile_Run.cpp
    ${ROOT_DIR}/Compile/Compile_Server.cpp
    ${ROOT_DIR}/Compile/Compiler.cpp
    ${ROOT_DIR}/Compile/Runner.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

#链接库
target_link_libraries(${PROJECT_NAME}
    Threads::Threads
    JsonCpp::JsonCpp
    Boost::headers
    Drogon::Drogon
)