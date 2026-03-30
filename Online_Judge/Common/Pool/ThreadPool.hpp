#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <queue>
#include <mutex>
#include <thread>
#include <vector>
#include <future>
#include <memory>
#include <atomic>
#include <stdexcept>
#include <functional>
#include <condition_variable>

namespace ns_pool
{
    class ThreadPool
    {
    public:
        ThreadPool(int Default_Thread_Num = 5);
        ~ThreadPool();

        template<class F , class ...Args>
        auto enqueue(F&& f , Args&&... args)->std::future<typename std::invoke_result_t<F , Args...>>
        {
            //推导返回类型
            using return_type = typename std::invoke_result_t<F , Args...>;
        
            //获取任务使用保证
            auto task = std::make_shared<std::packaged_task<return_type()>>(
                //绑定函数和参数
                //forward是为了保持参数的左值右值属性(完美转发)
                std::bind(std::forward<F>(f) , std::forward<Args>(args)...)
            );
            
            std::future<return_type>res = task->get_future();

            //上锁给队列里面加入内容
            {
                std::unique_lock<std::mutex>lock(mtx);
                //加入任务队列
                tasks.emplace([task]()
                {
                    //执行任务
                    (*task)();
                });
            }

            //唤醒一个线程执行任务
            cond.notify_one();

            return res;
        }

    private:
        void work();

    private:
        std::mutex mtx;
        std::atomic<bool>stop;
        std::vector<std::thread>works;
        std::condition_variable cond;
        std::queue<std::function<void()>>tasks;

    };

}

#endif
