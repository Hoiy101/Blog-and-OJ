#include "Searcher.hpp"

//初始化搜索引擎
void ns_Searcher::Searcher::initSearcher(const std::string& input)
{
    //获取单例模式
    index = ns_Index::Index::Getinstance();
    LOG(NORMAL , "获取单例模式成功");

    //构建索引
    index->BuildIndex(input);
    LOG(NORMAL , "建立索引成功");
}

//query:我们搜素的关键词 , json_string:我们存放发给客户的内存地址
void ns_Searcher::Searcher::Search(const std::string& query , std::string* json_string)
{
    //对于查询操作，我们要先对查询的词汇进行分词，然后根据每个分过的词进行索引搜索
    std::vector<std::string>words;
    util::JiebaUtil::CutString(query , &words);

    //在这里的查询操作中，我们要考虑去重的必要性，所以我们这里声明个容器来帮组我们去重
    std::vector<InvertedElemPrint> Inverted_List;
    std::unordered_map<uint64_t , InvertedElemPrint> tokens_map;

    for(std::string word : words)
    {
        //步骤:先获取每个词->获取它对应的倒序排序->进行去重
        ns_Index::InvertedList* Inverted_List = index->GetInvertedList(word);

        if(Inverted_List == nullptr)
        {
            continue;
        }

        //遍历到排序列，然后进行去重
        for(const auto &Elem : *Inverted_List)
        {
            //然后我们获取元素，如果有重复直接覆盖
            //如果没有的话，那么就是新增一个元素
            auto& item = tokens_map[Elem.doc_id];
            item.doc_idx = Elem.doc_id;
            item.weight += Elem.weight;
            item.words.push_back(Elem.word);
        }
    }

    //遍历整个map，然后放在倒排集合
    for(const auto& elem : tokens_map)
    {
        Inverted_List.push_back(std::move(elem.second));
    }

    //按照weight来排序
    std::sort(Inverted_List.begin() , Inverted_List.end() , 
        [](InvertedElemPrint& a , InvertedElemPrint& b)
        {
            return a.weight > b.weight;
        }
    );

    //构建Json串放入json_string里，用于和客户端通信
    // 第一步：创建 JSON 树结构
    Json::Value root;
    for(auto &item : Inverted_List)
    {
        //先获取文档信息
        ns_Index::DocInfo* doc = index->GetForwardIndex(item.doc_idx);
        if(doc == nullptr)
        {
            continue;
        }

        //构建JSON
        Json::Value elem;
        //三个json分别是标题，简述和网址
        elem["title"] = doc->title;
        elem["desc"] = Desc(doc->content , item.words[0]);
        elem["url"] = doc->url;

        //拼JSON串
        root.append(elem);
    }

    // 第二步：创建写入器（序列化器）   
    Json::StreamWriterBuilder writer;

    // 第三步：执行序列化
    *json_string = Json::writeString(writer, root);
    // 将内存中的 Json::Value 对象转换为字符串
}

std::string ns_Searcher::Searcher::Desc(const std::string& html_content , const std::string& word)
{
    //找到word(关键字)在html_content中首次出现的位置
    //然后往前找50个字节(如果往前不足50字节，就从begin开始)
    //往后找100个字节(如果往后不足100字节，就找到end即可)
    //截取出这部分内容

    const int prev_step = 50;
    const int next_step = 100;

    //找到首次出现的位置
    auto item = std::search(html_content.begin() , html_content.end() , word.begin() , word.end() , 
        [](int x , int y)
        {
            return (std::tolower(x) == std::tolower(y));
        }
    );

    if(item == html_content.end())
    {
        //这里证明其实我们没找到那么我们直接返回
        return "None1";
    }

    int pos = std::distance(html_content.begin() , item);

    //获取start和end的位置
    int start = 0;
    int end = html_content.size() - 1;

    //根据pos，更新位置
    if(pos > prev_step + start) start = pos - prev_step;
    if(pos < end - next_step) end = pos + next_step;

    //截取字符串
    if(start >= end) return "None2";
    std::string doc = html_content.substr(start , end - start);
    doc += "...";
    
    return doc;
}
