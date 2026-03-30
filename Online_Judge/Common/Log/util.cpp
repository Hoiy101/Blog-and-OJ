#include "util.hpp"

size_t ns_clog::Util::Data::now()
{
    return (size_t)time(nullptr);
}

bool ns_clog::Util::File::exists(const std::string& Path)
{
    struct stat st;
    //stat函数表示:系统调用查询文件/目录的状态信息
    return (stat(Path.c_str() , &st) == 0);
}

//获取文件路径
std::string ns_clog::Util::File::Path(const std::string& PathName)
{
    size_t pos = PathName.find_last_of("/\\");
    if(pos == std::string::npos)
    {
        //如果没有找到
        return ".";
    }
    //返回相应的路径
    return PathName.substr(0 , pos + 1);
}

//递归创建多级目录
void ns_clog::Util::File::CreateDirector(const std::string& PathName)
{
    size_t pos = 0 , idx = 0;
    while(idx < PathName.size())
    {
        pos = PathName.find_first_of("/\\" , idx);

        //最后一次目录
        if(pos == std::string::npos)
        {
            mkdir(PathName.c_str() , 0777);
            break;
        }
        std::string father_dir = PathName.substr(0 , pos + 1);

        //有这个的话不创建，来跳过
        if(exists(father_dir))
        {
            idx = pos + 1;
        }
        mkdir(father_dir.c_str() , 0777);
        idx = pos + 1;
    }
}