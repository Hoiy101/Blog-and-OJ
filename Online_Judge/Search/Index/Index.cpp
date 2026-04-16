#include "Index.hpp"

namespace ns_Index
{
    //为单例模式声明类型创建内存
    Index* Index::instance = nullptr;
    std::mutex Index::mtx;
};

ns_Index::Index* ns_Index::Index::Getinstance()
{
    if(instance == nullptr)
    {
        mtx.lock();
        if(instance == nullptr)
        {
            instance = new Index();
        }
        mtx.unlock();
    }

    return instance;
}

ns_Index::DocInfo* ns_Index::Index::GetForwardIndex(uint64_t Doc_idx)
{
    if(Doc_idx >= Forward_Index.size())
    {
        std::cout << "Doc_idx out range!" << std::endl;
        return nullptr;
    }

    return &Forward_Index[Doc_idx];
}

ns_Index::InvertedList* ns_Index::Index::  GetInvertedList(const std::string& word)
{
    auto iter = Inverted_Index.find(word);
    if(iter == Inverted_Index.end())
    {
        std::cout << "have no InvertedList" << std::endl;
        return nullptr;
    }

    //返回第二个成员
    return &(iter->second);
}

//根据去标签，格式化后的文档来构建索引
//这里改为数据库操作来获取数据
bool ns_Index::Index::BuildIndex()
{
    //这里我们要用到之前raw.txt里面存入了我们需要的数据，但是同时我们存储数据是以二进制的方式来存储的，所以我们也需要用二进制的方式来打开
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(Server_logger_name);
    // logger->debug("test begin");

    std::vector<ns_model::BlogPost>blog_posts;
    model.GetAllBlogPosts(&blog_posts);

    //logger->debug("尝试读入数据库成功");
    if(blog_posts.empty())
    {
        std::cout << "blog_posts is empty!" << std::endl;
        return  false;
    }

    for(auto post : blog_posts)
    {
        std::string line = post.title + "\3" + post.content;
        DocInfo *doc = BuildForwordIndex(line);
        logger->debug("尝试获取文档信息成功");



        if(doc == nullptr)
        {
            std::cerr << "Build " << line << " error!" << std::endl;
            continue;
        }

        BuildInvertedIndex(*doc);
    }

    return true;
}

//我们构建正排索引只需要填充DocInfo这个结构体，然后插入到队尾就行了
ns_Index::DocInfo* ns_Index::Index::BuildForwordIndex(const std::string& line)
{
    // 1. 解析 line ，字符串的切分  分为 DocInfo 中的结构
    // 1. line -> 3 个 string (title , content , url)

    std::vector<std::string>results;
    std::string Sep = "\3";             //这是需要定义的分隔符
    
    //将字符串进行分割
    ns_util::StringUtil::SplitString(line , &results , Sep);
    if(results.size() != 2)
    {
        return nullptr;
    }

    //将字符串填充到我们的DocInfo中
    DocInfo doc;
    doc.title = results[0];
    doc.content = results[1];
    doc.doc_id = Forward_Index.size();       //先保存idx，然后再插入，最后这个size就是我们的下标

    //插入到正排索引
    Forward_Index.push_back(std::move(doc));

    return &Forward_Index.back();
}

//建立倒排索引
bool ns_Index::Index::BuildInvertedIndex(const DocInfo &Doc)
{
    //词频结构体
    struct word_cnt
    {
        int title_cnt;
        int content_cnt;
        word_cnt() : title_cnt(0) , content_cnt(0) {}
    };

    //关键字和词频对应的结构体
    std::unordered_map<std::string , word_cnt> word_cnt;        //用来储存词频的映射

    //对标题进行分词统计
    std::vector<std::string> title_word;
    ns_util::JiebaUtil::CutString(Doc.title , &title_word);

    for(auto s : title_word)
    {
        boost::to_lower(s);         //将此统一化为小写
        word_cnt[s].title_cnt++;    //统计标题词频
    }

    //同理，统计内容的词频
    std::vector<std::string> content_word;
    ns_util::JiebaUtil::CutString(Doc.content , &content_word);

    for(auto s : content_word)
    {
        boost::to_lower(s);
        word_cnt[s].content_cnt++;
    }

    #define X 10
    #define Y 1

    //最终构建倒排索引
    for(auto& word_pair : word_cnt)
    {
        InvertedElem item;              //定义一个倒排拉链节点，然后填写相应的字段
        item.doc_id = Doc.doc_id;        //对应Doc的索引
        item.word = word_pair.first;      //关键词
        item.weight = X * word_pair.second.title_cnt + Y * word_pair.second.content_cnt;        //计算权重
        
        //将我们填充好的元素插入到List中
        InvertedList& Inverted_List = Inverted_Index[word_pair.first];
        Inverted_List.push_back(std::move(item));
    }   

    return true;
}
