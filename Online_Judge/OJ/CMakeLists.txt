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


#获取环境变量
set(ROOT_DIR $ENV{ONLINE_JUDGE})
set(LIB_DIR $ENV{LIBRARY})

#包含头文件
include_directories(OJ_Server
    /usr/include/ctemplate
    ${ROOT_DIR}/OJ/
    ${ROOT_DIR}/Common/Http/
    ${ROOT_DIR}/Common/Util/
    ${ROOT_DIR}/Common/Log/
    ${ROOT_DIR}/Common/Pool/
    ${ROOT_DIR}/Search/Index/
    ${LIB_DIR}/cppjieba/include/cppjieba
    ${LIB_DIR}/cppjieba/deps/limonp/include/limonp
)

#添加ctemplate库路径
link_directories(/usr/lib/x86_64-linux-gnu)

#添加源文件
set(SOURCES
    ${ROOT_DIR}/Search/Index/Index.cpp
    ${ROOT_DIR}/Search/Index/Searcher.cpp
    ${ROOT_DIR}/Common/Log/Buffer.cpp
    ${ROOT_DIR}/Common/Log/Format.cpp
    ${ROOT_DIR}/Common/Log/Logger.cpp
    ${ROOT_DIR}/Common/Log/Looper.cpp
    ${ROOT_DIR}/Common/Log/Sink.cpp
    ${ROOT_DIR}/Common/Log/util.cpp
    ${ROOT_DIR}/Common/Util/Tool.cpp
    ${ROOT_DIR}/Common/Pool/ThreadPool.cpp
    ${ROOT_DIR}/OJ/Control.cpp
    ${ROOT_DIR}/OJ/oj_Model.cpp
    ${ROOT_DIR}/OJ/OJ_Server.cpp
    ${ROOT_DIR}/OJ/View.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

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

