# 核心网
## docker查询相关命令
```shell
docker ps   #查询所有docker id号

docker exec <docker_id> ifconfig  #查看对应docker的ip地址

docker exec -it <docker_id> bash  #exec 命令用于在容器内部执行命令，-it 参数用于交互式地进入容器，bash 是要执行的命令（在这里我们使用的是 Bash Shell）

docker logs oai-amf  #查询连接UE的注册情况

docker network inspect demo-oai-public-net #查看所有oai docker网桥信息

docker network ls  #可以列出当前宿主机上运行的docker网路。

docker exec oai-ext-dn ping 12.1.1.4 -c4  #应当尝试的命令

docker compose up  #运行依靠两个文件：Dockerfile和docker-compose.yml。Dockerfile指定docker创建的环境和编译器的依赖，docker-compose.yml中指示了如何构建相应的docker容器，包括地址端口等很多信息。
```

![Alt text](image-23.png)

在OAI CN中主要模块的地址：

* AMF 192.168.70.132
* UPF(SPGWU) 192.168.70.134
* SMF 192.168.70.133
* NRF 192.168.70.130
* oai-ext-dn 192.168.70.135

