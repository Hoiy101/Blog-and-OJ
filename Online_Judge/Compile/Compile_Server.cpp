#include "../Common/Pool/ThreadPool.hpp"
#include "../Common/Util/Tool.hpp"
#include "Compile_Run.hpp"

#include <drogon/drogon.h>
#include <mysql/mysql.h>
#include <string>
#include <future>
#include <thread>

void init()
{
    const char *compile_run_logger_name = "compile_run_logger";
    const char *compile_logger_name = "compile_logger";
    const char *running_logger_name = "running_logger";
    const char *util_logger_name = "util_logger";

    ns_util::BuildAsyncLogger(compile_run_logger_name);
    ns_util::BuildAsyncLogger(compile_logger_name);
    ns_util::BuildAsyncLogger(running_logger_name);
    ns_util::BuildAsyncLogger(util_logger_name);
}

int main(int argc , const char *argv[])
{
    if(argc != 2)
    {
        std::cout << "使用错误\n";
        return 1;
    }

    init();

    //创建运行需要的些许东西
    ns_compile_run::CompileRun runner;
    //创建线程池
    ns_pool::ThreadPool pool(5);
    
    // 参数：req：用户的需求，resp：服务器的相应
    drogon::app().registerHandler("/compile_and_run", 
        [&runner , &pool](const drogon::HttpRequestPtr&req , 
            std::function<void (const drogon::HttpResponsePtr &)> &&callback)
    {
        std::string in_json = req->body().data();
        if (!in_json.empty())
        {
            pool.enqueue([&runner , in_json = std::move(in_json) , callback = std::move(callback)]()
            {
                std::string out_json;
                // 编译并运行用户传过来的代码
                runner.Start(in_json , &out_json);
                // 要响应的内容
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setBody(out_json);
                resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
                resp->addHeader("charset" , "utf-8");
                callback(resp);
            });
        }
    } , {drogon::Post});
 
    
    // 让服务器在所有ip，指定端口服务
    drogon::app().addListener("0.0.0.0", atoi(argv[1])); // 启动http服务
    drogon::app().run();

    return 0;
}