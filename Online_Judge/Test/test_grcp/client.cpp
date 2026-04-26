#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>
#include "rpc.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using test_grpc::Request;
using test_grpc::Response;
using test_grpc::Greeter;

class GreeterClient {
public:
    GreeterClient(std::shared_ptr<Channel> channel)
        : stub_(Greeter::NewStub(channel)) {}

    // 调用简单 RPC
    void SayHello(const std::string& name, int id) {
        Request request;
        request.set_name(name);
        request.set_id(id);

        Response reply;
        ClientContext context;

        std::cout << "📤 发送请求: name=" << name << ", id=" << id << std::endl;

        Status status = stub_->SayHello(&context, request, &reply);

        if (status.ok()) {
            std::cout << "✅ 收到响应: " << reply.message() << std::endl;
            std::cout << "   成功状态: " << (reply.success() ? "true" : "false") << std::endl;
        } else {
            std::cout << "❌ RPC 失败: " << status.error_code() << ": "
                      << status.error_message() << std::endl;
        }
    }

    // 调用服务器流式 RPC
    void SayHelloStream(const std::string& name, int id) {
        Request request;
        request.set_name(name);
        request.set_id(id);

        Response reply;
        ClientContext context;

        std::cout << "📤 开始流式请求: name=" << name << std::endl;

        std::unique_ptr<grpc::ClientReader<Response>> reader(
            stub_->SayHelloStream(&context, request));

        int count = 0;
        while (reader->Read(&reply)) {
            count++;
            std::cout << "📥 收到流数据 #" << count << ": "
                      << reply.message() << std::endl;
        }

        Status status = reader->Finish();
        if (status.ok()) {
            std::cout << "✅ 流式传输完成，共接收 " << count << " 条消息" << std::endl;
        } else {
            std::cout << "❌ 流式传输失败: " << status.error_message() << std::endl;
        }
    }

private:
    std::unique_ptr<Greeter::Stub> stub_;
};

int main() {
    std::string server_address("localhost:50051");

    std::cout << "🔌 连接到服务器: " << server_address << std::endl;

    GreeterClient client(
        grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));

    // 测试简单 RPC
    std::cout << "\n=== 测试简单 RPC ===" << std::endl;
    client.SayHello("World", 1001);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 测试流式 RPC
    std::cout << "\n=== 测试流式 RPC ===" << std::endl;
    client.SayHelloStream("StreamClient", 2002);

    return 0;
}