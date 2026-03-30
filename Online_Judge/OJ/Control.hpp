#ifndef CONTROL_HPP
#define CONTROL_HPP

#include <mutex>
#include <vector>
#include <string>
#include <future>
#include <memory>
#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>

#include <drogon/drogon.h>
#include <jsoncpp/json/json.h>

#include "View.hpp"
#include "oj_Model.hpp"
#include "../Common/Pool/ThreadPool.hpp"
#include "../Common/Log/clog.hpp"
#include "../Common/Util/Tool.hpp"
#include "../Common/Http/httplib.h"

namespace ns_control
{
    inline const char *control_logger_name = "control_logger";

    //提供服务的主机
    class Machine
    {
    public:
        Machine();
        ~Machine();

        //提升和减少主机负载
        void IncLoad();
        void DecLoad();
        //清空负载
        void ResetLoad();

        //获取主机负载
        uint64_t Load();
    public:

        std::string ip;
        int port;
        //服务器的ip和端口号
        uint64_t load;      //编译服务的负载    
        std::mutex *mtx;    // 用于保护计数器的锁（注意：cpp中的mutex是禁止拷贝的，所以我们这里定义指针来进行后面的拷贝）
    };

    const std::string service_machine = "/home/zz/Desktop/project/Online_Judge/OJ/conf/service_machine.conf"; // 提供服务的主机列表的路径

    //负载是多线程模块所以每次进行操作的时候都要加锁避免操作错误和指针泄漏等问题
    //负载均衡模块
    class LoadBlance
    {
    public:
        LoadBlance();
        ~LoadBlance();

        //加载主机
        bool LoadConf(const std::string& machine_conf);

        // 智能选择合适的主机提供服务
        // 参数 id:输出型参数 m:输出型参数
        bool SmartChoice(int *id , Machine** m);

        //离线指定的主机
        void offlineMachine(int which);
        //上线主机
        void onlineMachine();

        //展示所有在线和离线的主机
        void ShowMachines();

    private:
        std::vector<Machine> machines;          //用来编译的主机
        std::vector<int>online;                 //在线的主机id
        std::vector<int>offline;                //离线的主机id
        std::mutex mtx;                         // mtx是LoadBalance的锁，是保证LoadBalance它的数据安全

    };

    //业务逻辑控制器
    class Control
    {
    public:
        Control();
        ~Control();

        //恢复主机
        void RecoveryMachine();

        //根据所有题目基本数据构建网页
        bool AllQuestions(std::string* html);
        //根据题号构建单个题目网页
        bool Question(const std::string& number , std::string* html);
        //判题功能
        // number：题号，in_json：客户上传上来的代码，out_json：要返回的结果
        void Judge(const std::string in_json , std::string* out_json); 

        //上传题目(这个返回的json串的话就是报错信息嘛)
        void UploadQuestion(const std::string& in_json , std::string* out_json);
        int GetMaxid();
    private:

        ns_model::Model model;   //提供后台数据
        ns_view::View view;     //提供渲染服务
        LoadBlance load_blance; //负载均衡器
        std::mutex mtx;         //插入操作为了保证一致性加的锁
    };

};

#endif