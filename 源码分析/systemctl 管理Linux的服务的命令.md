启动计算机，需要启动很多服务(Unit)，而传统service命令启动，Runleve是串行启动 慢

而Systemd采用Target(Unit组)方式，启动一个Target时，里面的服务(unit)都会启动，且可以并行启动



$ systemctl list-unit-files --type=target //查看当前系统的所有Target 

$ systemctl get-default  //查看默认启动的Tar(如 graphical.target) $ systemctl list-dependencies multi-user.target //查看target中包含的服务(unit) 

​               //也可直接查看 /etc/systemd/system/multi-user.target.wants 文件夹           $ cd 	/etc/systemd/system/   

$ ls -l default.target  //开机时默认启动的服务   

//default.target -> /lib/systemd/system/graphical.target  软链接到 图形界面   

//通常符号链接到graphical.target(图形界面)或者multi-user.target(多用户命令行)    

//可通过更改软连接方式，指定默认启动的服务   

//或通过命令 $ sudo systemctl set-default multi-user.target





$ systemctl daemon-reload  //重载系统服务 

$ systemctl enable *.service  //设置开机启动某服务 

$ systemctl disable *.service //不开机启动某服务 

$ systemctl start *.service   //立即启动服务 

$ systemctl stop *.service    //停止服务 

$ systemctl restart *.service //重启服务 

$ systemctl is-enabled nginx.service //查看服务是否开机启动 

$ systemctl list-unit-files|grep enabled //查看已启动的服务列表

 $ systemctl --failed	 //查看启动失败的服务列表