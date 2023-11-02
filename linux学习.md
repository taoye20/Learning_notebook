# Linux学习
## 进程学习
### 进程终止
基本的文件操作方法和进程的终止：
```c
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void func1() {
    printf("func is exit!\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s file ...\n", argv[0]);
        exit(1);
    }

    axexit(func1);  //向内核登记终止函数，最后进程结束时会调用该函数。（自己登记的）

    FILE* fp = fopen(argv[1], "w");  //w代表写
    fprintf(fp, "hello ty");
    fclose(fp);
    return 0;
    //exit(0);   标准c库函数
    //_exit(0);  系统调用，不支持标准IO缓存，不自动调用终止函数
}
```

![Alt text](image-62.png)

### 进程信息查看
```shell
ps   #直接查看当前终端中的进程，具有PID，运行时间和进程运行的命令
ps -ef  #看到后台的进程，0号进程为系统内核
```
进程状态：

* 运行态，R
* 等待状态，S，分为可中断等待和不可中断等待，可中断可以接收信号并中断。
* 停止状态，T
* 僵尸状态，Z，进程已经结束，但内核还记录了进程的相关信息。

![Alt text](image-63.png)

### 进程调用函数
```c
#include <unistd.h>
#include <sys/types.h>
pid_t getpid(void);
uid_t getuid(void); //当前进程的实际用户，就我进入系统时，我自己就是实际用户，但是我可以使用root的身份来运行，那么此时root为有效用户
uid_t geteuid(void); //当前进程的有效用户，以有效用户的身份运行进程
gid_t getgid(void); //进程用户组
pid_t getppid(void);
pid_t getpgrp(void); //所在进程组ID
pid_t getpgid(pid_t pid); //进程id为pid的进程所在的进程组ID
```
**在shell上执行的命令都是shell的子进程**，因此查看一个c执行时看它父进程号是shell进程号。

### 子进程创建
pid_t pid = fork(void);创建子进程。在这行命令后会出现两个进程，父进程中pid的值为非负数，子进程中pid为0。父进程可以根据返回的pid控制子进程启停。
```c
int main(int argc, char* argv[]) {
    printf("pid: %d\n", getpid());

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("fork error");
    } else if (pid > 0) {   //处于父进程
        printf("i am parent, my pid is %d, my child pid is %d, fork return is %d\n", getpid(), pid, pid);
    } else {      //子进程
        printf("i am child, my pid is %d, my parent pid is %d, fork return is %d\n", getpid(), getppid(), pid);
    }
    
    return 0;
}
```
vfork()先运行且不复制父进程的内存空间。
![Alt text](image-64.png)
父进程和子进程共享代码，但是堆栈区都是独立分配的。在fork()时会将已经赋值的变量和一堆东西一起复制到子进程中。

请关注一个问题，无论是goble变量还是static父进程子进程都是使用的不同物理地址，可以各自修改。
```c
int g_v = 30;

int main(int argc, char* argv[]) {
    int a_v = 30;
    static s_v = 30;
    char* s = "hello ty";
    ssize_t size = strlen(s) * sizeof(char);
    
    FILE* fp = fopen("file_1.txt", "w");  //标准IO操作
    int fd = open("file_2.txt", O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG); //系统调用直接操作，不通过缓存直接写

    fprintf(fp, "%s, pid:%d", s, getpid());  //通过缓存（意味着内容会被子进程复制到缓存，到下面调用fprintf时会出现在子进程打印的行中）
    write(fd, s, size);  //直接写入不通过缓存，不会复制到子进程

    pid_t pid;
    pid = fork();

    if (pid < 0) {    //分开进程赋值
        perror("fork error");
    } else if (pid > 0) {   //处于父进程
        a_v = 40; g_v = 40; s_v = 40;
    } else {      //子进程
        a_v = 50; g_v = 50; s_v = 50;
    }
    
    //父子进程都会运行，fp指向同一个文件
    fprintf(fp, "pid:%d, g_v:%d, a_v:%d, s_v:%d\n", getpid(), g_v, a_v, s_v);
    //这里打出两行结果
    //hello ty, pid:2863 pid:2863, g_v:40, a_v:40, s_v:40
    //hello ty, pid:2863 pid:2864, g_v:50, a_v:50, s_v:50  //子进程复制了父进程中上面的缓存，因此前面也会打出hello和2863
    sleep(1);

    fclose(fp);  //父子进程都会调用，只有当父子进程都执行了这个操作后才会真正的close这个文件
    close(fd);
    return 0;
}
```
上面的程序关于到了fprintf和子进程复制父进程参数和缓存，以及独立的部分，看懂了就可以明白子进程对父进程的继承关系。

### 僵尸进程
子进程结束了，父进程还没结束，僵尸进程避免方法为：
* 父进程隔一段时间查询子进程是否结束，调用wait()和waitid()通知内核释放僵尸进程。
* 信号SIGCHLD通知处理，并在信号中调用wait
* 使子进程变成孤儿进程，然后用init回收。


### 守护进程和孤儿进程
守护进程在系统开始是就出现了，父进程为1号进程（init进程）。

孤儿进程是：父进程结束了子进程继续，成为孤儿进程，则会由init进程接管。
```c
int main(int argc, char* argv[]) {
    printf("pid: %d\n", getpid());

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("fork error");
    } else if (pid > 0) {   //处于父进程
        printf("%d deaded", getpid());
        exit(0);
    } else {      //子进程
        sleep(4);
        printf("pid: %d, ppid: %d", getpid(), getppid());  //输出这个时候的父进程（原来的父进程已经终止了），此时输出父进程为1。（与理论相同）
    }
    
    return 0;
}
```

### wait函数


## 指令

### 基本指令
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
### 建立库
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