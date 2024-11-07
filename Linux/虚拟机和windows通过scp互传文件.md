先在虚拟机端检查并安装ssh服务，

```bash
sudo apt update        	# 更新数据
sudo apt upgrade        # 更新软件
sudo apt install openssh-server  #下载安装ssh服务的服务器
sudo apt install openssh-client  #下载安装ssh服务的客户端
sudo systemctl start ssh	 # 启动
sudo systemctl stop ssh		 # 停止
sudo systemctl enable ssh 	 # 设置随系统启动时启动
```

若已经安装，则只需启动即可；

windows也需要开启ssh服务；

接着使用命令：（注意：路径不要出现中文）

```bash
scp  root@10.152.255.255:/home/zhao/data/tt.txt /C:/Users/zw/Desktop/summary
或
scp  root@10.152.255.255:/home/zhao/data/test.txt /C:/Users/zw/Desktop/summary/tt.txt
```

root为linux用户名，10.152.255.255为linux的ip， /C:/Users/zw/Desktop/summary/tt.txt（/C:/Users/zw/Desktop/summary）为想要将文件复制到Windows的哪个文件（文件夹）