#include "Buffer.hpp"

ns_clog::Buffer::Buffer()
:buffer(DEFAULT_BUFFER_SIZE)
,Write_idx(0)
,Read_idx(0)
{

}

void ns_clog::Buffer::Push(const char *data , int len)
{
    //扩容->写数据->位置后移
    EnsureEnoughSize(len);
    //将数据写入Buffer
    std::copy(data , data + len , &buffer[Write_idx]);
    MoveWriteIndex(len);
}

void ns_clog::Buffer::MoveReadIndex(size_t len)
{
    //判断数据是否可读
    assert(len <= ReadAbleSize());
    Read_idx += len;
}

void ns_clog::Buffer::reset()
{
    Write_idx = 0;
    Read_idx = 0;
}

void ns_clog::Buffer::swap(Buffer& buffer_)
{
    //交换二者的数据
    buffer.swap(buffer_.buffer);
    std::swap(Read_idx , buffer_.Read_idx);
    std::swap(Write_idx , buffer_.Write_idx);
}

void ns_clog::Buffer::MoveWriteIndex(size_t len)
{
    //写入数据后移
    assert(Write_idx + len <= buffer.size());
    Write_idx += len;
}

void ns_clog::Buffer::EnsureEnoughSize(size_t len)
{
    //判断是否需要扩容
    if(len <= WriteAbleSize())
    {
        return;
    }

    size_t newsize = 0;
    //判断是否小于阈值
    if(buffer.size() < THREHOLD_BUFFER_SIZE)
    {
        newsize = buffer.size() * 2 + len;
    }
    else
    {
        newsize = buffer.size() + LINEARGROWTH_BUFFER_SIZE + len;
    }

    buffer.resize(newsize);
}