#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

#include <grpcpp/grpcpp.h>
#include "rpc.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;
using test_grpc::Request;
using test_grpc::Response;
using test_grpc::Greeter;

class GreeterServiceImpl final : public Greeter::Service {
    // 简单 RPC 实现
    Status SayHello(ServerContext* context, const Request* request, Response* reply) override {
        std::cout << "收到请求: name=" << request->name()
                  << ", id=" << request->id() << std::endl;

        std::string message = "Hello, " + request->name() + "! Your ID is: " + std::to_string(request->id());
        reply->set_message(message);
        reply->set_success(true);

        return Status::OK;
    }

    // 服务器流式 RPC 实现
    Status SayHelloStream(ServerContext* context, const Request* request, ServerWriter<Response>* writer) override {
        std::cout << "开始流式响应客户端: " << request->name() << std::endl;

        for (int i = 1; i <= 5; i++) {
            Response response;
            response.set_message("Stream message #" + std::to_string(i) + " for " + request->name());
            response.set_success(true);

            if (!writer->Write(response)) {
                std::cout << "流中断" << std::endl;
                break;
            }

            std::cout << "发送流数据 #" << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << "流式响应完成" << std::endl;
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    GreeterServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "✅ 服务器已启动，监听端口: " << server_address << std::endl;
    std::cout << "按 Ctrl+C 停止服务器" << std::endl;

    server->Wait();
}

int main() {
    try {
        RunServer();
    } catch (const std::exception& e) {
        std::cerr << "服务器异常: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}