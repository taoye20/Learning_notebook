#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

int terminal_opened = 0; // 标志变量，表示终端是否已经打开

pid_t ran_child_pid = -1; // 保存ran的子进程的PID
pid_t cn_child_pid = -1;

void cn_execute_command_in_new_terminal(const char* command)
{
    cn_child_pid = fork();
    if (cn_child_pid == 0) {
        // 子进程
        execlp("gnome-terminal", "gnome-terminal", "--", "bash", "-c", command, NULL);
        exit(0);
    }
    else if (cn_child_pid > 0) {
        // 父进程
        int status;
        waitpid(cn_child_pid, &status, 0); // 等待子进程退出 
        cn_child_pid = -1; // 将子进程PID重置为无效值
    }
}

void ran_execute_command_in_new_terminal(const char* command)
{
    ran_child_pid = fork();
    if (ran_child_pid == 0) {
        // 子进程
        execlp("gnome-terminal", "gnome-terminal", "--", "bash", "-c", command, NULL);
        exit(0);
    }
    else if (ran_child_pid > 0) {
        // 父进程
        int status;
        waitpid(ran_child_pid, &status, 0); // 等待子进程退出 
        ran_child_pid = -1; // 将子进程PID重置为无效值
    }
}


//关闭基站相关的进程
void terminate_ran_child_process()
{
    system("pkill -f ./nr-softmodem");
    ran_child_pid = -1;
}


//清理命令的文件
void clear_file(const char* fileName) {
    FILE *fp = fopen(fileName, "w");
    if (ftruncate(fileno(fp), 0) == -1) {
        printf("error clearing file\n");
    }
    fclose(fp);
    printf("file has been cleared\n");
}




int main()
{
    FILE* fp;
    char buffer[80];
    bool is_ran_open = false;
    bool is_cn_open = false;

    while (1) {
        fp = popen("cat /home/core1/partfrom_program/bin/try.txt", "r");
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            printf("%s", buffer);
            
            //开启基站
            if (strstr(buffer, "begin_oai") != NULL && !is_ran_open) {
                ran_execute_command_in_new_terminal("cd ~/openairinterface5g/cmake_targets/ran_build/build && sudo ./nr-softmodem -O ../../../targets/PROJECTS/GENERIC-NR-5GC/CONF/gnb.sa.band78.fr1.106PRB.usrpb210.conf --gNBs.[0].min_rxtxtime 6 --sa -E --continuous-tx -d");
                is_ran_open = true;
                //system("gnome-terminal -- bash -c ls -al /etc/passwd");
                //sleep(3);
            }
            
            //开启核心网
            if (strstr(buffer, "begin_cn") != NULL && !is_cn_open) {
                cn_execute_command_in_new_terminal("cd ~/oai-cn5g && docker compose up -d");
                is_cn_open = true;
            }

            //关闭核心网
            if (strstr(buffer, "stop_cn") != NULL && is_cn_open) {
                cn_execute_command_in_new_terminal("cd ~/oai-cn5g && docker compose down");
                is_cn_open = false;
            }
            
            //关闭基站
            // 检查是否需要终止子进程
            if (is_ran_open && strstr(buffer, "stop_oai") != NULL) {
                terminate_ran_child_process();
                is_ran_open = false;
            }
            clear_file("/home/core1/partfrom_program/bin/try.txt");
        }
        pclose(fp);
        sleep(1); // 等待一秒钟
    }

    return 0;
}