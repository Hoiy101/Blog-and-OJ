#include "Upload.hpp"

//构造函数中获取最大的题号和基础信息
ns_file_transfer::Upload::Upload(int number)
{
    Max_Question_Number = number;

}

ns_file_transfer::Upload::~Upload()
{

}

//处理上传请求主业务，其中json就一个message字段
void ns_file_transfer::Upload::handleUploadRequest(const drogon::HttpRequestPtr& req , std::string* out_json)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(upload_logger_name);

    //每次只能上传一道题(后续实现批量上传)
    Max_Question_Number += 1;
    logger->debug("当前最大题号为:{%d}" , Max_Question_Number);

    drogon::MultiPartParser parser;
    Json::StyledWriter writer;
    Json::Value in_json;

    //解析请求
    int result = parser.parse(req);
    if (result != 0) 
    {
        // 解析失败处理
        in_json["message"] = "解析请求失败";
        *out_json = writer.write(in_json);

        logger->warn("解析请求失败");
        return;
    }

    //获取上传文件的列表   
    const auto& files = parser.getFiles();
    if (files.empty()) 
    {
        in_json["message"] = "没有上传文件";
        *out_json = writer.write(in_json);
        logger->warn("没有上传文件");
        return;
    }

    //创建相应的文件目录
    if(!CreateDirectory(base_dir_path + "input" + std::to_string(Max_Question_Number))
            || !CreateDirectory(base_dir_path + "output" + std::to_string(Max_Question_Number)))
    {
        //创建文件夹失败
        in_json["message"] = "后端创建文件夹失败,请联系管理员";
        *out_json = writer.write(in_json);
        logger->warn("创建文件夹失败");
        return;
    }
    //遍历文件列表并保存文件
    for(const auto& file : files)
    {
        //获取文件类型
        std::string filename = file.getFileName();
        FileType type = GetFileType(filename);
        
        //保存文件
        if(!SaveFile(file , type))
        {
            in_json["message"] = in_json["message"].asString() + "保存" + filename + "失败" + "\n";
            *out_json = writer.write(in_json);
            logger->warn("保存文件失败%s",filename.c_str());
        }
    }

    logger->debug("上传文件成功");
    //返回成功信息
    if(!in_json["message"].isString())
        in_json["message"] = "上传文件成功";
    *out_json = writer.write(in_json);
}

//提取文件后缀名
ns_file_transfer::FileType ns_file_transfer::Upload::GetFileType(const std::string& file_name)
{
    if(file_name.find(".in") != std::string::npos)
    {
        return FileType::input;
    }
    else if(file_name.find(".out") != std::string::npos)
    {
        return FileType::output;
    }

    return FileType::input;
}

//根据最大题号在两个文件里面创建文件
bool ns_file_transfer::Upload::CreateDirectory(const std::string& dir_path)
{
    if(!std::filesystem::exists(dir_path))
    {
        std::filesystem::create_directory(dir_path);
        return true;
    }

    return false;
}

//保存文件到本地
bool ns_file_transfer::Upload::SaveFile(const drogon::HttpFile& file , FileType file_type)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(upload_logger_name);

    if(file_type == FileType::input)
    {
        file.saveAs(base_dir_path + "input" + "/" + std::to_string(Max_Question_Number) + "/" + file.getFileName());
        return true;
    }
    else if(file_type == FileType::output)
    {
        file.saveAs(base_dir_path + "output" + std::to_string(Max_Question_Number) + "/" + file.getFileName());
        return true;
    }

    logger->warn("文件类型错误:%s",file.getFileName().c_str());
    
    return false;
}
