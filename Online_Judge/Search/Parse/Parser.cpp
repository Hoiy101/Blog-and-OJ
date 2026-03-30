#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

#include "Util.hpp"

//这个用来解析html文件，把html文件里有用的东西解析出来

const std::string input = "/home/zz/project/Boost_Searcher/Code/data/input";
const std::string output = "/home/zz/project/Boost_Searcher/Code/data/raw_html/raw.txt";

//定义文件信息的结构体
typedef struct DocInfo
{
    //需要存储的信息
    std::string title;
    std::string context;
    std::string url;
}DocInfo_t;

//我们这个程序是为了解析所有html文件，把有用的信息提取出来，存储到一个文本文件里，但是嘞我们这个目录中有一些文件夹我们需要知道这个文件夹的路径
//1.我们需要先收集所有html的路径
//2.解析html文件，收集有用的信息
//3.保存信息

//收集路径信息
bool EnumFile(const std::string& input_path , std::vector<std::string>* Files_path);

//解析html信息并存入vector
bool ParseHtml(const std::vector<std::string>& Files_path , std::vector<DocInfo_t> *Result);

//把vector内的内容写入output里面
bool SaveHtml(const std::vector<DocInfo_t>& Result , const std::string& output);

int main(int argc , const char *argv[])
{
    std::vector<std::string>Files_Path;

    //获取html文件的地址
    if(!EnumFile(input , &Files_Path))
    {
        std::cerr << "Enum file name error!" << std::endl;
        return 1;
    }

    std::cout << "EnumFile success!" << std::endl;

    std::vector<DocInfo_t> Results;
    
    //解析HTML文件
    if(!ParseHtml(Files_Path , &Results))
    {
        std::cerr << "Parse Html error!" << std::endl;
        return 2;
    }

    std::cout << "ParseHtml success!" << std::endl;

    //利用IO操作写入.txt文件中
    if(!SaveHtml(Results , output))
    {
        std::cerr << "Save Html error!" << std::endl;
        return 3;
    }

    std::cout << "Hello , World!" << std::endl;

    return 0;
}

//把每个.html文件包括它们的路径写入数组中
bool EnumFile(const std::string& input_path , std::vector<std::string>* Files_path)
{
    namespace fs = boost::filesystem;
    //定义一个path对象用来递归搜索html文件
    fs::path root_path(input_path);

    if(!fs::exists(root_path))
    {
        //表示这个路径不存在
        std::cerr << "input path error!" << std::endl;
        return false;
    }

    //对当前文件路径进行遍历
    //这个玩意儿是个递归目录迭代器
    fs::recursive_directory_iterator end;               //定义一个空的迭代器来判断是否已经借宿
    for(fs::recursive_directory_iterator iter(root_path); iter != end; iter++)
    {
        //判断当前遍历到的是不是一个文件
        if(!fs::is_regular_file(*iter))
        {
            continue;
        }

        //如果这个文件是普通文件，那么它是不是我们需要的哪种html文件呢？
        //判断当前文件是不是.html结尾
        if(iter->path().extension() != ".html")
        {
            continue;
        }

        //走到这里肯定是个.html文件，把它的路径存入vector中
        Files_path->push_back(iter->path().string());
    }

    return true;
}

//解析title
static bool ParseTitle(const std::string& files , std::string* title)
{
    //先查找<title>标签的位置
    std::size_t begin = files.find("<title>");
    if(begin == std::string::npos)
    {
        return false;
    }
    
    // 找到</title>标签的位置
    std::size_t end = files.find("</title>");
    if(end == std::string::npos)
    {
        return false;
    }

    //调整begin的位置，指向标签内容的开始位置
    begin += std::string("<title>").size(); 

    if(begin > end)
    {
        return false;
    }

    *title = files.substr(begin, end - begin);
    return true;
}

//去标签
static bool ParseContent(const std::string& files , std::string* content)
{
    //我们这里搞一个简单的状态机来解析这个文件
    enum status
    {
        LABEL,        //表示当前在标签内
        CONTENT      //表示当前在标签外
    };

    enum status s = LABEL;         //开始会遇见"<"这样的内容，所有我们要初始化为LABEL状态
    for(auto c : files)
    {
        //这里来检查状态
        switch(s)
        {
        case LABEL:
            if(c == '>') s = CONTENT;
            break;
        case CONTENT:
            if(c == '<') s = LABEL;
            else
            {
                // 我们不想保留原始文件中的\n，因为我们想用\n作为html解析之后的文本的分隔符
                if(c == '\n') c = ' ';
                content->push_back(c);
            }
            break;
        default:
            break;
        }
    }
    
    return true;
}

//解析URL,我们要说明一下为什么要这样解析，因为我们访问的网址是类似https://www.boost.org/doc/libs/1_90_0/doc/html/thread.html
//这样的,而我们要提去类似于thread.html这样的内容
////构建官网url :url_head + url_tail
static bool ParseUrl(const std::string& file_path , std::string* url)
{
    std::string url_path = "https://www.boost.org/doc/libs/1_90_0/doc/html";
    //接下来，我们要从file_path中提取出类似thread.html这样的内容
    std::string url_tail = file_path.substr(input.size());
    //这里为什么要用size，要从我们的项目结构来说，我们文件目录前面有些固定字段，用这个就是为了把这些固定字段消除掉
    *url = url_path + url_tail;
    return true;
}

static void ShowDoc(const DocInfo_t& doc)
{
    std::cout << "title: " << doc.title << std::endl;
    std::cout << "content: " << doc.context << std::endl;
    std::cout << "url: " << doc.url << std::endl;
}

//用来解析Html文件
bool ParseHtml(const std::vector<std::string>& Files_path , std::vector<DocInfo_t> *Result)
{
    //遍历所有文件路径
    for(auto File_path : Files_path)
    {
        //1.先读取文件
        std::string result;
        if(!util::FileUtil::ReadFile(File_path , &result))
        {
            continue;
        }

        //解析指定的文件内容,提取相应的信息
        DocInfo_t doc;
        if(!ParseTitle(result , &doc.title))
        {
            continue;
        }

        if(!ParseContent(result , &doc.context))
        {
            continue;
        }

        //构建URL路径
        if(!ParseUrl(File_path , &doc.url))
        {
            continue;
        }

        //完成解析，放入容器
        Result->push_back(std::move(doc));
    }

    return true;
}

//写入磁盘
bool SaveHtml(const std::vector<DocInfo_t>& Result , const std::string& output)
{
    #define SEP '\3'      //定义分隔符,我们这里用自定义分隔符方便使用getline来一次读取完整

    //打开文件，并且以二进制的方式来储存，方便我们保存
    std::ofstream out(output , std::ios::out | std::ios::binary);
    if(!out.is_open())
    {
        std::cerr << "open file " << output << " failed!" << std::endl;
        return false;
    }

    //开始写入文件
    for(const auto& doc : Result)
    {
        //构造string写入文件
        std::string out_string;
        out_string = doc.title;
        out_string += SEP;
        out_string += doc.context;
        out_string += SEP;
        out_string += doc.url;
        out_string += '\n';        
        //每条记录占一行
        out.write(out_string.c_str() , out_string.size());
    }

    return true;
}
