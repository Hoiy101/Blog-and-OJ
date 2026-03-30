#include "Format.hpp"

ns_clog::Formatter::Formatter(const std::string &Pattern_)
:Pattern(Pattern_)
{
    //检查分割字符串是否正确
    assert(parsePattern());
}

std::ostream& ns_clog::Formatter::format(std::ostream& out , const LogMsg& msg)
{
    //将内容输出到流
    for(auto& it : Items)
    {
        it->format(out , msg);
    }

    return out;
}

//将内容输出到字符串
std::string ns_clog::Formatter::format(const LogMsg& msg)
{
    std::stringstream ss;
    for(auto& it : Items)
    {
        it->format(ss , msg);
    }

    return ss.str();
}


//  根据不同的格式化字符创建不同的格式化子项对象
/*
    %d ⽇期
    %T 缩进
    %t 线程id
    %p ⽇志级别
    %c ⽇志器名称
    %f ⽂件名
    %l ⾏号
    %m ⽇志消息
    %n 换⾏
*/
//格式化创建对象
ns_clog::FormatItem::ptr ns_clog::Formatter::CreateItem(const std::string& key , const std::string value)
{
    if (key == "d")
        return std::make_shared<TimeFormatItem>(value);
    if (key == "T")
        return std::make_shared<TabFormatItem>();
    if (key == "t")
        return std::make_shared<ThreadFormatItem>();
    if (key == "p")
        return std::make_shared<LevelFormatItem>();
    if (key == "c")
        return std::make_shared<LoggerFormatItem>();
    if (key == "f")
        return std::make_shared<FileFormatItem>();
    if (key == "l")
        return std::make_shared<LineFormatItem>();
    if (key == "m")
        return std::make_shared<MesFormatItem>();
    if (key == "n")
        return std::make_shared<NlineFormatItem>();
    if (key == "")
        return std::make_shared<OtherFormatItem>(value);

    std::cout << "不存在该格式化字符: %" << key << std::endl;
    //断开程序
    abort();

    return FormatItem::ptr();
}

bool ns_clog::Formatter::parsePattern()
{
    //1.提取各部分的字符串
    std::vector<std::pair<std::string , std::string>>order;
    
    size_t pos = 0;
    std::string value , key;

    while(pos < Pattern.size())
    {
        //判断是否为原始字符串
        if(Pattern[pos] != '%')
        {
            value.push_back(Pattern[pos++]);
            continue;
        }

        //看看这里是不是有两个%
        if(pos + 1 < Pattern.size() && Pattern[pos + 1] == '%')
        {
            value.push_back('%');
            pos += 2;
            continue;
        }

        //如果这个字符是格式化字符,并且原始字符处理完毕
        if(!value.empty())
        {
            order.push_back(std::make_pair("" , value));
            value.clear();
        }

        //提取格式化字符比如:%d,%p中的d和p
        pos++;
        if(pos == Pattern.size())
        {
            std::cout << "%之后没有格式还的字符" << std::endl;
            return false;
        }
        key = Pattern[pos];
        pos++;
        
        //开始提取子规则
        if(pos < Pattern.size() && Pattern[pos] == '{')
        {
            //指向子规则提取的位置
            pos++;
            //提取里面的内容
            while(pos < Pattern.size() && Pattern[pos] != '}')
            {
                value.push_back(Pattern[pos++]);
            } 

            //如果此时跳出来且到了末尾的话，那么出错了返回false
            if(pos == Pattern.size())
            {
                std::cout << "不存在}，发生错误" << std::endl;
                return false;
            }
            pos++;
        }

        order.push_back(std::make_pair(key , value));
        key.clear();
        value.clear();
    }

    //创建子对象，并且初始化Item数组
    for(auto& elem : order)
    {
        Items.push_back(CreateItem(elem.first , elem.second));
    }

    return true;
}