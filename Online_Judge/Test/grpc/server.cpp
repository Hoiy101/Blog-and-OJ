#include <cstdio>
#include <iostream>
#include "rpc.pb.h"
#include "rpc.grpc.pb.h"

class Service :public Greeter::Service
{
public:
    virtual grpc::Status SayHello(grpc::ServerContext* context , const Request* request , Response* response)
    {
        std::cout << request->age();
    }
};

int main(void)
{

    return 0;
}