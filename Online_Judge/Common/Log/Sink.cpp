#include "Sink.hpp"

ns_clog::FileSink::FileSink(const std::string& pathname) 
:PathName(pathname)
{
    //对于使用这个类我们需要先创建一个文件，然后再打开这个文件
    ns_clog::Util::File::CreateDirector(ns_clog::Util::File::Path(PathName));

    //打开文件(二进制的追加模式)
    ofs.open(PathName , std::ios::binary | std::ios::app);          //这里如果文件不存在会创建文件，如果存在则会在在原文件后面追加新内容
    assert(ofs.is_open());
    //std::cout << "创建成功" << std::endl;
}

void ns_clog::FileSink::log(const char *data , int len)  
{
    ofs.write(data , len);
    if(!ofs.good())
    {
        std::cout << "日志输出到文件失败" << std::endl;
    }
    ofs.flush();
}

ns_clog::RollSink::RollSink(const std::string& basename , int max_fsize)
:BaseName(basename)
,MAX_FILE_SIZE(max_fsize)
,Cur_File_Size(0)
,Name_Count(0)
{
    //获取文件名
    std::string filename = CreateNewFile();

    //和之前创建普通文件一样
    ns_clog::Util::File::CreateDirector(ns_clog::Util::File::Path(filename));
    ofs.open(filename , std::ios::binary | std::ios::app);

    assert(ofs.is_open());
    //std::cout << "创建成功" << std::endl;
}

void ns_clog::RollSink::log(const char *data , int len)
{
    if(Cur_File_Size >= MAX_FILE_SIZE)
    {
        //如果当前文件过于大了，那么在创建一个
        ofs.close();                    //关闭旧文件防止内存泄漏
        std::string filename = CreateNewFile();
        ofs.open(filename , std::ios::binary | std::ios::app);
        assert(ofs.is_open());
    }

    ofs.write(data , len);
    if(!ofs.good())
    {
        std::cout << "滚动文件输出失败" << std::endl;
    }

    Cur_File_Size += len;
}

std::string ns_clog::RollSink::CreateNewFile()
{
    //获取系统事件
    time_t t = ns_clog::Util::Data::now();
    struct tm m;
    //储存时间
    localtime_r(&t , &m);

    //组装文件名
    std::stringstream ss;
    ss << BaseName;
    ss << m.tm_year + 1900;
    ss << m.tm_mon + 1;
    ss << m.tm_mday;
    ss << m.tm_hour;
    ss << m.tm_min;
    ss << m.tm_sec;
    ss << "-";
    ss << Name_Count++;
    ss << ".log";

    return ss.str();
}