#include "Control.hpp"

ns_control::Control::Control()
{

}

ns_control::Control::~Control()
{

}

bool ns_control::Control::AllQuestions(std::string* html)
{
    bool ret = true;
    std::vector<ns_model::Question>all;

    if(model.GetAllQuestions(&all))
    {
        //根据题目编号进行排序
        std::sort(all.begin() , all.end() , [](ns_model::Question& a , ns_model::Question& b)
        {
            return atoi(a.number.c_str()) < atoi(b.number.c_str());
        });
    
        //获取成功构造页面
        view.AllExpandHtml(all , html);
    }
    else
    {
        // 获取失败
        *html = "获取题目失败，形成题目列表失败";
        ret = false;
    }

    return ret;
}

bool ns_control::Control::Question(const std::string& number , std::string* html)
{
    bool ret = true;
    ns_model::Question q;

    if(model.GetOneQuestion(number , &q))
    {
        //获取数据成功，构建网页
        view.OneExpand(q , html);
    }
    else
    {
        //获取失败
        *html = "指定题目" + number + " 不存在";
        ret = false;
    }

    return ret;
}

//判题
void ns_control::Control::Judge(const std::string in_json , std::string* out_json)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(control_logger_name);
    
    //先将用户的代码反序列化提炼出来之后再拼接上我们准备的代码然后在进行提交
    Json::Value in_value;
    Json::Reader reader;

    //将in_json进行反序列化得到用户代码
    reader.parse(in_json , in_value);
    std::string code = in_value["code"].asString();
    std::string num = in_value["number"].asString();
    std::string lang = in_value["language"].asString();
    std::string self_input = in_value["input"].asString();
    bool text = in_value["is_self_test"].asBool();

    //logger->debug("已经开始处理");
    //根据题目编号获得相应数据
    struct ns_model::Question q;

    if(!model.GetOneQuestion(num , &q))
    {
        logger->warn("获取资源失败");
    }

    Json::Value compile_code;
    compile_code["number"] = num;
    compile_code["code"] = code + "\n";
    compile_code["cpu_limit"] = q.cpu_limit;
    compile_code["mem_limit"] = q.mem_limit;
    compile_code["test_count"] = q.test_count;
    compile_code["input"] = in_value["input"].asString();
    compile_code["language"] = lang;
    compile_code["is_self_test"] = text;

    logger->debug("input:%s",self_input.c_str());

    //logger->debug("编译代码为:%s",code.c_str());
    Json::FastWriter writer;
    //将拼接好的代码序列化
    std::string compile_string = writer.write(compile_code);

    std::promise<bool> promise;
    std::future<bool> future = promise.get_future();

    //一直选择负载最低的主机
    while(true)
    {
        int id = 0;
        Machine* m = nullptr;

        if(!load_blance.SmartChoice(&id , &m))
        {
            //所有主机已经挂机
            break;
        }
        
        //发起http请求
        std::shared_ptr<drogon::HttpClient> cli = drogon::HttpClient::newHttpClient("http://" + m->ip + ":" + std::to_string(m->port));
        
        //创建请求
        auto req = drogon::HttpRequest::newHttpRequest();
        req->setMethod(drogon::Post);
        req->setPath("/compile_and_run");
        req->setBody(compile_string);
        req->addHeader("Content-Type" , "application/json;charset=utf-8");

        m->IncLoad();

        //发送请求
        cli->sendRequest(req , [& , cli , req , m , id , out_json](drogon::ReqResult result , 
            const drogon::HttpResponsePtr& resp)
        {
            ns_clog::Logger::ptr loggers = ns_clog::LoggerManager::GetInstance().getLogger(control_logger_name);
            loggers->info("连接主机成功id为%d,主机状态为%s:%d",id , m->ip.c_str() , m->port);
            if(result == drogon::ReqResult::Ok && resp != nullptr)   
            {
                if(resp->getStatusCode() == drogon::HttpStatusCode::k200OK)
                {
                    //http请求成功
                    promise.set_value(true);
                    *out_json = resp->body();
                    m->DecLoad();           //减少主机负载
                    loggers->info("请求和编译服务成功");
                }

                m->DecLoad();
            }
            else
            {
                //请求失败
                loggers->warn("主机成功id为%d,主机状态为%s:%d的主机可能已离线",id , m->ip.c_str() , m->port);
                promise.set_value(false);
                load_blance.offlineMachine(id);
                load_blance.ShowMachines();
            }
        });

        //等待异步结果返回
        if(future.wait_for(std::chrono::seconds(20)) == std::future_status::ready) 
        {
            if(future.get()) 
            {
                break; // 成功
            } 
            else 
            {
                load_blance.offlineMachine(id);
            }
        }
    }
}

void ns_control::Control::UploadQuestion(const std::string& in_json , std::string* out_json)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(control_logger_name);

    //提取数据
    Json::Value in_value;
    Json::Reader reader;
    ns_model::Question q;

    reader.parse(in_json , in_value);
    q.title = in_value["title"].asString();
    q.star = in_value["star"].asString();
    q.desc = in_value["desc"].asString();
    q.test_count = in_value["test_count"].asInt();
    q.cpu_limit = in_value["cpu_limit"].asInt();
    q.mem_limit = in_value["mem_limit"].asInt();

    Json::Value out_value;

    //将数据插入数据库
    if(!model.InsertOneQuestion(q))
    {
        logger->warn("插入数据失败");
        out_value["message"] = "插入数据失败";
    }

    if(!out_value["message"].isString())
    {
        out_value["message"] = "插入数据成功";
    }

    Json::StyledWriter writer;
    writer.write(out_json);

    return;
}

int ns_control::Control::GetMaxid()
{
    return model.GetQuestionMaxID();
}

void ns_control::Control::RecoveryMachine()
{
    //将所有离线主机上线
    load_blance.onlineMachine();
}

ns_control::Machine::Machine()
:ip("") , port(0) , load(0) , mtx(nullptr)
{

}

ns_control::Machine::~Machine()
{

}

void ns_control::Machine::IncLoad()
{
    if(mtx)
        mtx->lock();
    
    ++load;

    if(mtx)
        mtx->unlock();
}

void ns_control::Machine::DecLoad()
{
    if(mtx)
        mtx->lock();

    --load;

    if(mtx)
        mtx->unlock();
}

void ns_control::Machine::ResetLoad()
{
    if(mtx)
        mtx->lock();

        load = 0;

    if(mtx)
        mtx->unlock();
}

//获得主机负载
uint64_t ns_control::Machine::Load()
{
    uint64_t temp = 0;
    if(mtx)
        mtx->lock();

        temp = load;

    if(mtx)
        mtx->unlock();

    return temp;
}

ns_control::LoadBlance::LoadBlance()
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(control_logger_name);
    assert(LoadConf(service_machine));
    //logger->info("加载%s成功",service_machine.c_str());
}

ns_control::LoadBlance::~LoadBlance()
{

}

//加载主机
bool ns_control::LoadBlance::LoadConf(const std::string& machine_conf)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(control_logger_name);

    std::ifstream in(machine_conf);
    if(!in.is_open())
    {
        logger->fatal("加载%s失败\n",machine_conf.c_str());
        return false;
    }
    
    std::string line;
    //获取主机相应数据
    while(std::getline(in , line))
    {
        //读取数据，分割字符，插入主机数据
        std::vector<std::string>tokens;
        ns_util::StringUtil::SplitString(line , &tokens , ":");

        if(tokens.size() != 2)
        {
            //数据有误
            logger->warn("获取数据%s失败\n",line.c_str());
            return false;       
        }

        Machine machine;
        machine.ip = tokens[0];
        machine.port = atoi(tokens[1].c_str());
        machine.load = 0;
        machine.mtx = new std::mutex;

        //上线并且记录此主机
        online.push_back(machines.size());
        machines.push_back(machine);
    }

    in.close();

    return true;
}

//二级指针可以改变指针指向的对象
bool ns_control::LoadBlance::SmartChoice(int* id , Machine** m)
{
    //加锁,锁定选择功能
    mtx.lock();    

    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(control_logger_name);

    //使用负载均衡算法:hash+轮询
    int online_num = online.size();
    if(online_num == 0)
    {
        //没有在线的主机
        mtx.unlock();
        logger->fatal("没有在线的主机!!!");

        return false;
    }

    //通过遍历找到负载最低的机器
    *id = online[0];
    *m = &machines[online[0]];
    //默认最小负载数
    uint64_t min_load = machines[online[0]].Load();
    for(int i = 1; i < online_num; i++)
    {
        uint64_t cur_load = machines[online[i]].Load();
        if(min_load > cur_load)
        {
            min_load = cur_load;
            *id = online[i];
            *m = &machines[online[i]];
        }
    }

    //解锁
    mtx.unlock();
    return true;
}

void ns_control::LoadBlance::offlineMachine(int which)
{
    mtx.lock();

    for(auto iter = online.begin(); iter != online.end(); iter++)
    {
        if(*iter == which)
        {
            //去重相应参数
            //要先重置负载，不然再次上线的时候负载会累积增减
            machines[which].ResetLoad();

            online.erase(iter);
            offline.push_back(which);

            break;
        }
    }

    mtx.unlock();
}

//上线所有主机
void ns_control::LoadBlance::onlineMachine()
{
    mtx.lock();

    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(control_logger_name);

    online.insert(online.end() , offline.begin() , offline.end());
    offline.erase(offline.begin() , offline.end());

    mtx.unlock();

    logger->info("所有主机已经重新上线");
}

void ns_control::LoadBlance::ShowMachines()
{
    mtx.lock();

    std::cout << "当前上线的主机为:";
    for(auto &id : online)
    {
        std::cout << id << " ";
    }
    std::cout << "\n";

    for(auto& id : offline)
    {
        std::cout << id << " ";
    }
    std::cout << "\n";

    mtx.unlock();
}

