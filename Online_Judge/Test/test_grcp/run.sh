#!/bin/bash

# 一键构建和运行脚本
set -e  # 遇到错误立即退出

echo "🧹 清理旧文件..."
rm -rf build
rm -f *.pb.* *.grpc.pb.*

echo "🛠️  生成 protobuf 和 gRPC 代码..."
protoc --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` rpc.proto
if [ $? -ne 0 ]; then
    echo "❌ 代码生成失败，请检查 protoc 和 gRPC 安装"
    exit 1
fi

echo "📦 创建构建目录..."
mkdir -p build && cd build

echo "🔧 运行 CMake..."
cmake ..

echo "🔨 编译项目..."
make -j4

echo ""
echo "✅ 构建完成！"
echo ""
echo "📋 使用说明:"
echo "1. 在一个终端中运行服务器:"
echo "   ./build/server"
echo ""
echo "2. 在另一个终端中运行客户端:"
echo "   ./build/client"
echo ""
echo "3. 或者运行一键测试:"
echo "   ./test.sh"

cd ..