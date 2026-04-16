#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <mutex>

#include "../../OJ/oj_Model.hpp"
#include "../../Common/Util/Tool.hpp"
#include "../../Common/Log/clog.hpp"

//这里的代码需要重构一下
//1.记录当前最大的博文id号码
//2.每隔一段时间来更新索引


namespace ns_Index 
{
    inline const char *Server_logger_name = "server_model_logger";

    struct DocInfo
    {
        uint64_t doc_id;        //这个是文档的唯一标识符
        std::string title;      //文档的标题
        std::string content;    //文档的内容
    };

    //倒排元素对应的节点
    struct InvertedElem
    {
        uint64_t doc_id;     //文档ID,同时也与DocInfo中的doc_id对应
        std::string word;    //关键词
        int weight;          //关键词在该文档中的权重
    };

    //一个关键词可能会出现在多个文档中，所以我们要构造一组数组然后根据权重一次排列到用户看到的内容中

    typedef std::vector<InvertedElem> InvertedList;     //倒排索引，一个关键词对应着一组数据

    class Index
    {
    private:
        //这是我们的索引类，对于这个类我们要建立正向索引和倒排索引
        //正向索引采用文档ID，这样可以对应着一个文件内容
        //倒排索引采用关键词，这样可以对应着一组文档ID和权重

        std::vector<DocInfo> Forward_Index;                                 //正向索引，文档ID对应文档信息
        std::unordered_map<std::string , InvertedList> Inverted_Index;      //倒排索引，关键词对应倒排列表

        //我们这个索引类对于一次搜索只需要一个类而且是相同的，那么我们使用单例模式
    private:
        
        Index() {}
        Index(const Index&) = delete;
        Index& operator=(const Index&) = delete;

        //这里使用的赖汉式，双重检查实现的单例模式
        static Index* instance;
        static std::mutex mtx;

    public:

        ~Index() {}
        static Index* Getinstance();

        //获取索引对应的信息  获取关键词对应的倒排索引
        DocInfo* GetForwardIndex(uint64_t doc_id);
        InvertedList* GetInvertedList(const std::string& word);

        //建立索引
        bool BuildIndex();
    
    private:   

        // 构建正排索引 将拿到的一行html文件传输进来，进行解析
        // 构建的正排索引，就是填充一个 DocInfo这个数据结构 ，然后将 DocInfo 插入 正排索引的 vector中即可 
        DocInfo* BuildForwordIndex(const std::string& line);
        bool BuildInvertedIndex(const DocInfo &Doc);

    private:
        ns_model::Model model;

    };

}