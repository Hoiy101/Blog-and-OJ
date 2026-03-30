#include <filesystem>
#include <iostream>
#include <fstream>

//先创建文件然后更改绝对地址变为相对地址

#include "/home/zz/Desktop/project/Online_Judge/Common/Http/httplib.h"
#include "../Index/Searcher.hpp"
#include "../../../OJ/oj_Model.hpp"

const std::string input = "../../data/raw_html/raw.txt";
const std::string root_path = "../wwwroot";

int main(int argc , const char *argv[])
{
    std::filesystem::path current_path = std::filesystem::current_path();
    std::cout << current_path.string() << std::endl;

    std::ofstream file("Online_Judge.log");

    //搜搜大致流程:获取Index单例根据数据构建倒排索引，根据关键字使用Searcher返回json串

    ns_Searcher::Searcher search;
    search.initSearcher(input);

    //创建一个Server对象用于搭建服务器
    httplib::Server server;

    //访问首页
    server.set_base_dir(root_path.c_str());

    //这里用来处理GET请求
    server.Get("/s" , [&search](const httplib::Request& req , httplib::Response& rsp)
    {
        //has_param：这个函数用来检测用户的请求中是否有搜索关键字，参数中的word就是给用户关键字取的名字（类似word=split）    
        if(!req.has_param("word")){    
            rsp.set_content("必须要有搜索关键字!", "text/plain; charset=utf-8");    
            return;    
        }    

        //获取用户输入的关键字
        std::string word = req.get_param_value("word");    
        LOG(NORMAL , "用户在搜索：" + word);    
        
        //根据关键字，构建json串
        std::string json_string;    
        search.Search(word, &json_string);

        //设置 get "s" 请求返回的内容，返回的是根据关键字，构建json串内容
        rsp.set_content(json_string, "application/json");  
    });


    //std::cout << "服务器启动成功......" << std::endl; 
    LOG(NORMAL , "服务器启动成功......");    
    // 绑定端口（8080），启动监听（0.0.0.0表示监听任意端口）
    server.listen("0.0.0.0", 8080);                                                                                                                                                      

    return 0;
}