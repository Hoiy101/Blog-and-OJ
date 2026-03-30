#ifndef DOWNLOAD_HPP
#define DOWNLOAD_HPP

#include "../Common/Log/clog.hpp"
#include "../Common/Util/Tool.hpp"

#include <string>
#include <fstream>
#include <json/json.h>

//大概三个函数GetFilePath为获取文件的主路径
//

namespace ns_file_transfer
{   
    inline const char *download_logger_name = "download_logger";

    class Download
    {
    public:
        Download();
        ~Download();
    
    public:
        //获取文件路径(两个json)
        bool GetFilePath(const std::string& in_json , std::string* out_json);
        bool GetInputFilePath(const std::string& question_number , const std::string& test_number);
        bool GetOutputFilePath(const std::string& question_number , const std::string& test_number);

    private:
        bool CheckFile(const std::string& File_Path);

    private:
        std::string input_file_path;
        std::string output_file_path;

    };
    
}

#endif