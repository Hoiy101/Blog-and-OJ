#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <json/json.h>
#include <sstream>

// 实现数据的序列化  --- 将原本的对象，转换为 Json::Value 格式存储
bool Hierarch(const Json::Value &val , std::string &body)
{
    std::stringstream ss;
    // 进行序列化
    // 因为 StreamWriter 它的基类是一个抽象类，有纯虚函数存在，不能进行实例化
    // 所以通过工厂类 StreamWriterBuilder 来产生派生类对象
    Json::StreamWriterBuilder swb;
    // 父类对象指向子类对象
    // 注意 sw 是一个 new 出来的一个对象，需要去释放它 --- 采用智能指针
    std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter()); //实例化结束
    int ret = sw->write(val , &ss);
    // 这里序列化成功，会返回一个 0
    if(ret!=0)
    {
        std::cout<<"Json Hierarch failed!\n";
        return false;
    }
    body = ss.str();
    return true;

}


// 反序列化  -- 将 Json::Value 对象 转换成 原本的数据 
bool UnHierarch( const std::string &body , Json::Value &val)
{
    // 因为 CharReader 它的基类是一个抽象类，有纯虚函数存在，不能进行实例化
    // 所以通过工厂类 CharReaderBuilder 来产生派生类对象
    Json::CharReaderBuilder crb;
    // 父类对象指向子类对象
    // 注意 cr 是一个 new 出来的一个对象，需要去释放它 --- 采用智能指针
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
    std::string errs;
    // 进行数据解析
    bool ret = cr->parse(body.c_str() , body.c_str() + body.size() , &val , &errs);
    if(ret == false)
    {
        std::cout<<"Json UnHierarch failed\n";
        return false;
    }
    return true;
}

int main()
{
    const char* name  = "xas";
    int age = 18;
    const char* sex = "男";
    float score[3] = {88 , 77.5 , 66};

    // 进行数据传输
    Json::Value student;
    student["姓名"] = name;
    student["年龄"] = age;
    student["性别"] = sex;
    student["成绩"].append(score[0]);
    student["成绩"].append(score[1]);
    student["成绩"].append(score[2]);

    Json::Value fav;
    fav["书籍"] = "我与地坛";
    fav["运动"] = "打乒乓球";
    student["爱好"] = fav;
    std::string body;
    Hierarch(student , body);
    std::cout<<body<<std::endl;


    std::string str = R"({"姓名" : "WD" , "年龄" : 19 , "成绩" : [32,45.5,56]})";
    Json::Value stu;
    bool ret = UnHierarch(str , stu);
    if(ret == false)
    {
        return -1;  // 退出
    }
    std::cout<<"姓名: "<<stu["姓名"].asString()<<std::endl;
    std::cout<<"年龄: "<<stu["年龄"].asInt()<<std::endl;
    int sz = stu["成绩"].size();
    for(int i = 0;i < sz ; i++)
    {
        std::cout<<"成绩: "<<stu["成绩"][i].asFloat()<<std::endl;
    }

    return 0;
}
