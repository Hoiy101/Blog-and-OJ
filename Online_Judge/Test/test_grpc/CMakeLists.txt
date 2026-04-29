cmake_minimum_required(VERSION 3.8)
project(YourGrpcProject)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

# 包含 gRPC 的头文件
include_directories(/home/ai/mkdi.local/include)
include_directories(include)
include_directories(protos)

# 设置 gRPC 的库路径
link_directories(/home/ai/.local/lib)

# 查找 gRPC 包
find_package(Protobuf REQUIRED) # 要先找Protobuf，否则会报错
find_package(gRPC REQUIRED)


# 如果 gRPC 没有被正确找到，您可能需要设置 gRPC_DIR
# set(gRPC_DIR /path/to/grpc/lib/cmake/grpc)

# 设置源文件
set(SERVER_SOURCE_FILES servers/server.cpp protos/my_service.grpc.pb.cpp protos/my_service.pb.cpp)
set(CLIENT_SOURCE_FILES clients/client.cpp protos/my_service.grpc.pb.cpp protos/my_service.pb.cpp)

# 添加可执行文件
add_executable(server ${SERVER_SOURCE_FILES})
add_executable(client ${CLIENT_SOURCE_FILES})

# 链接 gRPC 和相关库
target_link_libraries(server
    gRPC::grpc++ gRPC::grpc++_reflection
    protobuf pthread dl z)
target_link_libraries(client
    gRPC::grpc++ gRPC::grpc++_reflection
    protobuf pthread dl z)

# 如果需要从 .proto 文件生成 .cpp 和 .h 文件，可以使用以下命令，这块暂时没试过，仅供参考。
# find_program(PROTOC_EXECUTABLE protoc)
# find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
# add_custom_command(
#     OUTPUT "${PROTOS_PATH}/my_service.grpc.pb.cc"
#            "${PROTOS_PATH}/my_service.pb.cc"
#     COMMAND ${PROTOC_EXECUTABLE}
#     ARGS --grpc_out="${PROTOS_PATH}"
#          --cpp_out="${PROTOS_PATH}"
#          --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN_EXECUTABLE}"
#          -I "${PROTOS_PATH}"
#          "${PROTOS_PATH}/my_service.proto"
#     DEPENDS "${PROTOS_PATH}/my_service.proto")

