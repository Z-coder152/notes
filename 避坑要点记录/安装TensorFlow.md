#### 使用以下命令进行安装

```
sudo apt-get install libhdf5-serial-dev hdf5-tools libhdf5-dev zlib1g-dev zip libjpeg8-dev liblapack-dev libblas-dev gfortran
sudo apt-get install python3-pip
sudo pip3 install -U pip
sudo pip3 install -U pip testresources setuptools numpy==1.16.1 future==0.17.1 mock==3.0.5 h5py==2.9.0 keras_preprocessing==1.0.5 keras_applications==1.0.8 gast==0.2.2 futures protobuf pybind11
# TF-2.x
$ sudo pip3 install --pre --extra-index-url https://developer.download.nvidia.com/compute/redist/jp/v44 tensorflow==2.3.1+nv20.12
# TF-1.15
$ sudo pip3 install --pre --extra-index-url https://developer.download.nvidia.com/compute/redist/jp/v44 ‘tensorflow<2’
```

**问题1**：安装h5py时出现 “Building wheel for h5py (setup.py) ... error”

​		解决：降级cython到3.0以下，接着在使用上面的Pip命令，安装最终自动通过另一种方式“h5py was installed using the legacy 'setup.py install' method”，可以成功实现。

**问题2**：以上AI包在安装完成后，python3环境下导入出现“Illegal instruction (core dumped)”问题

​		解决：修改~/.bashrc文件，在其文件末尾新加一行“export OPENBLAS_CORETYPE=ARMV8”，接着在终端运行“source ~/.bashrc”，可以重新加载 `.bashrc` 文件，使其更改立即生效。



参考文献：

[Installing TensorFlow for Jetson Platform - NVIDIA Docs](https://docs.nvidia.com/deeplearning/frameworks/install-tf-jetson-platform/index.html#troubleshooting)

[jetson nano 安装 tensorflow - 侬本多情2019 - 博客园 (cnblogs.com)](https://www.cnblogs.com/ydthu/p/14965755.html)

[Failed building wheel of h5py - Jetson & Embedded Systems / Jetson Nano - NVIDIA Developer Forums](https://forums.developer.nvidia.com/t/failed-building-wheel-of-h5py/263322)

[Ubuntu/Jetson Nano问题解决“Illegal instruction(core dumped)”_illegal instruction (core dumped)-CSDN博客](https://blog.csdn.net/LYiiiiiii/article/details/119133826)

