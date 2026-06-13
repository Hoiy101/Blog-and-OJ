#编译脚本

cd /home/zz/Desktop/project/Blog-and-OJ/Online_Judge

cd Compile

mkdir build && cd build
cmake ..
make -j$(nproc)

cd /home/zz/Desktop/project/Blog-and-OJ/Online_Judge

cd OJ

mkdir build && cd build
cmake ..
make -j$(nproc)