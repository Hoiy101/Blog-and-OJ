#添加陌生库直接指向路径参考(添加ctemplate库的时候)
cmake_minimum_required(VERSION 3.10)

project(OJ_Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#设置第三方库
find_package(Threads REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(Boost REQUIRED)
find_package(Drogon REQUIRED COMPONENTS json)
#find_package(ctemplate REQUIRED)
#find_package(mysqlclient REQUIRED)

set(MYSQL_INCLUDE_DIRS "/usr/include/mysql")
set(MYSQL_LIBRARIES "/usr/lib/x86_64-linux-gnu/libmysqlclient.so")


#包含头文件
include_directories(OJ_Server
    /usr/include/ctemplate
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/OJ/
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Http/
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Util/
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Pool/
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Search/Index/
    /home/zz/Desktop/library/cppjieba/include/cppjieba
    /home/zz/Desktop/library/cppjieba/deps/limonp/include/limonp
)

#添加ctemplate库路径
link_directories(/usr/lib/x86_64-linux-gnu)

add_executable(OJ_Server
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Search/Index/Index.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Search/Index/Searcher.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/Buffer.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/Format.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/Logger.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/Looper.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/Sink.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Log/util.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Util/Tool.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/Common/Pool/ThreadPool.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/OJ/Control.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/OJ/oj_Model.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/OJ/OJ_Server.cpp
    /home/zz/Desktop/project/Blog-and-OJ/Online_Judge/OJ/View.cpp
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

