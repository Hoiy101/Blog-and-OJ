//因此本项目采用双缓冲区的思想，也就是说，存在一个输入缓冲区和一个处理缓冲区，当输入缓冲区中有数据的时候，就可交换两个缓冲区，
//此时工作线程对缓冲区中的数据进行读取，业务线程也可以在此时对另外一个空的缓冲区进行写入，这样子就可以实现业务线程和工作线程的同步，
//两者唯一存在的锁冲突就是交换缓冲区的那一刻，提高了写入和读取的效率，两者不需要进行大量的锁竞争，且双缓冲区不涉及资源的频繁申请和释放
//任务池的设计思想：双缓冲区阻塞数据池

//采用双缓冲区数据池的思想来解决生产者-消费者线程间的冲突

#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <iostream>
#include <cassert>
#include <vector>

#define DEFAULT_BUFFER_SIZE (1 * 1024 * 1024)
#define THREHOLD_BUFFER_SIZE (8 * 1024 * 1024)
#define LINEARGROWTH_BUFFER_SIZE (1 * 1024 * 1024)

namespace ns_clog
{
    class Buffer
    {
    public:
        Buffer();
        //写入数据
        void Push(const char *data , int len);
        //剩余可写空间的大小
        size_t WriteAbleSize() { return buffer.size() - Write_idx; }
        //读取位置的起始位置
        const char* Begin() { return &buffer[Read_idx]; }
        //可读取位置的长度
        size_t ReadAbleSize() {return (Write_idx - Read_idx); }
        //当前可读位置向后移动
        void MoveReadIndex(size_t len);
        //初始化缓冲区
        void reset();
        //交换缓冲区
        void swap(Buffer& buffer_);
        //判断缓冲器是否为空
        bool empty() { return Read_idx == Write_idx; }

    private:
        //当前写入位置后移
        void MoveWriteIndex(size_t len);
        //扩容
        void EnsureEnoughSize(size_t len);

    private:
        std::vector<char>buffer;
        size_t Read_idx;         //可读位置的指针
        size_t Write_idx;        //可写位置的指针
    };
};

#endif 