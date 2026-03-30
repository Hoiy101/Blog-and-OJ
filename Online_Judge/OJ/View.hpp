#ifndef VIWE_HPP
#define VIWE_HPP

#include <ctemplate/template.h>
#include <iostream>
#include <string>
#include <vector>

#include "oj_Model.hpp"

const std::string template_path = "/home/zz/Desktop/project/Online_Judge/OJ/template_html/";       //渲染的路径

namespace ns_view
{
    class View
    {
    public:
        View();
        ~View();
    
        //将所有题目数据构成网页
        void AllExpandHtml(const std::vector<ns_model::Question> &questions , std::string* html);
        
        //构建单个题目的网页
        void OneExpand(const ns_model::Question &q , std::string* html);
    };
};

#endif