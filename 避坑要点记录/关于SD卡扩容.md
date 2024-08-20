### 扩容Jetbot镜像

该扩容方法是针对SD卡或者U盘空间足够，但是镜像烧录后分配的空间小于实际存储设备的空间的。 出现这种情况的原因是提供的Jetbot镜像文件是基于32G的SD卡配置，导致分区和容量大小被固定。可以通过扩容脚本将文件系统扩容到完整的存储空间大小，
打开终端，输入以下指令

```
cd /usr/lib/nvidia/resizefs/
sudo chmod 777 nvresizefs.sh
sudo ./nvresizefs.sh
sudo reboot
```

出处：[JetBot AI Kit 教程二、安装镜像 - Waveshare Wiki](https://www.waveshare.net/wiki/JetBot_AI_Kit_教程二、安装镜像)