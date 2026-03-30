#include "View.hpp"

ns_view::View::View()
{

}

ns_view::View::~View()
{

}

void ns_view::View::AllExpandHtml(const std::vector<ns_model::Question> &questions , std::string* html)
{
    //菜单显示的题目类型为:题目编号 题目 难度
    std::string src_html = template_path + "all_questions.html";

    //形成数据字典(类似于map的键值对)
    ctemplate::TemplateDictionary root("all_questions");

    for(const auto& q : questions)
    {
        ctemplate::TemplateDictionary* sub = root.AddSectionDictionary("question_list");        //忘里面加入一个节字典(总体类似于一个表格)
        sub->SetValue("number" , q.number);
        sub->SetValue("title" , q.title);
        sub->SetValue("star" , q.star);       
    }

    //获取渲染好的网页
    ctemplate::Template* tpl = ctemplate::Template::GetTemplate(src_html , ctemplate::DO_NOT_STRIP);

    //完成渲染功能
    tpl->Expand(html , &root);
}

void ns_view::View::OneExpand(const ns_model::Question &q , std::string* html)
{
    std::string src_html = template_path + "one_question.html";

    //同上
    ctemplate::TemplateDictionary root("one_questions");

    root.SetValue("number" , q.number);
    root.SetValue("title" , q.title);
    root.SetValue("star" , q.star);
    root.SetValue("desc", q.desc);
    //root.SetValue("pre_code", q.header);

    ctemplate::Template* tpl = ctemplate::Template::GetTemplate(src_html , ctemplate::DO_NOT_STRIP);
    tpl->Expand(html , &root);
}
