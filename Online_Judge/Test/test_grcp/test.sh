#!/bin/bash

# 一键测试脚本
echo "🚀 启动服务器..."
cd build
./server &
SERVER_PID=$!

echo "等待服务器启动..."
sleep 3

echo "🎯 运行客户端测试..."
./client

echo "🛑 停止服务器..."
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

echo ""
echo "✅ 测试完成！"