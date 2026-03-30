// 测试用例

#ifndef COMPILER_ONLINE
// 下面的代码，我们不想让编译器编译的时候，保留它，而是裁剪掉（g++ -D COMPILER_ONLINE）
// 仅仅是为了让我们设计测试用例的时候，不要报错
#include "header.cpp";
#endif

void Test1()
{
    std::vector<int> v = {1, 2, 3, 4, 5, 6};
    int max = Solution().Max(v);
    
    if (max == 6)
    {
        std::cout << "Test 1 ... OK" << std::endl;
    }
    else
    {
        std::cout << "Test 1 ... Failed" << std::endl;
    }
}


void Test2()
{
    std::vector<int> v = {-1, -2, -3, -4, -5, -6};
    int max = Solution().Max(v);
    
    if (max == -1)
    {
        std::cout << "Test 2 ... OK" << std::endl;
    }
    else
    {
        std::cout << "Test 2 ... Failed" << std::endl;
    }
}

int main()
{
    Test1();
    Test2();

    return 0;
}