import random
import os       #文件操作
import subprocess   #执行shell命令

def generate_one_case():
    #先生成n , m
    n = random.randint(1, 100)    #生成指定返回的随机数
    m = random.randint(1, 1000)
    arr = [random.randint(1, 500) for i in range(n)]  #生成随机列表
    result = generate_result(arr, m)
    
    return n , m , arr , result

def generate_result(arr, m):
    #创建字典
    dir = {}
    #enumerate(arr) 可以同时得到索引和元素
    for i , num in enumerate(arr):
        if num not in dir:
            dir[num] = i

    min_i , min_j = 100000 + 4 , 100000 + 4
    st = False

    for i , num in enumerate(arr):
        temp = m - num
        if temp in dir:
            j = dir[temp]
            if i != j:
                min_i = i
                min_j = j
                st = True
                break
    if st:
        return min_i + 1 , min_j + 1
    else:
        return [-1]
    
def generate_file(T , inputname , outputname):
    #生成T组数据
    #使用with自动管理资源，as把对象赋值给input
    with open(inputname , 'w') as input:
        with open(outputname , 'w') as output:
            input.write(f"{T}\n")
            #生成T组数据
            for _ in range(T):
                n , m , arr , result = generate_one_case()
                input.write(f"{n} {m}\n")
                #将1 2 3 4 5\n写入文件
                input.write(" ".join(map(str , arr)) + "\n")
                if result == n + 5:
                    output.write("-1\n")
                else:
                    output.write(" ".join(map(str , result)) + "\n")

def main():
    #设置种子保证可重复性

    T = random.randint(1 , 10)
    base_name = "/home/zz/Desktop/project/Online_Judge/OJ/Questions/Data/"
    intput = ""
    output = ""

    print(f"生产{T}组数据")
    for i in range(5 , 10):
        input = base_name + "input/" + f"{i + 1}.in"
        output = base_name + "output/" + f"{i + 1}.out"
        generate_file(T , input , output)
        print(f"生产第{i + 1}组数据完成")

if __name__ == "__main__":
    main()