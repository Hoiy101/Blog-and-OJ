#ifndef OJ_MODEL_HPP
#define OJ_MODEL_HPP

// MySQL版本
// 根据题目list文件，加载所有的题目信息到内存中
// model：主要用来和数据进行交互，对外提供访问数据的接口

#include "../Common/Log/clog.hpp"
#include "../Common/Util/Tool.hpp"

#include <ctime>
#include <string>
#include <vector>
#include <future>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <mysql/mysql.h>
//上个模块的数据库版本

namespace ns_model
{
    inline const char * model_logger_name = "model_logger";

    using namespace std;
    using namespace ns_util;

    struct Question
    {
        string number; // 题目编号（唯一）
        string title;  // 题目的标题
        string star;   // 题目的难度：简单 中等 困难
        string desc;   // 题目的描述
        int test_count;// 题目的测试用例数量
        int cpu_limit; // 题目的时间要求（S）
        int mem_limit; // 题目的空间要求（KB）
    };

    struct BlogPost
    {
        int id; // 博文的id
        string title; // 博文的标题
        std::string desc; // 博文的描述
        string content; // 博文的内容
        time_t time; // 博文的发布时间
    };

    const std::string blog = "blog";                          // 要访问的表名
    const std::string oj_questions = "Ques";                 // 要访问的表名
    const std::string host = "60.205.125.90";               // ip为本地服务器
    const std::string user = "bao";                        // MySQL用户名
    const std::string passwd = "18026472002p";            // MySQL密码
    const std::string db = "bao";                        // 要连接的数据库名
    const int port = 3307;                              // MySQL的端口号

    class Model
    {
    public:
        Model();

        // 查询MySQL
        // 参数：sql：sql查询语句，out：输出查询结果
        bool QueryMySql(const std::string &sql, vector<Question> *out);
        bool QueryMySql(const std::string &sql, vector<BlogPost> *out);

        // 获取所有的题目
        bool GetAllQuestions(vector<Question> *out);
        // 获取所有的博文
        bool GetAllBlogPosts(vector<BlogPost> *out);

        // 获取单个题目
        bool GetOneQuestion(const string &number, Question *q);
        bool GetOneBlogPost(const int id, BlogPost *bp);

        //插入数据
        bool InsertOneQuestion(const Question &q);
        bool InsertOneBlogPost(const BlogPost &bp);
        

        //获得最大的ID号
        int GetQuestionMaxID();
        int GetBlogPostMaxID();

        ~Model();

    private:
        bool StringToTime(const std::string &text, time_t &time);

    };
}

#endif