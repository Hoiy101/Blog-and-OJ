cmake_minimum_required(VERSION 3.8)

project(rpc)

set(CMAKE_CXXSTANDARD 17)

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

include_directories(.)


set(SERVER_SOURCE_FILES server.cpp rpc.grpc.pb.cc rpc.pb.cc)
set(CLIENT_SOURCE_FILES client.cpp rpc.pb.cc rpc.grpc.pb.cc)

add_executable(Server ${SERVER_SOURCE_FILES})
add_executable(Client ${CLIENT_SOURCE_FILES})

target_link_libraries(Server
        gRPC::grpc++_reflection
        gRPC::grpc++
        protobuf
        pthread
        dl
        z
)

target_link_libraries(Client
        gRPC::grpc++_reflection
        gRPC::grpc++
        protobuf
        pthread
        dl
        z
)
