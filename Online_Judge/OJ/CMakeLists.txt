#添加陌生库直接指向路径参考(添加ctemplate库的时候)
cmake_minimum_required(VERSION 3.10)

project(OJ_Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Drogon_DIR "//usr/local/lib/cmake/Drogon/DrogonConfig.cmake")
set(jsoncpp_DIR "/usr/local/lib/cmake/jsoncpp")

#设置第三方库
find_package(Threads REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(Boost REQUIRED)
find_package(Drogon REQUIRED)
#find_package(ctemplate REQUIRED)
#find_package(mysqlclient REQUIRED)

set(MYSQL_INCLUDE_DIRS "/usr/include/mysql")
set(MYSQL_LIBRARIES "/usr/lib/x86_64-linux-gnu/libmysqlclient.so")


#包含头文件
include_directories(OJ_Server
    /usr/include/ctemplate
    ../
    ../OJ/
    ../Common/Http/
    ../Common/Util/
    ../Common/Log/
    ../Common/Pool/
    ../Search/Index/
    ../../../../library/cppjieba/include/cppjieba
    ../../../../library/cppjieba/deps/limonp/include/limonp
)

#添加ctemplate库路径
link_directories(/usr/lib/x86_64-linux-gnu)

add_executable(OJ_Server
    ../Search/Index/Index.cpp
    ../Search/Index/Searcher.cpp
    ../Common/Log/Buffer.cpp
    ../Common/Log/Format.cpp
    ../Common/Log/Logger.cpp
    ../Common/Log/Looper.cpp
    ../Common/Log/Sink.cpp
    ../Common/Log/util.cpp
    ../Common/Util/Tool.cpp
    ../Common/Pool/ThreadPool.cpp
    ../OJ/Control.cpp
    ../OJ/oj_Model.cpp
    ../OJ/OJ_Server.cpp
    ../OJ/View.cpp
)

#链接第三方库
target_link_libraries(${PROJECT_NAME}
    Threads::Threads
    JsonCpp::JsonCpp
    Boost::headers
    Drogon::Drogon
    mysqlclient
    ctemplate
    pthread
)

