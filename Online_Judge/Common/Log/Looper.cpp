#include "Looper.hpp"

ns_clog::AsyncLooper::AsyncLooper(const Functor& cb_ , AsyncType looper_type)
:Stop(false)
,thread(std::thread(&AsyncLooper::threadEntry , this))
,cb(cb_)
,Looper_Type(looper_type)
{

}

ns_clog::AsyncLooper::~AsyncLooper()
{
    stop();
}

void ns_clog::AsyncLooper::stop()
{
    //停止并且堵塞所有线程
    Stop = true;
    cond_con.notify_all();
    thread.join();
}

void ns_clog::AsyncLooper::push(const char*data , int len)
{
    //上锁保证线程同步
    std::unique_lock<std::mutex>lock(mutex);

    //判断是否有空间，然后写入数据
    if(Looper_Type == AsyncType::ASYNC_SAVE)
    {
        //这里SAVE版本需要检查一下
        cond_pro.wait(lock , [&]() {return len <= pro_buf.WriteAbleSize(); });   
        //这个使用谓词检查的步骤
        //1:获取锁，检查谓词(若谓词为true直接取消堵塞)
        //2:释放锁并等待
        //3:被唤醒再次尝试获取锁，检查谓词
    }

    //开始写入数据
    pro_buf.Push(data , len);
    //唤醒工作线程
    cond_con.notify_all();
}

void ns_clog::AsyncLooper::threadEntry()
{
    while(1)
    {
        //我们这里设计两个缓冲区的交换，所有我们要声明一个栈内存，防止并发过程中出现错误
        {
            //加锁
            std::unique_lock<std::mutex>lock(mutex);
            //如果异步工作器已经退出并且输入缓冲器搞好了，那么直接退出
            if(Stop && pro_buf.empty())
            {
                break;
            }

            //如果缓冲器没有数据，就要被堵塞
            cond_con.wait(lock , [&](){ return Stop || !pro_buf.empty(); });
            //开始交换数据

            pro_buf.swap(con_buf);
        }
        //交换好了就可以处理数据了
        if(Looper_Type == AsyncType::ASYNC_SAVE)
        {
            //唤醒线程处理数据
            cond_pro.notify_all();
        }

        //调用回调函数处理数据并且初始化缓冲器
        cb(con_buf);
        con_buf.reset();
    }

}
