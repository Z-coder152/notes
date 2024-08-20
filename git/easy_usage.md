#### 一、本地电脑操作

##### 1、安装后进行设置

```bash
$ git config --global user.name "Your Name"
$ git config --global user.email "email@example.com"
```

##### 2、创建版本库(Repository)

首先通过路径，进入目标文件夹

```bash
$ cd C:/Users/user/my_project
```

键入

```bash
$ git init
```

从而创建`.git`文件

##### 3、添加文件到Git库

​	对单个文件，使用命令：

```bash
$ git add <file>
```

来添加文件；

​	对整个文件夹：

```bash
$ git add .
```

##### 4、提交文件

```bash
$ git commit -m <message>
```

##### 5、查看仓库状态

```bash
$ git status
$ git diff <file_name>
```

##### 6、查看提交日志

```bash
$ git log
```

##### 7、版本回退与后退

首先，Git必须知道当前版本是哪个版本，在Git中，用`HEAD`表示当前版本，也就是最新的提交`1094adb...`（注意我的提交ID和你的肯定不一样），上一个版本就是`HEAD^`，上上一个版本就是`HEAD^^`，当然往上100个版本写100个`^`比较容易数不过来，所以写成`HEAD~100`

版本回退：

```bash
$ git reset --hard HEAD^
```

版本后退（需要知道版本号）：

```bash
$ git reset --hard 1094a
```

注：`1094a`是版本号的前几位，不必写全，可通过`git log`命令查看

##### 8、关于暂存区

首先，工作区就是电脑里能够看到的目录，工作区中的`.git`目录是Git的版本库。

Git的版本库里存了很多东西，其中最重要的就是称为==stage（或者叫index）的暂存区==，还有Git为我们自动创建的第一个分支`master`，以及指向`master`的一个指针叫`HEAD`。

前面讲了我们把文件往Git版本库里添加的时候，是分两步执行的：

第一步是用`git add`把文件添加进去，实际上就是把文件修改添加到暂存区；

第二步是用`git commit`提交更改，实际上就是把暂存区的所有内容提交到当前分支。

<img src="01.png" style="zoom:80%;" />

即：==需要提交的文件修改通通放到暂存区，然后，一次性提交暂存区的所有修改。==

------

#### 二、远程仓库

##### 1、创建SSH Key

```bash
$ ssh-keygen -t rsa -C "youremail@example.com"
```

注：这一步会出现以下问题

<img src="02.png" style="zoom:80%;" />

解决方法：
需要修改文件路径，将上图中白色块部分补充完整即可（如下图）

![](03.png)

出现的乱码可能原因是，路径中包含中文

##### 2、在Github中添加SSH Key

略（可参考[远程仓库 - Git教程 - 廖雪峰的官方网站 (liaoxuefeng.com)](https://liaoxuefeng.com/books/git/remote/index.html)）

##### 3、将本地仓库Push到远程仓库

前提：远程已经建好空仓库

```bash
$ git remote add origin https://github.com/Z-coder152/notes.git
$ git branch -M main
$ git push -u origin main
```

![](04.png)

![](05.png)

从现在起，只要本地作了提交，就可以通过命令：

```bash
$ git push origin master
```

把本地`master`分支的最新修改推送至GitHub