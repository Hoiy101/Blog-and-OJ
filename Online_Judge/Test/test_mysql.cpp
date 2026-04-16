#include <ctime>
#include <vector>
#include <sstream>
#include <iostream>
#include <mysql/mysql.h>
#include <boost/algorithm/string.hpp>

struct BlogPost
{
    int id; // 博文的id
    std::string title; // 博文的标题
    std::string desc; // 博文的描述
    std::string content; // 博文的内容
    time_t time; // 博文的发布时间
};

void stringToTime(const std::string &text, time_t &time)
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
        return;
    }

    int year, month, day, hour, minute, second;

    year = atoi(Data_arr[0].c_str());
    month = atoi(Data_arr[1].c_str());
    day = atoi(Data_arr[2].c_str());
    hour = atoi(Time_arr[0].c_str());
    minute = atoi(Time_arr[1].c_str());
    second = atoi(Time_arr[2].c_str());

    std::cout << "year = " << year << " , month = " << month << " , day = " << day << std::endl;
    std::cout << "hour = " << hour << " , minute = " << minute << " , second = " << second << std::endl;

    std::tm tm;
    tm.tm_year = year;
    tm.tm_mon = month;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    tm.tm_isdst = -1;
    time = mktime(&tm);

}

int main(void)
{

    // 创建MySQL句柄
    MYSQL *my = mysql_init(nullptr);

    // 连接数据库
    if (nullptr == mysql_real_connect(my, "60.205.125.90", "bao", "18026472002p", "bao" , 3307, nullptr, 0))
    {

        std::cout << "连接数据库失败" << std::endl;
        return 1;
    }
    
    // 一定要设置该链接的编码格式，要不然会出现乱码问题
    mysql_set_character_set(my, "utf8");

    std::cout << "连接成功" << std::endl;
    const std::string sql = "select * from blog";

    // 执行sql语句
    if (0 != mysql_query(my, sql.c_str()))
    {
        // 如果执行失败
        //LOG(WARNING) << sql << " execute error!" << "\n";
        
        std::cout << "执行失败" << std::endl;
        return 1;
    }

    //logger->debug("%s execute success" , sql.c_str());
    // 提取结果
    MYSQL_RES * res = mysql_store_result(my);

    // 分析结果
    int rows = mysql_num_rows(res);   // 获得行数
    int cols = mysql_num_fields(res); // 获得列数

    std::vector<BlogPost> out;
    out.reserve(rows);

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
        stringToTime(text, bp.time);
        // 将当前博文的所有信息放到返回数组里面
        out.push_back(bp);
    }

    // 释放结果空间
    //free(res);
    mysql_free_result(res);
    // 关闭MySQL连接
    mysql_close(my);


}