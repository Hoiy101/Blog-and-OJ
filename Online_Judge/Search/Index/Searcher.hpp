#ifndef SEARCHER_HPP
#define SEARCHER_HPP

#include "Index.hpp"
#include "../Parse/Util.hpp"
#include <algorithm>
#include <vector>
#include <string>
//#include <json/json.h>
#include <jsoncpp/json/json.h>

namespace ns_Searcher
{
    inline const char *Server_logger_name = "server_model_logger";

    //这里我们搜索中会出现一种情况
    //比如我们的关键词是C++ , Boost。那么可能会有两个文档都满足这种结果，然后它们就会出现在两个倒排索引中从而导致我们出现的网址有重复的
    //所以我们要声明一个去重的数据结构
    struct InvertedElemPrint
    {
        uint64_t doc_idx;
        int weight;
        std::vector<std::string>words;      //关键词的集合
        InvertedElemPrint() : doc_idx(0) , weight(0) {}
    };

    class Searcher
    {
    private:
        ns_Index::Index *index;

    public:

        Searcher() {}
        ~Searcher() {}

    public:

        void initSearcher();
        void Search(const std::string& query , std::string* json_string);
        std::string Desc(const std::string& html_content , const std::string& word);
        
    };

};


#endif  //Searcher.hpp