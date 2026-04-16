#include <signal.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Control.hpp"
#include "../Common/Util/Tool.hpp"
#include "../Search/Index/Searcher.hpp"


#include <drogon/drogon.h>
#include <drogon/HttpClient.h>
#include <drogon/HttpResponse.h>

static ns_control::Control *ctrl_ptr = nullptr; // 定义一个控制器指针，让其既可以局部使用，也可以全局使用

// 使用手册
void Usage(std::string proc)
{
    std::cerr << "Usage: " << "\n\t" << proc << " port" << std::endl;
}

// 写一个恢复主机的回调方法
void Recovery(int signo)
{
    ctrl_ptr->RecoveryMachine();
}

void init()
{
    const char * model_logger_name = "model_logger";
    const char *control_logger_name = "control_logger";
    const char * Server_logger_name = "server_model_logger";

    ns_util::BuildAsyncLogger(model_logger_name);
    ns_util::BuildAsyncLogger(Server_logger_name);
    ns_util::BuildAsyncLogger(control_logger_name);
}

int main(int argc , const char *argv[])
{
    init();
    signal(SIGQUIT , Recovery);

    if(argc != 2)
    {
        Usage(argv[0]);
        return 1;
    }

    //创建一个服务器和控制器
    ns_control::Control ctrl;
    ctrl_ptr = &ctrl;

    ns_Searcher::Searcher search;
    search.initSearcher();

    //注册路由
    //参数一:URL地址
    //参数二:lambda表达式触发URL后执行的逻辑
    //参数三:HTTP请求方法
    drogon::app().registerHandler("/all_questions", 
        [&ctrl](const drogon::HttpRequestPtr&req , std::function<void (const drogon::HttpResponsePtr &)> && callback){
        // 返回一张包含所有题目的html网页
        std::string html;
        ctrl.AllQuestions(&html); 
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setBody(html);
        resp->setContentTypeCode(drogon::CT_TEXT_HTML);
        resp->addHeader("charset" , "utf-8");
        callback(resp);
    } , {drogon::Get});

    //(\d+):可以从URL里面捕捉数字
    //R(): 原始字符串
    drogon::app().registerHandler("/question/{id}", 
        [&ctrl](const drogon::HttpRequestPtr&req , std::function<void (const drogon::HttpResponsePtr &)> && callback)  {
        //std::string number = req->getParameter("number");

        std::string html;
        std::string URL = req->getPath();
        std::string number = URL.substr(URL.find_last_of("/") + 1);
        //std::cout << "number: " << number << std::endl;
        ctrl.Question(number, &html); 
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setBody(html);
        resp->setContentTypeCode(drogon::CT_TEXT_HTML);
        resp->addHeader("charset" , "utf-8");
        callback(resp);
    } , {drogon::Get});

    // 用户提交代码，使用我们的判题功能（1. 每道题的测试用例 2. compile_and_run）
    drogon::app().registerHandler("/judge/{id}", 
        [&ctrl](const drogon::HttpRequestPtr&req , std::function<void (const drogon::HttpResponsePtr &)> && callback){

        std::string number = req->getParameter("id");
        std::string result_json;
        ctrl.Judge(req->body().data() , &result_json);
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setBody(result_json);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        resp->addHeader("charset" , "utf-8");
        callback(resp);
    } , {drogon::Post});
    
    drogon::app().registerHandler("/s", 
        [&search](const drogon::HttpRequestPtr&req , std::function<void (const drogon::HttpResponsePtr &)> && callback){

            std::string word = req->getParameter("word");
            std::string result_json;
            search.Search(word , &result_json);
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setBody(result_json);
            resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
            resp->addHeader("charset" , "utf-8");
            callback(resp);
    } , {drogon::Get});

    drogon::app().registerHandler("/search", 
        [&search](const drogon::HttpRequestPtr&req , std::function<void (const drogon::HttpResponsePtr &)> && callback){
            std::string html;
            auto resp = drogon::HttpResponse::newHttpResponse();
            bool st = ns_util::FileUtil::ReadFile("../wwwroot/search.html" , &html , true);
            if(!st)
            {
                std::cerr << "read file error\n";
                resp->setBody("read file error");
            }
            else
            {
                resp->setBody(html);
            }
            resp->setBody(html);
            resp->setContentTypeCode(drogon::CT_TEXT_HTML);
            resp->addHeader("charset" , "utf-8");
            callback(resp);
    } , {drogon::Get});

    drogon::app().addListener("0.0.0.0", atoi(argv[1])); // 启动http服务
    drogon::app().run();
    return 0;
}

