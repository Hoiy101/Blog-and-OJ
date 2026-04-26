cmake_minimum_required(VERSION 3.10)
project(test_grpc)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 pkg-config
find_package(PkgConfig REQUIRED)

# 使用 pkg-config 查找 protobuf
pkg_search_module(PROTOBUF REQUIRED protobuf)
if(PROTOBUF_FOUND)
    message(STATUS "Found protobuf: ${PROTOBUF_VERSION}")
    include_directories(${PROTOBUF_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "protobuf not found via pkg-config")
endif()

# 使用 pkg-config 查找 gRPC
pkg_search_module(GRPC REQUIRED grpc++)
if(GRPC_FOUND)
    message(STATUS "Found gRPC: ${GRPC_VERSION}")
    include_directories(${GRPC_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "gRPC not found via pkg-config")
endif()

# 查找 protoc 和 grpc_cpp_plugin
find_program(PROTOC protoc)
if(NOT PROTOC)
    message(FATAL_ERROR "protoc not found")
endif()

find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
if(NOT GRPC_CPP_PLUGIN)
    message(FATAL_ERROR "grpc_cpp_plugin not found")
endif()

# 设置 protobuf 文件
set(PROTO_FILE rpc.proto)
set(PROTO_SRCS ${CMAKE_CURRENT_BINARY_DIR}/rpc.pb.cc)
set(PROTO_HDRS ${CMAKE_CURRENT_BINARY_DIR}/rpc.pb.h)
set(GRPC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/rpc.grpc.pb.cc)
set(GRPC_HDRS ${CMAKE_CURRENT_BINARY_DIR}/rpc.grpc.pb.h)

# 生成 protobuf 代码
add_custom_command(
        OUTPUT ${PROTO_SRCS} ${PROTO_HDRS}
        COMMAND ${PROTOC}
        ARGS --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
        --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
        ${PROTO_FILE}
        DEPENDS ${PROTO_FILE}
        COMMENT "Generating protobuf code"
)

# 生成 gRPC 代码
add_custom_command(
        OUTPUT ${GRPC_SRCS} ${GRPC_HDRS}
        COMMAND ${PROTOC}
        ARGS --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
        --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        ${PROTO_FILE}
        DEPENDS ${PROTO_FILE}
        COMMENT "Generating gRPC code"
)

# 服务器可执行文件
add_executable(server server.cpp ${PROTO_SRCS} ${GRPC_SRCS})
target_include_directories(server PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROTOBUF_INCLUDE_DIRS}
        ${GRPC_INCLUDE_DIRS}
)
target_link_libraries(server
        ${PROTOBUF_LIBRARIES}
        ${GRPC_LIBRARIES}
        pthread
)

# 客户端可执行文件
add_executable(client client.cpp ${PROTO_SRCS} ${GRPC_SRCS})
target_include_directories(client PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROTOBUF_INCLUDE_DIRS}
        ${GRPC_INCLUDE_DIRS}
)
target_link_libraries(client
        ${PROTOBUF_LIBRARIES}
        ${GRPC_LIBRARIES}
        pthread
)
