# server_frame

### 部分工具安装

* 文档工具
~~~bash
sudo apt-get install doxygen
sudo apt-get install graphviz
~~~

* http有限状态机
~~~bash`
sudo apt-get install ragel
ragel -G2 -C XXX.rl XXX.cc
~~~

* google test安装
~~~bash
git clone git@github.com:google/googletest.git
cd googletest
mkdir build && cd build
cmake ..
make && make install
~~~

* 测试工具安装
~~~bash
wget http://blog.s135.com/soft/linux/webbench/webbench-1.5.tar.gz  
tar zxvf webbench-1.5.tar.gz  
cd webbench-1.5  
make
sudo make install
~~~

~~~bash
sudo apt-get install apache2-utils
~~~

* 测试命令
~~~bash
webbench -c 10000 -t 5 http://172.19.150.194:8020/
ab -n 50000 -c 1000 http://172.19.150.194:8020/index.html
~~~
