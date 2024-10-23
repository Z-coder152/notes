无法通过SSH远程访问jetson nano，使用SSH命令时，返回“Network unreachable”

1、问题背景

jetson nano 小车有一段时间未使用；

最后一次使用时，关闭VMWARE之前使用了“挂起虚拟机”，而非关机；

这次使用时，直接加载了之前挂起的虚拟机；

2、问题描述

虚拟机加载完成后，像往常一样使用命令：`ssh <login_name>@<ip_address>`，该命令返回：

`ssh: connect to host <login_name>@<ip_address> port 22: Network unreachable`

经确认，登录名和ip地址均没有问题。

3、解决方法

经历了一系列的尝试（包括关防火墙、重启小车），均无果，最后考虑**将虚拟机关机后，重新开机**，问题成功解决。目前不清楚其中的原因，先记录一下，期待以后能解决(✧∀✧)。