#编写CMakeLists.txt工程文件
cmake_minimum_required(VERSION 3.10)

#项目名
project(Parse)

#设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED NO)

#设置Boost库
find_package(Boost REQUIRED COMPONENTS filesystem system thread)
include_directories(${Boost_INCLUDE_DIRS})

#包含头文件目录
include_directories(
    .                                       # 当前目录，包含 Util.hpp
    /home/zz/Desktop/library/cppjieba/include/cppjieba
    /home/zz/Desktop/library/cppjieba/deps/limonp/include/limonp
    ${Boost_INCLUDE_DIRS}                     # Boost 1.90.0 头文件
)

#添加可执行文件
add_executable(${PROJECT_NAME}
    Util.cpp
    Parser.cpp
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    pthread
)