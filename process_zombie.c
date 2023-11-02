#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    pid_t pid;
    pid = fork();
    if(pid < 0) {
        perror("fork error");
        exit(1);
    }else if(pid == 0){
        printf("pid: %d, ppid:%d\n", getpid(), getppid());
        exit(0); //子进程结束，父进程继续运行，子进程变僵尸进程，进程结束了但结构体并没有被内核删除
    }

    while (1)
    {
        sleep(1);
    }
    return 0;
}