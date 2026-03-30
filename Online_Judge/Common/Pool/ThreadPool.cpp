#include "ThreadPool.hpp"

ns_pool::ThreadPool::ThreadPool(int num) : stop(false)
{
    for(int i = 0; i < num; i++)
    {
        works.emplace_back([this]
        {
            work();
        });
    }
}

ns_pool::ThreadPool::~ThreadPool()
{
    //先修改停止状态
    {
        std::unique_lock<std::mutex>lock(mtx);
        stop = true;
    }

    //唤醒所有线程回收
    cond.notify_all();

    for(auto& worker : works)
    {
        //堵塞线程
        worker.join();
    }
}

void ns_pool::ThreadPool::work()
{
    while(true)
    {
        std::function<void()>task;

        //执行任务
        {
            std::unique_lock<std::mutex>lock(mtx);
            cond.wait(lock , [this]
            {
                return stop || !tasks.empty();
            });

            if(stop && tasks.empty())
                return;

            if(!tasks.empty())
                task = std::move(tasks.front());

            tasks.pop();
        }

        task();
    }
}