#ifndef LOOPER_HPP
#define LOOPER_HPP

#include "Buffer.hpp"

#include <mutex>
#include <atomic>
#include <thread>
#include <iostream>
#include <functional>
#include <condition_variable>

//这个类是我们的主要业务处理类
//所有，我们需要两个缓冲区一个锁，两个环境变量和用于并发的线程

namespace ns_clog
{
    enum class AsyncType
    {
        ASYNC_SAVE = 0,
        ASYNC_UNSAVE
    };

    class AsyncLooper
    {
        
    public:
        using Functor = std::function<void(Buffer&)>;
        using ptr = std::shared_ptr<AsyncLooper>;

        AsyncLooper(const Functor& cb_ , AsyncType looper_type = AsyncType::ASYNC_SAVE);
        ~AsyncLooper();
        void push(const char *data , int len);
        void stop();

    private:
        void threadEntry();

    private:
        
        AsyncType Looper_Type;
        std::mutex mutex;
        Buffer con_buf;             //处理的缓冲器
        Buffer pro_buf;             //输入的缓冲区
        std::condition_variable cond_con;
        std::condition_variable cond_pro;
        std::atomic<bool>Stop;
        std::thread thread;
        Functor cb;

    };
};

#endif