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

#包含头文件目录
include_directories(
    /usr/include/jsoncpp
    /home/zz/Desktop/project/Online_Judge/Search/Code/Index/
    /home/zz/Desktop/project/Online_Judge/Common/Http/               #httplib   头文件
    /home/zz/Desktop/library/cppjieba/include/cppjieba
    /home/zz/Desktop/library/cppjieba/deps/limonp/include/limonp
    /home/zz/Desktop/project/Online_Judge/OJ
    /home/zz/Desktop/project/Online_Judge/Common/Log
    /home/zz/Desktop/project/Online_Judge/Common/Util

    ${Boost_INCLUDE_DIRS}                                             # Boost 1.90.0 头文件
)

#添加可执行文件
add_executable(${PROJECT_NAME}
    Http_Server.cpp
    /home/zz/Desktop/project/Online_Judge/Search/Index/Index.cpp
    /home/zz/Desktop/project/Online_Judge/Search/Index/Searcher.cpp
    /home/zz/Desktop/project/Online_Judge/OJ/oj_Model.cpp
    /home/zz/Desktop/project/Online_Judge/Common/Util/Tool.cpp
    /home/zz/Desktop/project/Online_Judge/Common/Log/Buffer.cpp
    /home/zz/Desktop/project/Online_Judge/Common/Log/Format.cpp
    /home/zz/Desktop/project/Online_Judge/Common/Log/Logger.cpp
    /home/zz/Desktop/project/Online_Judge/Common/Log/Looper.cpp
    /home/zz/Desktop/project/Online_Judge/Common/Log/Sink.cpp
    /home/zz/Desktop/project/Online_Judge/Common/Log/util.cpp
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    jsoncpp
    pthread
    mysqlclient
)