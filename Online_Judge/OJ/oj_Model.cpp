#include "oj_Model.hpp"

ns_model::Model::Model()
{
    // 初始化MySQL库
    if (mysql_library_init(0, NULL, NULL) != 0) 
    {
        ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(model_logger_name);
        logger->fatal("MySQL library initialization failed");
    }
}

ns_model::Model::~Model()
{
    // 清理MySQL库
    mysql_library_end();
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

bool ns_model::Model::QueryMySql(const std::string& sql , std::vector<BlogPost> *out)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(model_logger_name);
    // logger->debug("DataBase test begin");

    // 创建MySQL句柄
    MYSQL *my = mysql_init(nullptr);
    if(my == nullptr)
    {
        std::cerr << "error\n";
        return false;
    }
    // 连接数据库
    if (nullptr == mysql_real_connect(my, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0))
    {
        // 如果连接失败
        logger->fatal("连接数据库失败\n");
        return false;
    }
    
    std::cerr << "Second test point apperove\n";
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

    logger->debug("%s execute success" , sql.c_str());
    //提取结果
    MYSQL_RES * res = mysql_store_result(my);
        if (res == nullptr) {
        logger->Error("mysql_store_result failed: %s", mysql_error(my));
        mysql_close(my);
        return false;
    }
    
    // 分析结果
    int rows = mysql_num_rows(res);   // 获得行数
    int cols = mysql_num_fields(res); // 获得列数

    // 获得每行每列的数据
    for (int i = 0; i < rows; ++i)
    {
        MYSQL_ROW row = mysql_fetch_row(res); // 拿到当前这一行的所有数据（这里的row是一个二级指针）
        BlogPost bp; // 用于保存结果

        // 拿到当前行，每列的所有数据

        bp.id = atoi(row[0]);
        bp.title = row[2];
        bp.desc = row[3];
        bp.content = row[4];

        std::string text = row[5];
        StringToTime(text, bp.time);
        // 将当前博文的所有信息放到返回数组里面
        out->push_back(bp);
    }

    // 释放结果空间
    //free(res);
    mysql_free_result(res);
    // 关闭MySQL连接
    mysql_close(my);

    // std::cerr << "Third test point apperove\n";
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
    std::cerr << "GetBlog test\n";

    std::string sql = "select * from ";
    sql += blog;

    return QueryMySql(sql, out);
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

//这里以后可以用模版元编程来实现
bool ns_model::Model::GetOneBlogPost(const int id, BlogPost *bp)
{
    //把上面的代码复制粘贴

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

bool ns_model::Model::StringToTime(const std::string &text, time_t &time)
{
    std::string Date , Time;
    std::vector<std::string>Data_arr , Time_arr;
    size_t pos = text.find(' ');
    Date = text.substr(0, pos);
    Time = text.substr(pos + 1);

    boost::algorithm::trim(Date);
    boost::algorithm::trim(Time);

    boost::split(Data_arr , Date, boost::is_any_of("-") , boost::algorithm::token_compress_on);
    boost::split(Time_arr , Time , boost::is_any_of(":") , boost::algorithm::token_compress_on);
    
    if(Data_arr.size() != 3 || Time_arr.size() != 3)
    {
        std::cout << "日期格式错误" << std::endl;
        return false;
    }

    int year, month, day, hour, minute, second;

    year = atoi(Data_arr[0].c_str());
    month = atoi(Data_arr[1].c_str());
    day = atoi(Data_arr[2].c_str());
    hour = atoi(Time_arr[0].c_str());
    minute = atoi(Time_arr[1].c_str());
    second = atoi(Time_arr[2].c_str());

    // std::cout << "year = " << year << " , month = " << month << " , day = " << day << std::endl;
    // std::cout << "hour = " << hour << " , minute = " << minute << " , second = " << second << std::endl;

    std::tm tm;
    tm.tm_year = year;
    tm.tm_mon = month;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    tm.tm_isdst = -1;
    time = mktime(&tm);

    return true;
}