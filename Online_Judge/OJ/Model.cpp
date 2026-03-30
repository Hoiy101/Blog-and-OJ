#include "Model.hpp"


ns_model::Model::Model()
{
    assert(LoadQuestionList(questions_list));
}

ns_model::Model::~Model()
{

}

bool ns_model::Model::LoadQuestionList(const std::string& question_list)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(model_logger_name1);

    // 加载配置文件：questions/questions.list + 题目编号文件
    std::ifstream in(question_list);
    if(!in.is_open())
    {
        logger->fatal("打开题库文件失败 , 请检查题库文件");
        return false;
    }

    //读取题目
    std::string line;
    while(std::getline(in , line))
    {
        //分割后字符串储存的地方
        std::vector<std::string>tokens;

        //将题目描述进行分割
        ns_util::StringUtil::SplitString(line , &tokens , " ");
        //每一个题目都分为:编号，题目，难度，时间限制，空间限制
        if(tokens.size() != 5)
        {
            logger->warn("获取题目失败,请检查文件格式,题目编号为:%s" , tokens[0].c_str());
            continue;
        }

        //填充结构体
        Question q;
        q.num = tokens[0];
        q.title = tokens[1];
        q.star = tokens[2];
        q.cpu_limit = atoi(tokens[3].c_str());
        q.mem_limit = atoi(tokens[4].c_str());

        //指定文件的路径
        std::string path = question_path;
        path += q.num;
        path += "/";

        //继续填充结构体
        ns_util::FileUtil::ReadFile(path + "desc.txt" , &(q.desc) , true);
        ns_util::FileUtil::ReadFile(path + "header.cpp" , &(q.header) , true);
        ns_util::FileUtil::ReadFile(path + "tail.cpp" , &(q.tail) , true);

        //插入到哈希表中
        questions.insert({q.num , q});
    }

    logger->info("加载题库成功");
    in.close();

    return true;
}

bool ns_model::Model::GetAllQuestion(std::vector<Question>* out)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(model_logger_name1);

    if(questions.size() == 0)
    {
        logger->Error("获取题目失败");
        return false;
    }

    for(auto& q : questions)
    {
        out->push_back(q.second);
    }
    return true;
}

bool ns_model::Model::GetOneQuestion(const std::string& num , Question* q)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(model_logger_name1);

    const auto& iter = questions.find(num);
    if(iter == questions.end())
    {
        logger->Error("获取题目失败,题目编号为%d",num);
        return false;
    }

    (*q) = iter->second;
    return true;
}