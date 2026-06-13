#!/bin/bash

#遇见错误时退出
set -e

#注册异常抛出任务
trap echo "脚本在$LINENO行出错"

#设置颜色
readonly GREEN='\033[0;32m'
readonly RED='\033[0;31m'
readonly NC='\033[0m'

#${NC}重置颜色
#$1函数调用的第一个参数
echo_step() {
  echo -e "${GREEN}[OK]${NC} $1"
}

echo_error(){
    echo -e "${RED}[ERROR]${NC} $1"
}

run_step(){
  local step_name="$1"
  shift

  if "$@"; then
    echo_step "$step_name"
  else
    echo_error "$step_name"
    FAILED_STEPS+=("$step_name")
  fi
}

cd ~/Desktop

echo_step "安装CPP依赖"
sudo apt-get update
sudo apt-get install -y cmake g++ libssl-dev uuid-dev zlib1g-dev

echo_step "创建库目录"
mkdir library
sudo apt update

#安装cppjieba库
if [ ! -d "cppjieba" ]; then
  git clone https://github.com/yanyiwu/cppjieba.git
fi
cd cppjieba

# 2. 初始化并更新子模块（重要！）
echo -e "${GREEN}[INIT]${NC} $1"
git submodule init
git submodule update

# 3. 创建构建目录并编译
mkdir build
cd build
cmake ..
make -j$(nproc)

# 4. 安装到系统目录（可选）
sudo make install

#之后进入到limonp目录
cd limonp
sudo cp -r include/limonp /usr/local/include/
#或者进入目录后
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install

#安装Boost库
sudo apt install libboost-all-dev -y
echo_step "安装Boost库成功"

#安装jsoncpp库
sudo apt install libjsoncpp-dev -y
echo_step "安装jsoncpp库成功"

#安装ctemplate库
sudo apt install libctemplate-dev -y
echo_step "安装ctemplate库成功"

#安装MySQL C Connect库
sudo apt install libmysqlcppconn-dev -y
echo_step "安装MySQL C Connect库成功"

#通过源码安装drogon库
cd ~/Desktop
cd library

if [ ! -d "drogon" ]; then
  git clone https://github.com/drogonframework/drogon.git
fi
cd drogon

mkdir build
cd build
cmake ..
make
sudo make install