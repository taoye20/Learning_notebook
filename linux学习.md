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
```