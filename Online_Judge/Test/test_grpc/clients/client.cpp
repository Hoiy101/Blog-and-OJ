#include <grpcpp/grpcpp.h>
#include "my_service.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using myservice::MyRequest;
using myservice::MyResponse;
using myservice::MyService;

class MyClient {
 public:
  MyClient(std::shared_ptr<Channel> channel)
      : stub_(MyService::NewStub(channel)) {}

  std::string SayHello(const std::string& user) {
    MyRequest request;
    request.set_name(user);
    MyResponse reply;
    ClientContext context;

    Status status = stub_->SayHello(&context, request, &reply);

    if (status.ok()) {
      return reply.greeting();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<MyService::Stub> stub_;
};

int main(int argc, char** argv) {
  MyClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  std::string user("World");
  std::string reply = client.SayHello(user);
  std::cout << "Client received: " << reply << std::endl;

  return 0;
}
