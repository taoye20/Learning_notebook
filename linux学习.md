# Linux学习
## 指令
```c++
df -h //查看磁盘空间和挂载情况

free -m  //查看内存，其中标识为swap的为虚拟内存，用磁盘的

tree  //显示目录结构用树形展示

find ./ -name '*.txt'  //寻找文件的一个例子

grep -r 'copy' ./  //寻找文件中的内容的例子

ps -aux  //进程查询，a为查看当前系统所有用户进程，u为详细信息，x为显示没有终端控制的进程

find /sbin -perm +700 |xargs ls -l  //xargs捕获输出并管道操作
```

```c++
//输出重定向
command > file	//将输出重定向到 file。
command < file	//将输入重定向到 file。
command >> file	//将输出以追加的方式重定向到 file。
ls | tee -a ls.txt   //保留ls.txt文件中原来的内容，并把ls命令的执行结果添加到ls.txt文件的后面。
```

```c++
//快捷键
ctrl + a  //光标移动到最前
ctrl + e  //光标移动到最后
```

```c++
//文件功能
bin //放二进制可执行文件
boot  //放开机启动程序
dev  //设备文件
home  //放用户
etc  //用户信息和系统配置文件
lib  //库文件
root  //管理员
usr  //用户资源管理目录
```

```c++
//用户和用户组
whoami  //查看当前用户
exit  //退出root
-rwxrw-r-- core core 。。。 //数值为4，2，1。三个权限集分别为用户、组、root。后面表示文件所属用户和用户组，使用chown来改变
```
![Alt text](image-21.png)

```c++
//gcc编译
//上面我们经常使用的-o参数实际上是指定输出文件名
gcc hello.c -I ./inc -o hello  //当头文件放在inc中时，使用-I参数指定.h文件所在路径 
```

程序库（lib）分为动态库（共享库）和静态库，动态库在程序调用时加载，具有灵活的优点，静态库有加载快的优点。静态库会在链接的时候将库里函数复制到执行文件中，但共享库会使用原库函数中的地址，所以多个程序都可以同时去读取库中的函数，叫共享。

```c++
//静态库制作
//1，编译c文件
gcc -c add.c -o add.o
gcc -c sub.c -o sub.o

/*
//add.c，执行加操作
int add(int a, int b){
    return a + b;
}
*/

//创建和加入静态库
ar rcs libmylib.a add.o sub.o  //libmylib.a是我为静态库起的名字，起名规范是前面带lib

//使用静态库，若add.c被移动到了别处，使用gcc test.c -o test将无法完成上面的链接步骤
gcc test.c libmylib.a -o test  //其中test.c调用了add函数

/*
//test.c
include "mymath.h"  //即使有静态库一起编译，但没有这行声明，编译器会隐式声明并在编译时有wraning
include "mymath.h"  //.h中的ifndef是防止重复的引用

int main(){
    int a = 4;
    int b = 6;
    printf("add res = %d", add(a, b));
    return 0;
}



//mymath.h
#ifndef _MYMATH_H_
#define _MYMATH_H_

int add(int, int);
int sub(int, int);

#endif
*/

//结合以上知识，此时使用以下代码可以创建一个目录的结构
mkdir inc
mv mymath.h ./inc
mkdir lib
mv libmtlib.a lib
gcc test.c ./lib/libmylib.a -o test -I ./inc
```

```c++
//动态库制作
//根据理论，动态库的文件应当生成与位置无关的代码，记结论，即在生成时加上-fPIC
gcc add.c -o add.o -fPIC

//制作库
gcc -sharded -o libmymath.so add.o sub.o
mv libmaymath.so ./lib

//编译, -l so文件名 -L 库文件路径
export LD_LIBRARY_PATH=./lib  //添加动态库路径，临时生效
vim `/.bashrc //写入LD_LIBRARY_PATH=./lib  ， source .bashrc
gcc test.c -o test -l mymath -L ./lib -I ./inc
```

gdb是用于程序调试的工具，功能强大。
```c++
//调试以前编译时要加-g
gcc gdbtest.c -o gdbtest -g

//进入调试模式
gdb gdbtest

//打印原代码
list 1 //从第一行开始，简写l

//设置断点
break 52  //在52行断点，简写b

//调试运行
run  //简写r，停在断点处，出错调试直接查找到出错位置

//下一步
next //n，执行完下一行，如果是函数则也执行完函数
step //s，如果是函数则进去

//查看值
print i //p

//运行到结束
cotinue

//退出调试
quit

//设置main函数输入参数
set args ...  //也可以run ...

//条件断点
b 41 if i=5

//查看所有断点
info b
```