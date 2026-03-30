#ifndef MODEL_HPP
#define MODEL_HPP

/*
    题库的文件版本
*/

#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

#include "../Common/Log/clog.hpp"
#include "../Common/Util/Tool.hpp"

namespace ns_model
{
    inline const char * model_logger_name1 = "model_logger1";

    //题目的结构体
    struct Question
    {
        std::string num;        //唯一编号
        std::string title;      //题目
        std::string star;       //题目难度
        int cpu_limit;          //时间限制
        int mem_limit;          //内存限制(以kb为标准)
        std::string desc;       //题目描述
        std::string header;     //预留代码
        std::string tail;       //测试用例 
    };
    
    const std::string question_path = "./Questions/";
    const std::string questions_list = "./Questions/questions.list";

    class Model
    {   
    public:

        Model();
        ~Model();
        //加载题目的函数
        bool LoadQuestionList(const std::string& question_list);
        //获取题目
        bool GetAllQuestion(std::vector<Question>* out);
        bool GetOneQuestion(const std::string& num , Question* q);

    private:

        //题目的哈希对应
        std::unordered_map<std::string , Question> questions;
    };
    

};

#endif