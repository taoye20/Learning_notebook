# Linux学习

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
## 建立库
程序库（lib）分为动态库（共享库）和静态库，动态库在程序调用时加载，具有灵活的优点，静态库有加载快的优点。静态库会在链接的时候将库里函数复制到执行文件中，但共享库会使用原库函数中的地址，所以多个程序都可以同时去读取库中的函数，叫共享。

注意库是作用于可执行文件的可执行文件的，即汇编后的链接阶段。

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


## gdb调试
gdb是用于程序调试的工具，功能强大。gdb使用也是作用于可执行文件。
```c++
//调试以前编译时要加-g
gcc gdbtest.c -o gdbtest -g

//进入调试模式
gdb ...  //...代表可执行文件的文件名

//打印原代码
list 1 //从第一行开始，简写l

//设置断点
break 52  //在52行断点，简写b
delete 5   //删除断点，断点号
clear 52   //删除断点，行号

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

### gdb其他命令
实例：如果程序出现错误，则直接run，可以停顿在出错行。

```c++
//gdb模式下想直接运行代码
start

//结束当前函数调用返回到函数调用点
finish

//查看变量类型
ptype i  //i为变量名

//切换栈区作用域
bt  //查看栈区编号
frame ..  //..表示栈区的编号，换到该栈区
ptype ..  //查看不同函数栈区的存储变量

//一直跟踪一个值，我每次next时，它都会输出当前i的值
display i
undisplay i
```

## makefile
### 一个规则
makefile只能在文件中命名为makefile或Makefile，makefile的规则：
```c++
目标：依赖条件
    命令

//举例，如果希望执行命令gcc hello.c -o hello，makefile文件中应该如下写法
hello:hello.c
    gcc hello.c -o hello


//换分步骤的方式，注意这个规则，目标：依赖条件
hello:hello.o  //hello.o暂时不存在，则去找如何生成该条件
    gcc hello.o -o hello

hello.o:hello.c
    gcc -c hello.c -o hello.o
```

```c++
//另外一个例子
//使用之前制作库的例子有main.c，add.c，sub.c，div1.c
//命令
gcc hello.c add.c sub.c div.c -o a.out

//makefile书写
a.out:hello.c add.c sub.c div.c
    gcc hello.c add.c sub.c div.c -o a.out

//但此时忽然改正了add.c文件，那么之前的a.out不能用了，只能再次执行make，把全部都编译了，这在合作中并不好。
//在单独改add最后一步其实就只有一个链接需要做，因此
//这时候改正某个c文件后再make，就只执行非常小部分更改的位置
a.out:hello.o add.o sub.o div.o   //重执行链接
    gcc hello.o add.o sub.o div.o -o a.out

hello.o:hello.c
    gcc -c hello.c -o hello.o

add.o:add.c    //重执行编译
    gcc -c add.c -o add.o

sub.o:sub.c
    gcc -c sub.c -o sub.o

div.o:div.c
    gcc -c div.c -o div.o


//但如果我这么写，把第一行放到最后
hello.o:hello.c
    gcc -c hello.c -o hello.o

add.o:add.c    
    gcc -c add.c -o add.o

sub.o:sub.c
    gcc -c sub.c -o sub.o

div.o:div.c
    gcc -c div.c -o div.o

a.out:hello.o add.o sub.o div.o  
    gcc hello.o add.o sub.o div.o -o a.out
//出错！只执行hello那一行，因为它把第一行当成默认的终极目标执行完就走了

//加入一行，指定终极目标
ALL:a.out

```

### 两个函数
```c++
//找到当前目录下所有后缀为.c的文件，赋值给src
src = $(wildcard *.c) 

//把src变量中所有后缀为.c的文件替换成.o
obj = $(patsubst %.c, %.o, $(src))

a.out:$(obj)   
    gcc $(obj) -o a.out

hello.o:hello.c
    gcc -c hello.c -o hello.o

add.o:add.c    
    gcc -c add.c -o add.o

sub.o:sub.c
    gcc -c sub.c -o sub.o

div.o:div.c
    gcc -c div.c -o div.o

clean:  //清除指令，使用方法为make clean -n, -n是先打出来看一下是不是删除正确的东西，“-”删除不存在东西也不报错
    -rm -rf $(obj) a.out
```

### 三个变量
```c++
$@  //在规则命令中，表示规则的目标
$^  //在命令中，表示所有依赖条件
$<  //在命令中表示第一个依赖条件

//举例
src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))

ALL:a.out

a.out:$(obj)   
    gcc $(obj) -o a.out

hello.o:hello.c
    gcc -c $< -o $@

add.o:add.c    
    gcc -c $< -o $@

sub.o:sub.c
    gcc -c $< -o $@

div.o:div.c
    gcc -c $< -o $@

clean:
    -rm -rf $(obj) a.out



//模式规则，上面的代码可以写成如下
src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))

ALL:a.out

a.out:$(obj)   
    gcc $(obj) -o a.out

%.o:%.c      //如果将$<用在模式规则中，可以将列表中的依赖依次取出，并套用模式规则
    gcc -c $< -o $@

clean:
    -rm -rf $(obj) a.out

//伪目标
.PHONY: clean ALL

```
该模式规则应用后，我即便添加一个.c文件，也不用更改makefile文件。

```c++
//现在有一个要求
//所有.h文件放在inc目录下
//所有.c文件放在src目录下
//所有.o文件放在obj目录下
src = $(wildcard ./src/ *.c)
obj = $(patsubst ./src/%.c, ./obj/%.o, $(src))  //注意%，此时obj代表了./obj/*.o

ALL:a.out

inc_path = ./inc
myArgs = -I
myArgs_1 = -Wall -g

a.out:$(obj)
    gcc $< -o $@ $(myArgs_1)

./obj/%.o:./src/%.c   //注意%
    gcc -c $< -o $@ $(myArgs) $(inc_path) $(myArgs_1)

clean:
    -rm -rf $(obj) a.out

.PHONY: clean ALL
```

最后是make的指令
```c++
make -n //模拟运行make
make -f .. //指定文件执行make
```

# 系统编程
## 文件操作
使用man查看指令的用法

![Alt text](image-71.png)

```c++
man 2 open  //查看open的用法
```

### open
```c
#include<unistd.h>  //open的头
#include<fcntl.h>  //flags的头
#include<stdio.h>   //printf头

int open(const char* pathname, int flags);
int open(const char* pathname, int flags, mode_t mode);

fd = open("./xx.txt", O_RDONLY | O_CREAT, 0644);  //rw-r--r--权限mode，读或创建（若不存在）
```

### read和write
```c++
ssize_t read(int fd, void *buf, size_t count);  //返回读到的字节数，存在buf中，conut是max ssize

ssize_t write(int fd, const void *buf, size_t count);   //只不过buf是写入内容，count是要写入的大小

//读写例子
int main(int argc, char* argv[]){
    char buf[1024];
    int n = 0;

    int fd1 = open(argv[1], O_RDONLY);
    int fd2 = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, 0664);
    //if(fd < 0) perror("...");

    while(n = read(fd1, buf, 1024)){
        write(fd2, buf, n); //读多少n写多少
    }

    close(fd1);
    close(fd2);
}
```

那么对比系统函数与库函数fopen(),fgetc(),fputc()区别

![Alt text](image-72.png)

但是了解了系统函数，还是推荐优先使用库函数。

### fcntl控制文件属性
```c++
int fcntl(int fd, int cmd, .../* arg */);

//获取文件状态F_GETFL，设置文件状态F_SETFL

flags = fcntl(STDIN_FILENO, F_GETFL);  //获取stdin的属性
flags |= O_NOBLOCK;  //或等于添加该属性，属于位运算

int ref = fcntl(STDIN_FILENO, F_SETFL, flags);  
```

### lseek调整读写指针位置，获取文件大小
```c++
off_t lseek(int fd, off_t offset, int whence);
//offset 偏移量
//whence 起始偏移位置

int fseek(FILE *stream, long offset, int whence);  //库函数

//例子
char buf[1024];
char msg[] = "hello i am ty";
fd = open("s.txt", o_RDWR|O_CREAT, 0644);
write(fd, msg, strlen(msg));
//因此在这以前要加上以下代码
lseek(fd, 0, SEEK_SET);
while((n = read(fd, buf, 1))){
    if(n < 0){
        perror("read error");
        exit(1);
    }
    write(STDOUT_FILENO, buf, n);  //该处输出没有，为什么？因为读写的指针位置一样，写完时指向的是文本末尾
}
close(fd);


//获取文件大小
int lenth = lseek(fd, 0, SEEK_END);
```

### inode结构体
放文件的权限，类型，大小，时间，用户，磁盘盘号等信息。dentry为目录项。

![Alt text](image-74.png)

不同目录项创建文件有相同的inode号，那么就是读取的同一个地址，那么删除其中一个文件，inode记录减一，直到减到0，就没有文件用那个空间了。


### 输入输出函数和stat
```c++
int stat(const char *path, struct stat *buf);  //其中传入参数为path，传出的参数stat通过指针返回到buf，存放文件属性，因此也叫输入输出函数。

struct stat buf;
int ret = stat("s.txt", &buf);
if(ret < 0) ...
printf("file size: %ld\n", buf.st_size);

//通过man可以看到有几个函数可以通过buf.mode判断文件是什么类型的

//stat会穿透符号连接，链接文件会输出指向的那个文件的类型，而不会判断该文件为链接文件,lstat不会符号穿透。
```

### link函数
```c++
//就是用于创建目录项的函数，硬连接
int link(const char *oldpath, const char *newpath);

link(argv[1], argv[2]);

//重点unlink，硬连接数减到0并不会马上删除文件，而是等到所有使用该文件的进程结束后再删除
先unlink ，后write，那么会write到缓存，然后在结束时释放文件
```

### 目录操作
```c++
DIR opendir(const char* name);
int closedir(DIR *dirp);
struct dirent *readdir(DIR *dirp);

//重要
struct dirent {
    inode
    char dname[]
}
```


### 递归目录小程序
```c++
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sysy/stat.h>
#include<pthread.h>
#include<dirent.h>

void fetchdir(const char *dir){
    char path[256];
    DIR *dp;
    struct dirent *sdp;

    dp = opendir(dir);
    if(dp == NULL){
        perror("opendir error");
        exit(1);
    }

    while((sdp = readdir(dp)) ! NULL){
        if(strcmp(sdp->d_name, ".") || strcmp(sdp->d_name, "..") == 0){
            continue;
        }
        sprintf(path, "%s/%s", dir, sdp->d_name);  //拼接绝对路径
        isFile(path);  //递归
    }

    closedir(dp);
}

void isFile(const char* name){
    struct stat sbuf;
    if(stat(name, &sbuf) == -1){
        fprintf(stderr, "isfile: can't access %s\n", name);
        exit(1);
    }
    if((buf.st_mode & S_IFMT) == S_IFDIR){
        fetchdir(name);  
    }else{
        printf("%ld %s\n", sbuf.st_size, name); //输出非目录文件的大小和名字
    }
}

int main(int argc, char *argv){
    if(argc == 1){
        isFile(".");
    }else{
        isFile(argv[1]);
    }

    return 0;
}
```



## 进程学习

### PCB进程控制块

![Alt text](image-73.png)

文件描述符表中记录了文件结构体FILE，这个对用户是不可见的。一个进程最多能打开1024个文件。

### 阻塞非阻塞
常规读文件不会阻塞。产生阻塞的场景：
* 读设备文件，例如：/dev/tty  终端文件
* 读网络文件

```c++
//阻塞
int main(){
    char buf[10];
    int n = 0;

    n = read(STDIN_FILENO, buf, 10);  //读标准输入，是设备文件属性，默认阻塞
    if(n < 0) {
        perror("read error");
        exit(1);
    }

    write(STDIN_FILENO, buf, 10);
    return 0;
}

//希望改变文件的是否阻塞属性
    fd = open("/dev/tty", O_RDONLY|O_NONBLOCK);

//注意非阻塞文件读取为空也会返回-1，但是erno会置为EAGAIN
trygain:
    n = read(fd, buf, 10);
    if(n < 0){
        if(errno != EAGAIN){
            perror(..)
            exit(1);
        }
        else{
            write(STDOUT_FILENO, "try again!\n", strlen("try again!\n"));
            sleep(2);
            goto tryagain;
        }
    
    write(STDOUT_FILENO, buf, n);
    close(fd);
    }
```
该代码将输入终端设置为非阻塞，并不停的访问是否有读到，是一种轮询的方式。不是一个好设计，好设计是中断。

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
两个函数：
* pid_t wait(int *status);等待所有子进程。返回可能是任何一个子进程的ID。
* pid_t waitpid(pid_t pid, int *status, int options);等待特定的子进程。

这两个函数都是成功调用就返回等待的子进程，失败返回-1。statue指等待的子进程是以什么状态结束的，可以使用以下代码打出退出状态。

![Alt text](image-65.png)

```c
int status;
wait(&status);
out_status(status);

void out_status(int status) {
    if(WIFEXITED(status)){
        printf("normal exit,stop sig is %d\n", WIFEXITED(status));
    }
    ...
}
```
kill -l 命令可以看到每种信号的编号。比如某些终止信号和中断信号。

理解了wait之后，知道了一件事，如果没有指定子进程结束，那么子进程就会一直在保存着，直到父进程结束，僵尸由init回收。而如果父进程调用wait，则在父进程中wait时会回收僵尸。

### exec函数
fork后常常要使用exec函数以执行另一个可执行程序（第三方写好的程序或系统中自带的函数），就比如我写了一个脚本，希望在子进程中启动一个wireshark抓包，则会用到exec。

调用exec不会创建新的子进程，只是用新的程序替换了子进程的代码段，堆和栈。

![Alt text](image-66.png)

![Alt text](image-67.png)

注意不返回原来的代码是指这条命令之后就不再执行下面的代码了。

### system函数
简化exec的使用，system在内部构建一个子进程，并在子进程中调用exec。system输入参数为char*类型。

### 进程状态
![Alt text](image-68.png)