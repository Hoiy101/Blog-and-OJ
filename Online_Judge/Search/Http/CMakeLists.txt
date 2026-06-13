#编写CMakeLists.txt工程文件
cmake_minimum_required(VERSION 3.10)

#项目名
project(Boost_Search)

#设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED NO)

#设置Boost库
find_package(Boost REQUIRED COMPONENTS filesystem system thread)
include_directories(${Boost_INCLUDE_DIRS})

#获取环境变量
set(ROOT_DIR $ENV{ONLINE_JUDGE})
set(LIB_DIR $ENV{LIBRARY})

#包含头文件目录
include_directories(
    /usr/include/jsoncpp
    ${ROOT_DIR}/Search/Code/Index/
    ${ROOT_DIR}/Common/Http/               #httplib   头文件
    ${LIB_DIR}/cppjieba/include/cppjieba
    ${LIB_DIR}/cppjieba/deps/limonp/include/limonp
    ${ROOT_DIR}/OJ
    ${ROOT_DIR}/Common/Log
    ${ROOT_DIR}/Common/Util

    ${Boost_INCLUDE_DIRS}                                             # Boost 1.90.0 头文件
)

#添加源文件
set(SOURCES
    Http_Server.cpp
    ${ROOT_DIR}/Search/Index/Index.cpp
    ${ROOT_DIR}/Search/Index/Searcher.cpp
    ${ROOT_DIR}/OJ/oj_Model.cpp
    ${ROOT_DIR}/Common/Util/Tool.cpp
    ${ROOT_DIR}/Common/Log/Buffer.cpp
    ${ROOT_DIR}/Common/Log/Format.cpp
    ${ROOT_DIR}/Common/Log/Logger.cpp
    ${ROOT_DIR}/Common/Log/Looper.cpp
    ${ROOT_DIR}/Common/Log/Sink.cpp
    ${ROOT_DIR}/Common/Log/util.cpp
)

#添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接库
target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    jsoncpp
    pthread
    mysqlclient
)