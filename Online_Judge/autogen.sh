#!/bin/bash
cd ~/Desktop
<<<<<<< HEAD

sudo apt-get update
sudo apt-get install -y cmake g++ libssl-dev uuid-dev zlib1g-dev


mkdir library
sudo apt update
=======
>>>>>>> 8a770b4 (将cmake文件的绝对路径改为相对路径)

# Arch Linux 使用 pacman 包管理器
# 更新系统并安装基础开发工具
sudo pacman -Syu --noconfirm
sudo pacman -S --noconfirm base-devel cmake openssl zlib

# 安装 uuid 库（Arch 中叫 libutil-linux）
sudo pacman -S --noconfirm util-linux-libs
# 开发头文件在 util-linux-devel 包中
sudo pacman -S --noconfirm util-linux-devel

# 创建 library 目录
mkdir -p ~/Desktop/library
cd ~/Desktop/library

# 安装 cppjieba 库
git clone https://github.com/yanyiwu/cppjieba.git
cd cppjieba

# 初始化并更新子模块
git submodule init
git submodule update

# 创建构建目录并编译
mkdir build
cd build
cmake ..
make -j$(nproc)

# 安装到系统目录
sudo make install

# 安装 limonp
cd ../limonp
sudo cp -r include/limonp /usr/local/include/

# 安装 Boost 库
sudo pacman -S --noconfirm boost

# 安装 jsoncpp 库
sudo pacman -S --noconfirm jsoncpp

# 安装 ctemplate 库
sudo pacman -S --noconfirm ctemplate

<<<<<<< HEAD
#安装MySQL C Connect库
sudo apt install libmysqlcppconn-dev -y

#通过源码安装drogon库
cd ~/Desktop
cd library

git clone https://github.com/drogonframework/drogon.git
cd drogon

=======
# 安装 MySQL C++ Connector
sudo pacman -S --noconfirm mariadb-libs
sudo pacman -S --noconfirm mariadb-connector-c
# MySQL C++ Connector 在 Arch 中是 mysql++-connector
sudo pacman -S --noconfirm mariadb-connector-c++

# 通过源码安装 drogon 库
cd ~/Desktop/library
git clone https://github.com/drogonframework/drogon.git
cd drogon

