#include "oj_Model.hpp"

ns_model::Model::Model()
{

}

ns_model::Model::~Model()
{

}

bool ns_model::Model::QueryMySql(const std::string &sql, vector<Question> *out)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(model_logger_name);

    // 创建MySQL句柄
    MYSQL *my = mysql_init(nullptr);

    // 连接数据库
    if (nullptr == mysql_real_connect(my, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0))
    {
        // 如果连接失败
        logger->fatal("连接数据库失败\n");
        return false;
    }
    
    // 一定要设置该链接的编码格式，要不然会出现乱码问题
    mysql_set_character_set(my, "utf8");

    logger->info("查询操作连接数据库成功");

    // 执行sql语句
    if (0 != mysql_query(my, sql.c_str()))
    {
        // 如果执行失败
        //LOG(WARNING) << sql << " execute error!" << "\n";
        logger->warn("%s execute error" , sql.c_str());
        return false;
    }

    //logger->debug("%s execute success" , sql.c_str());
    // 提取结果
    MYSQL_RES * res = mysql_store_result(my);

    // 分析结果
    int rows = mysql_num_rows(res);   // 获得行数
    int cols = mysql_num_fields(res); // 获得列数

    // 获得每行每列的数据
    for (int i = 0; i < rows; ++i)
    {
        MYSQL_ROW row = mysql_fetch_row(res); // 拿到当前这一行的所有数据（这里的row是一个二级指针）
        Question q; // 用于保存结果

        // 拿到当前行，每列的所有数据
        q.number = row[0];

        q.test_count = atoi(row[1]);
        q.title = row[2];
        q.desc = row[3];
        q.star = row[4];
        q.cpu_limit = atoi(row[5]);
        q.mem_limit = atoi(row[6]);
        
        // 将当前题的所有信息放到返回数组里面
        out ->push_back(q);
    }

    // 释放结果空间
    //free(res);
    mysql_free_result(res);
    // 关闭MySQL连接
    mysql_close(my);

    return true;
}

bool ns_model::Model::GetAllQuestions(vector<Question> *out)
{
    std::string sql = "select * from ";
    sql += oj_questions;

    return QueryMySql(sql, out);
}

bool ns_model::Model::GetAllBlogPosts(vector<BlogPost> *out)
{
    std::string sql = "select * from ";
    sql += blog;


}

bool ns_model::Model::InsertOneQuestion(const Question& q)
{


    return true;
}

bool ns_model::Model::GetOneQuestion(const std::string &number , Question *q)
{
    bool res = false;

    std::string sql = "select * from ";
    sql += oj_questions;
    sql += " where id=";
    sql += number;

    vector<Question> result;
    if (QueryMySql(sql, &result)) // 判断是否获取题目成功
    {
        // 判断获取的题目个数是否只有1个
        if (result.size() == 1) 
        {
            *q = result[0];
            res = true;
        }
    }

    return res;
}

bool ns_model::Model::GetOneBlogPost(const int id, BlogPost *bp)
{


    return true;
}

bool ns_model::Model::InsertOneBlogPost(const BlogPost& bp)
{


    return true;
}

//获取最大的id号
int ns_model::Model::GetQuestionMaxID()
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(model_logger_name);

    //准备数据阶段
    int max_id = 0;
    std::string sql = "select * from Ques where id = (select max(id) from Ques);";

    // 创建MySQL句柄
    MYSQL *my = mysql_init(nullptr);
    if(nullptr == mysql_real_connect(my , host.c_str() , user.c_str() , passwd.c_str() , db.c_str() , port , nullptr , 0))
    {
        logger->fatal("连接数据库失败");
        return false;
    }

    // 执行sql语句
    if(mysql_query(my , sql.c_str()) != 0)
    {
        logger->warn("%s execute error" , sql.c_str());
        return false;
    }

    //获取结果集
    MYSQL_RES* res = mysql_store_result(my);
    MYSQL_ROW row = mysql_fetch_row(res);

    if(row == nullptr)
    {
        logger->warn("get max id error");
        return false;
    }

    max_id = atoi(row[0]);

    return true;
}

int ns_model::Model::GetBlogPostMaxID()
{


    return 0;
}