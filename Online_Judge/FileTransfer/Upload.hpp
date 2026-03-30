#ifndef UPLOAD_HPP
#define UPLOAD_HPP

//首先要创建目录(mkdir)，其次要规范文件命名固定为.in和.out文件(这个要怎么做?)
//用户点击上传，我们这里自动分配题号

#include <drogon/drogon.h>
#include <json/json.h>
#include <filesystem>
#include <iostream>
#include <string>

#include "../Common/Log/clog.hpp"

namespace ns_file_transfer
{
    inline const char *upload_logger_name = "upload_logger";

    enum class FileType
    {
        input = 0,
        output = 1
    };

    //在构造函数中获取最大的题号
    class Upload
    {
    public:
        Upload(int number);
        ~Upload();
    
    public:
        void handleUploadRequest(const drogon::HttpRequestPtr& req , std::string* out_json);

    private:
        //根据后缀发配相应文件夹
        static FileType GetFileType(const std::string& file_name);
        //创建两个文件夹
        bool CreateDirectory(const std::string& dir_path);
        //根据文件的后缀不同来上传文件夹
        bool SaveFile(const drogon::HttpFile& file , FileType file_type);

    private:
        static int Max_Question_Number;
        //基础目录路径:到达Data文件夹即可
        std::string base_dir_path;

    };

};

#endif