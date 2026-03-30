cd ~/Desktop
mkdir library
sudo apt update

#安装cppjieba库
git clone https://github.com/yanyiwu/cppjieba.git
cd cppjieba

# 2. 初始化并更新子模块（重要！）
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
#mkdir build && cd build
#cmake ..
#make -j$(nproc)
#sudo make install

#安装Boost库
sudo apt install libboost-all-dev -y

#安装jsoncpp库
sudo apt install libjsoncpp-dev -y

#安装ctemplate库
sudo apt install libctemplate-dev -y

sudo apt install libmysqlcppconn-dev -y