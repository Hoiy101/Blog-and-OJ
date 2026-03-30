#include "Runner.hpp"

ns_runner::Runner::Runner()
{

}

ns_runner::Runner::~Runner()
{
    
}

void ns_runner::Runner::SetProcLimit(int cpu_limit , int mem_limit)
{
    //设置cpu的时间
    struct rlimit cpu_rlimit;
    cpu_rlimit.rlim_cur = cpu_limit;
    cpu_rlimit.rlim_max = cpu_limit;

    setrlimit(RLIMIT_CPU , &cpu_rlimit);

    //设置最大的内存限制
    struct rlimit mem_rlimit;
    mem_rlimit.rlim_max = mem_limit * 1024;
    mem_rlimit.rlim_cur = mem_limit * 1024;

    setrlimit(RLIMIT_AS , &mem_rlimit);

    //设置栈大小，防止无限递归
    struct rlimit stack_rlimit;
    stack_rlimit.rlim_cur = 64 * 1024 * 1024;  // 64MB栈大小
    stack_rlimit.rlim_max = 64 * 1024 * 1024;

    setrlimit(RLIMIT_STACK , &stack_rlimit);

    //设置数据段大小，防止无限制申请内存
    struct rlimit data_rlimit;
    data_rlimit.rlim_cur = mem_limit * 1024;
    data_rlimit.rlim_max = mem_limit * 1024;
    setrlimit(RLIMIT_DATA , &data_rlimit);
}

/*
    返回值 > 0 : 程序异常了，退出时收到了信号，返回值就是对应的信号编号
    返回值 == 0 : 程序正常运行完毕的，结果保存到了对应的临时文件中
    返回值 < 0 : 内部错误：打开文件失败、创建子进程失败
*/
//和测试模块相似，在调用部分设置运行次数，然后跑n次最后和结果作比较，返回一个枚举类比较好
std::vector<int> ns_runner::Runner::Run(const std::string& file_name , const std::string&id , int test_count , int cpu_limit , int mem_limit)
{
    static ns_pool::ThreadPool thread_pool;

    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(running_logger_name);
    std::vector<int> final_status(test_count , 0);
    std::vector<std::future<void>>futures;
    
    for(int test_point = 1; test_point <= test_count; test_point++)
    {
        //不能直接值捕获，不然test_point会改变
        auto future = thread_pool.enqueue([&final_status , &logger , test_point , id , file_name , cpu_limit , mem_limit , this]
        {
            //测试的时候先尝试读取文件内容
            std::string input_file = "input/" + id + "/" + std::to_string(test_point);
            std::string output_file = "output/" + id + "/" + std::to_string(test_point);
            //获取对应题目的相应文件:标准输入，输出，错误，可执行文件
            std::string Execute = ns_util::PathUtil::Exe(file_name);
            std::string Stdin = ns_util::PathUtil::Stdin(input_file);
            std::string Stderr = ns_util::PathUtil::Stderr(file_name);
            std::string Stdout = ns_util::PathUtil::Stdout(output_file);

            umask(0);

            //这里改用为管道技术
            //int stderr_fd = open(Stderr.c_str() , O_CREAT | O_WRONLY , 0644);
            int stdin_fd = open(Stdin.c_str() , O_CREAT | O_RDONLY , 0644);
            int stdout_pipe[2];
            int stderr_pipe[2];

            if(pipe(stdout_pipe) < 0 || pipe(stderr_pipe) < 0)
            {
                logger->Error("创建管道失败");
                final_status[test_point - 1] = -1;
                return;
            }

            //看看文件是否打开成功

            //创建一个新进程来运行Code
            pid_t pid = fork();
            if(pid < 0)
            {
                //创建错误
                logger->Error("创建子线程失败\n");
                close(stdin_fd);
                close(stdout_pipe[0]);
                close(stderr_pipe[0]);
                close(stdout_pipe[1]);
                close(stderr_pipe[1]);
                
                final_status[test_point - 1] = -2;
                return;
            }
            else if(pid == 0)
            {
                close(stdout_pipe[0]);
                close(stderr_pipe[0]);

                //子进程:运行代码
                //重新定向文件
                //让标准输入读取重定向为std_fd文件，输出重定向为stdout_fd文件，错误重定向为stderr_fd文件
                dup2(stdin_fd , STDIN_FILENO);
                dup2(stdout_pipe[1] , STDOUT_FILENO);
                dup2(stderr_pipe[1] , STDERR_FILENO);

                close(stdout_pipe[1]);
                close(stderr_pipe[1]);

                SetProcLimit(cpu_limit , mem_limit);
                // 然后替换程序，将子进程替换成需要的可执行文件
                execl(Execute.c_str() , Execute.c_str() , nullptr);

                //替换失败
                logger->warn("切换程序失败\n");
                exit(1);
            }
            else
            {
                //父进程:回收进程
                close(stdin_fd);
                close(stdout_pipe[1]);
                close(stderr_pipe[1]);

                std::string stdout_text , stderr_text;
                size_t stdout_size = 0 , stderr_size = 0;
                char buffer[256];
                while((stdout_size = read(stdout_pipe[0] , buffer , sizeof(buffer))) > 0)
                {
                    stdout_text.append(buffer , stdout_size);
                }
                while((stderr_size = read(stderr_pipe[0] , buffer , sizeof(buffer))) > 0)
                {
                    stderr_text.append(buffer , stderr_size);
                }

                close(stdout_pipe[0]);
                close(stderr_pipe[0]);


                int state = 0 , temp;
                waitpid(pid , &state , 0);          //以阻塞的形式来获取退出的状态
                temp = state & 0x7f;
                logger->info("运行完成,info=%d",temp);

                //运行完成后判断结果是否正确(可以交给线程池来处理)
                if(!temp && Check_Output(Stdout , stdout_text))
                {
                    temp = 0;
                }
                else
                {
                    temp = 1;
                }

                final_status[test_point - 1] = temp;
            }
        });

        futures.push_back(std::move(future));
    }

    for(auto& future : futures)
    {
        future.get();
    }

    return final_status;
}

//这里使用管道技术(可以先在test函数试验一下)
/*
    子进程:输入管道写，其他读
    父进程:只需要读输出和错误管道
    在测试用例那一块需要一个新文件
*/

//data是用户输入的数据
//file_name是可执行文件的路径
std::string ns_runner::Runner::Run_one_test(const std::string&file_name , const std::string&data , int& status_code , int cpu_limit , int mem_limit)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(running_logger_name);

    std::string file_out , file_err;

    int stdin_pipe[2];
    int stdout_pipe[2];
    int stderr_pipe[2];
    if(pipe(stdout_pipe) < 0 || pipe(stderr_pipe) < 0 || pipe(stdin_pipe) < 0)
    {
        logger->Error("创建管道失败");
        return "error";
    }

    //创建进程
    pid_t pid = fork();
    if(pid < 0)
    {
        logger->Error("创建进程失败\n");
        return "error in create process";
    }
    else if(pid == 0)
    {
        //子进程模拟沙盒环境运行代码
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);

        dup2(stdin_pipe[0] , STDIN_FILENO);
        dup2(stdout_pipe[1] , STDOUT_FILENO);
        dup2(stderr_pipe[1] , STDERR_FILENO);

        close(stdin_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);
        //执行可执行文件

        SetProcLimit(cpu_limit , mem_limit);
        execl(ns_util::PathUtil::Exe(file_name).c_str() , ns_util::PathUtil::Exe(file_name).c_str() , nullptr);

        logger->warn("切换程序失败\n");
        exit(1);
    }
    else
    {
        //父进程输入数据
        close(stdin_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        //输入数据
        write(stdin_pipe[1] , data.c_str() , data.size());
        close(stdin_pipe[1]);

        size_t stdout_size , stderr_size;
        char buffer[256];
        while((stdout_size = read(stdout_pipe[0] , buffer , sizeof(buffer))) > 0)
        {
            file_out.append(buffer , stdout_size);
        }
        while((stderr_size = read(stderr_pipe[0] , buffer , sizeof(buffer))) > 0)
        {
            file_err.append(buffer , stderr_size);
        }

        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        //等待子进程结束(主要是捕获24信号和9信号，如果有这两个信号，那么直接截断)
        int state = 0;  
        waitpid(pid , &state , 0);          //以阻塞的形式来获取退出的状态
        status_code = state & 0x7f;
        logger->info("运行完成,info=%d",status_code);
    }

    if(file_err.empty())
        return file_out;
    return file_err;
}

//比较和切割字符串的数据
bool ns_runner::Runner::Check_Output(std::string ans_path , std::string stdout_text)
{
    ns_clog::Logger::ptr logger = ns_clog::LoggerManager::GetInstance().getLogger(running_logger_name);

    //直接使用副本数据，然后用户传入的时候要直接传
    //使用Boost模糊分割字符串类似于1 2 3                   4             5分割为1 2 3 4 5 
    //然后getline，并判断后续的str2是不是空字符，如果不是，那么输出false
    std::string line_one , line_two;
    //从文件中读取答案
    std::string ans_text;
    if(!ns_util::FileUtil::ReadFile(ans_path , &ans_text , true))
    {
        logger->Error("读取答案文件失败");
        return false;
    }

    std::stringstream ss1(ans_text) , ss2(stdout_text);
    if(stdout_text.size() < ans_text.size())
        return false;
    while(std::getline(ss1 , line_one) && std::getline(ss2 , line_two))
    {
        std::vector<std::string>token_one , token_two;
        //按" "分割字符串 

        //先去掉字符串首尾的空格
        boost::trim(line_one);
        boost::trim(line_two);
        
        boost::split(token_one , line_one , boost::is_space() , boost::token_compress_on);
        boost::split(token_two , line_two , boost::is_space() , boost::token_compress_on);
        //判断token_one和token_two是否相等
        if(token_one != token_two)
            return false;
    } 

    //判断之后的str2行中是否有内容
    while(std::getline(ss2 , line_two))
    {
        for(auto c : line_two)
            if(c != ' ')
            {
                std::cout << c << "\n";
                return false;
            }
    }

    return true;
}