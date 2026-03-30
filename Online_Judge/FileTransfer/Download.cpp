#include "Download.hpp"

ns_file_transfer::Download::Download()
{

}

ns_file_transfer::Download::~Download()
{

}

//检查文件是否存在
bool ns_file_transfer::Download::CheckFile(const std::string& File_Path)
{
    return ns_util::FileUtil::IsFileExist(File_Path);
}

bool ns_file_transfer::Download::GetFilePath(const std::string& in_json , std::string* out_json)
{
    bool st = true;
    //这个函数需要先接受前端传来的JSON,然后根据JSON中的数据来构造文件路径json预期可看项目中example.txt
    Json::Value in_value;
    Json::Reader reader;
    reader.parse(in_json , in_value);

    //提取数据
    std::string number = in_value["number"].asString();
    std::string test_number = in_value["test_number"].asString();

    Json::Value out_value;
    //根据数据构造文件路径
    if(!GetInputFilePath(number , test_number))
    {
        if(out_value["message"].isString())
            out_value["message"] = out_value["message"].asString() + "输入文件路径不存在";
        else
            out_value["message"] = "输入文件路径不存在";
        st = false;
    }
    if(!GetOutputFilePath(number , test_number))
    {
        if(out_value["message"].isString())
            out_value["message"] = out_value["message"].asString() + "输出文件路径不存在";
        else
            out_value["message"] = "输出文件路径不存在";
        st = false;
    }

    if(!out_value["message"].isString())
        out_value["message"] = "已成功插入文件地址";
    Json::StyledWriter writer;
    *out_json = writer.write(out_value);

    return st;
}

bool ns_file_transfer::Download::GetInputFilePath(const std::string& question_number , const std::string& test_number)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(download_logger_name);

    input_file_path = question_number + "/" + test_number;
    if(!CheckFile(ns_util::PathUtil::Stdin(input_file_path)))
    {
        logger->Error("输入文件路径:%s不存在" , input_file_path.c_str());
        return false;
    }
    
    input_file_path = ns_util::PathUtil::Stdin(input_file_path);
    return true;
}

bool ns_file_transfer::Download::GetOutputFilePath(const std::string& question_number , const std::string& test_number)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(download_logger_name);

    output_file_path = question_number + "/" + test_number;
    if(!CheckFile(ns_util::PathUtil::Stdout(output_file_path)))
    {
        logger->Error("输出文件路径:%s不存在" , output_file_path.c_str());
        return false;
    }
    
    output_file_path = ns_util::PathUtil::Stdout(output_file_path);
    return true;
}