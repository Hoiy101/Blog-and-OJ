#!/bin/bash

#遇见错误时退出
set -e

#注册异常抛出任务
trap 'echo "脚本在$LINENO行出错"' ERR

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
sudo apt-get install -y cmake g++ git libssl-dev uuid-dev zlib1g-dev

echo_step "创建库目录"
mkdir -p library
cd library

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
git submodule update --init --recursive
mkdir -p build
cd build
cmake ..
make -j$(npoc)
sudo make install