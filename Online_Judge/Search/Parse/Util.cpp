#include "Util.hpp"


namespace util
{
    //声明断词的中文词典路径
    const char* const DICT_PATH = "/home/zz/Desktop/library/cppjieba/dict/jieba.dict.utf8";
    const char* const HMM_PATH = "/home/zz/Desktop/library/cppjieba/dict/hmm_model.utf8";    
    const char* const USER_DICT_PATH = "/home/zz/Desktop/library/cppjieba/dict/user.dict.utf8";    
    const char* const IDF_PATH = "/home/zz/Desktop/library/cppjieba/dict/idf.utf8";    
    const char* const STOP_WORD_PATH = "/home/zz/Desktop/library/cppjieba/dict/stop_words.utf8";

    //类外初始化，就是将上面的路径传进去，具体和它的构造函数是相关的，具体可以去看一下源代码
    cppjieba::Jieba JiebaUtil::jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
}

//读取文件的内容，将文件的内容放入out里面
bool util::FileUtil::ReadFile(const std::string& File_Path , std::string* out)
{
    //打开文件(以只读的方式打开)
    std::ifstream file(File_Path , std::ios::in);

    if(!file.is_open())
    {
        std::cerr << "open file " << File_Path << " failed!" << std::endl;
        return false;
    }

    //读取文件内容
    std::string line;
    while(std::getline(file , line))
    {
        *out += line;
    }

    file.close();

    return true;
}

//分词函数的实现
void util::StringUtil::Split(const std::string& target , std::vector<std::string>* out , const std::string& sep)
{
    //boost库中的split函数
    boost::split(*out, target, boost::is_any_of(sep), boost::token_compress_on);

    //第一个参数：表示你要将切分的字符串放到哪里
    //第二个参数：表示你要切分的字符串
    //第三个参数：表示分割符是什么，不管是多个还是一个
    //第四个参数：它是默认可以不传，即切分的时候不压缩，不压缩就是保留空格
    //如：字符串为aaaa\3\3bbbb\3\3cccc\3\3d
    //如果不传第四个参数 结果为aaaa  bbbb  cccc  d
    //如果传第四个参数为boost::token_compress_on 结果为aaaabbbbccccd
    //如果传第四个参数为boost::token_compress_off 结果为aaaa  bbbb  cccc  d
}

void util::JiebaUtil::CutString(const std::string &src , std::vector<std::string>* output)
{
    //调用CutForSearch函数，第一个参数就是你要对谁进行分词，第二个参数就是分词后的结果存放到哪里
    jieba.CutForSearch(src , *output);
}