##### 1.1、32位机内存分配图

![](内存分配-01.png)



##### 1.2、内存四区

一般来说，内存主要分为如下左图==代码区和数据区==；数据区又分为==静态和动态区==。

右图为C语言中的内存四区的大致分布情况，自下而上地址递增。

<div style="display: flex; justify-content: space-between;">
    <img src="内存分配-02.png" style="zoom:90%;" />
    <img src="内存分配-03.png" style="zoom:90%;" />
</div>

参考链接：[C语言知识整理（3）：内存管理（详细版） - mattran - 博客园 (cnblogs.com)](https://www.cnblogs.com/yif1991/p/5049638.html)

------

##### 2.1 malloc与free

使用前提：需包含<stdlib.h>

malloc() :申请内存空间，位于堆上

free(): 释放内存空间，使得该空间可以再次被操作系统使用

```c
#include <stdio.h>
#include  <stdlib.h>
int main()
{
   char *pt;
   pt= (char *)malloc(10); //堆上申请空间(malloc的输入参数，是申请空间的字节数)
                           //成功 返回值是申请空间的地址，失败返回NULL;
   if(pt ==NULL){ 
      printf("申请空间失败");
      return -1;
   }
   *pt=0x11;
   *(pt+1)=0x22;
   printf("%x %x %x \n",pt[0],pt[1],pt[2]);
   free(pt); //释放空间，避免内存泄漏
   pt =NULL; //避免野指针 (迷途指针),操作已释放的空间
   return 0;
}

out: 11 22 0
```



如何用函数实现？

法1️⃣：使用不传参函数

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * getmemory(void)
{
   char* p = NULL;
   p=(char *) malloc(100); 
   strcpy(p,"hello world");
   return p;
}

int main( )
{
   char *str=NULL;
   str = getmemory(); 
   printf("%s\n",str);
   free(str);
   return 0;
}

out: hello world
```



法2️⃣：用指针的指针

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void getmemory(char** p)
{
   *p=(char *) malloc(100); 
   strcpy(*p,"hello world");
}

int main( )
{
   char *str=NULL;
   getmemory(&str); 
   printf("%s\n",str);
   free(str);
   return 0;
}
out: hello world
```

最后，

思考以下代码：

```c
//问: 输出结果是什么? 为什么
#include <stdio.h>
#include <stdlib.h>
void GetMemory(char **p,int num){                          
   *p=(char *)malloc(num); 
}  
int main(){
   char *str=NULL;
   GetMemory(&str,100);  
   strcpy(str,"hello");
   free(str);
   if(str!=NULL)
   {
      strcpy(str,"world");
   }   
   printf("\n str is %s",str);
}
```

 答案：输出str is world。

- 进程中的内存管理 是由库函数完成，不是由操作系统。

- malloc时 库函数 会向操作系统申请空间，并做记录 但free时，==库函数 不会把空间马上还给操作系统==，还能继续访问。 

- 但后面可能这个空间分配给别人。 所以==为避免误操作 free(str) 后 需 str=NULL==;



##### 2.2 内存泄露

==case1==: 未配对

```c
 char *pt = (char *)malloc(10); //堆上申请空间，未配对free(pt)
```

==case2==: 丢失地址

```c
char *pt= (char *)malloc(10); 	
pt= (char *)malloc(20); 	//覆盖了指针，导致前面10个空间的地址丢失。
```

==case3==: 未能分级释放; 当涉及结构嵌套时，使用free进行空间释放时，主要必须从嵌套的最深层使用free()

```c
#include <stdio.h>
#include  <stdlib.h>
struct birth
{
   int year;
   int month;
   int day;
};
struct student
{
   char is_male;
   char *name;
   struct birth * bi;
};
int main()
{
   struct student *pt;
   pt= (struct student *)malloc(sizeof(struct student)); //堆上申请空间

   pt->is_male =1;
   pt->name ="wangwei";
   pt->bi = (struct birth *)malloc(sizeof(struct birth)); //堆上申请空间
   pt->bi->year =2000;
   pt->bi->month =3;  
   pt->bi->day =2;  

   printf("%s %d \n",pt->name,pt->bi->day);

  //逐级释放空间，避免内存泄漏
   //pt->name 是字符串常量 不用释放 
   if(pt->bi!=NULL){ 
      free(pt->bi); //先释放子空间
      pt->bi=NULL;
   }
   free(pt); //后释放父空间
   pt =NULL; //避免野指针 (操作已释放的空间)
   return 0;
}	
```

------

##### 3 栈相关

##### 3.1 初步跟踪

下面以一简单代码为背景，使用gdb调试来分析函数是如何在栈中被调用，以期加深对c代码执行的理解；代码如下：

```c
#include <stdio.h>

void foo(int a, int b) {
    int c = a + b;
    printf("Result: %d\n", c);
}

int main() {
    int x = 3;
    int y = 4;
    foo(x, y);
    return 0;
}
```

首先，分别依次执行以下命令：

` gcc -g hello.c`

`gdb a.out`

`(gdb) b main`：main函数处打断点

`(gdb) b foo`：foo函数处打断点

`(gdb) r` :运行至main函数处

`(gdb) disas main`:反汇编main函数

下图分别为main函数和foo函数的汇编代码，最左侧为代码地址

<div style="display: flex; justify-content: space-between;">
    <img src="Stack-01.png" style="zoom:70%;" />
    <img src="Stack-02.png" style="zoom:60%;" />
</div>

此刻程序依然在main函数中，下面查看当前栈帧信息，命令如下

`(gdb) i frame`

```html
Breakpoint 1, main () at hello.c:8
8	int main() {
(gdb) i frame
Stack level 0, frame at 0x7fffffffe000:
 rip = 0x55555555517f in main (hello.c:8); saved rip = 0x7ffff7dea083
 source language c.
 Arglist at 0x7fffffffdff0, args: 
 Locals at 0x7fffffffdff0, Previous frame's sp is 0x7fffffffe000
 Saved registers:
  rip at 0x7fffffffdff8
```

**栈帧信息说明**：

`0x7fffffffe000 `为当前栈帧在内存中的起始地址；

`rip = 0x55555555517f` 指令指针寄存器，保存了当前正在执行的指令地址，结合汇编代码可知，其值为main函数地址；

`saved rip = 0x7ffff7dea083` 这是调用此函数时保存的返回地址。即，当`main`函数执行完毕时，程序将跳转回这个地址继续执行；

`Locals at 0x7fffffffdff0` 这个地址是局部变量所在的栈地址；

`Saved registers: rip at 0x7fffffffdff8` 指令指针寄存器存储的值在栈中的地址。



继续步进调试代码，进入`foo`函数，查看栈帧信息：

`(gdb) i frame`

```html
(gdb) i frame
Stack level 0, frame at 0x7fffffffdfe0:
 rip = 0x555555555149 in foo (hello.c:3); saved rip = 0x5555555551a8
 called by frame at 0x7fffffffe000
 source language c.
 Arglist at 0x7fffffffdfd0, args: a=21845, b=1431654909
 Locals at 0x7fffffffdfd0, Previous frame's sp is 0x7fffffffdfe0
 Saved registers:
  rip at 0x7fffffffdfd8
```

**栈帧信息说明**：(解释与main函数不同点)

`called by frame at 0x7fffffffe000` ：这里的`0x7fffffffe000`就是`main`函数的栈帧起始地址，说明本栈帧由main函数栈帧调用；

`saved rip = 0x5555555551a8`：指令指针寄存器存储的值，为函数调用后，主函数中的该被调函数代码的下一句代码的地址，从而实现函数返回后，可以继续执行剩余代码。



使用命令查看地址映射：(可以查看整个程序在系统中内存分配情况)

`(gdb) info proc mappings`

![](Stack-03.png)



##### 3.2 高级跟踪

==入栈顺序：从下到上，从右至左==，**注意**：下图中的“返回地址”和“前栈栈底rbp”需要颠倒一下。

![](Stack-04.png)

代码如下：

```c
#include <stdio.h>
int func(int x,int y){
   char name[6];
   int sum = 0; 
   printf("Type you name: "); 
   gets(name); //输入字符过多，name会越界，栈溢出崩溃
   return x+y;
}

int main (int argc,char *argv[]) {
    func(3,4);
    return 0;
}
```

注意编译时关闭栈保护：`gcc -g -fno-stack-protector -z execstack  main.c`

反汇编结果如下：

![](Stack-06.png)

**设置main函数的断点，起初栈中只有main函数信息如下**：（命令：`bt full`）

```bash
(gdb) bt full
#0  main (argc=1, argv=0x7fffffffe0e8) at hello.c:12
No locals.
```

查看此时栈底（`x $rbp`）

```bash
(gdb) x $rbp
0x7fffffffdff0:	0x00000000
```

查看栈顶：

```bash
(gdb) x/8 $rsp
0x7fffffffdfe0:	0xffffe0e8	0x00007fff	0x00000000	0x00000001
0x7fffffffdff0:	0x00000000	0x00000000	0xf7dea083	0x00007fff
```

查看地址映射：（栈起始地址为：`0x7ffffffde000`）

![](Stack-07.png)

**运行进入func函数， 查看栈帧信息**：

```bash
(gdb) bt full
#0  func (x=21845, y=1431654957) at hello.c:2
        name = "\000\000\340QUU"
        sum = 21845
#1  0x00005555555551d0 in main (argc=1, argv=0x7fffffffe0e8) at hello.c:12
No locals.
```

查看当前栈底：

```bash
(gdb) x/12xw $rbp
0x7fffffffdfd0:	0xffffdff0	0x00007fff	0x555551d0	0x00005555
0x7fffffffdfe0:	0xffffe0e8	0x00007fff	0x00000000	0x00000001
0x7fffffffdff0:	0x00000000	0x00000000	0xf7dea083	0x00007fff
```

查看当前栈的栈顶：

```
(gdb) x/20xw $rsp
0x7fffffffdfb0:	0xffffdfd6	0x00007fff	0x00000004	0x00000003
0x7fffffffdfc0:	0xf7fb72e8	0x00007fff	0x555551e0	0x00000000
0x7fffffffdfd0:	0xffffdff0	0x00007fff	0x555551d0	0x00005555
0x7fffffffdfe0:	0xffffe0e8	0x00007fff	0x00000000	0x00000001
0x7fffffffdff0:	0x00000000	0x00000000	0xf7dea083	0x00007fff
```

查看sum值

```bash
(gdb) x &sum
0x7fffffffdfcc:	0x00005555
```

查看name值

```
(gdb) x &name
0x7fffffffdfc6:	0x51e00000
```

结合以上内容，总结程序栈内容如下

```html
--------4G---------0xFFFFFFFF  高地址
|      -----      | 0xffffffffff601000  
|       内核      |   
|      -----      | 0xffffffffff600000  //内核系统调用 起始地址
|                 |   
========3G=========0xC0000000
|                 |
|-----------------| 0x7ffffffde000  //系统栈的起始地址: 栈是向下生长
|    栈(stack)    |    
|    --------     | 0x7fffffffdff0  //main函数的栈底
|   0x00000001    |   //argc=1   调用main函数时入栈 
|   0x00000000    |   //字节对齐  
|   0x00007fff    |   //argv=0x7fffffffe0e8  指针数组首地址(8字节，64位机，小端)
|   0xffffe0e8    |
|    -------      |   //当main函数栈进完后，会把下一地址存入rbp中，作为新栈的栈底
|   0x00005555    |  //返回地址(0x00005555555551d0)： 调用func后，返回到main的地址
|   0x555551d0    |
|   0x00007fff    | 0x7fffffffdfd0 //func函数的栈底：存储的上一栈的栈底(main函数的栈底 8字节)
|   0xffffdff0    |  
|   0x00005555    |  //sum 局部变量入栈   ->  顺序是从下到上，从右至左，反向进栈
|   0x51e00000    |  //name[6]
|   0x00007fff    |
|   0xf7fb72e8    |  //补全
|   0x00000003    |  //实参拷贝到形参 b=4 a=3; 
|   0x00000004	  |                
|-----------------| 0x7ffffffff000 
|                 |
|      .....      |          
|                 |
|-----------------|
|    堆(heap)     |   
-------------------
|    DATA数据段   | 
-------------------
|                 |
|      -----      | 0x555555558000 
|   代码区(只读)   | 
|      -----      | 0x555555554000  //a.out 程序的 起始地址
|                 | 
-------------------0x00000000 低地址		
```

------

##### 4、大小端

这部分关注内容为：数据在内存中存储时，它的地址高低问题；比如对于一个`int`型数据，按照从右到左的顺序，内存地址是增加还是减少？

- 小端（较多采用）：将低序字节存储在起始地址，这称为小端(little-endian)字节序
- 大端：将高序字节存储在起始地址，这称为大端(big-endian)字节序

假如现有一32位`int`型数`0x12345678`，那么其MSB(Most Significant Byte，最高有效字节)为`0x12`，其LSB (Least Significant Byte，最低有效字节)为`0x78`，在CPU内存中有两种存放方式：（假设从地址`0x4000`开始存放），则大小端区别如下：

![](大小端.png)

```c
#include <stdio.h>

int main () {
    int a = 0x12345678;
    char* b = (unsigned char *)&a;
    printf("%x\n", *(b+1));
    return 0;
}
out: 56
```

------

##### 5、指针偏移

==指针偏移多少是由它指向的数据类型决定==

```c
#include <stdio.h>
int main () {
    int a[5] = {1,2,3,4,5};
    int *ptr = (int *)(&a+1);
    printf("%d, %d\n", *(a+1), *(ptr-1));
    return 0;
}

out:2, 5
```

注意：上面代码中的`&a+1`，这里加一其实地址增加了20个字节，也即偏移了整个数组的大小。

（**这里不要和数组名也是地址搞混淆，记住`&a`表示取整个数组的地址，而`a`只是数组第一个元素的地址**）

当去掉上述代码中的第4行的指针类型强转，也即

```c
#include <stdio.h>
int main () {
    int a[5] = {1,2,3,4,5};
    int *ptr = (&a+1);
    printf("%d, %d\n", *(a+1), *(ptr-1));
    return 0;
}
```

运行出现warning（指针类型不匹配）:

**warning: initialization of ‘int \*’ from incompatible pointer type ‘int (*)[5]’ [-Wincompatible-pointer-types]**

```html
//例:指针偏移 + 类型转换
//问：下题输出信息是？
int a[5]={1,2,3,4,5}; 
int *ptr=(int *)(&a+1); 
	//&a:取数组的地址。    
	//&a+1:下一个对象的地址(对象是 数组类型，长度单位 是 4*5,偏移1个数组的大小),
	//指针的偏移: 是以它指向的对象的数据类型的长度为偏移的单位
	//          指针的类型就像人的身份，你是以个人，还是老板的身份
	//          不同的身份，你的贷款额度不一样
	//(int *): 把指向数组类型的地址，强转为指向int型的地址	                        
	//int 型是4 ，double型是8  数组型是数组长度。	                        
printf("%d,%d",*(a+1),*(ptr-1)); 
	//a: 数组名，是数组首个元素的地址，指向的数据类型是元素的类型 int
	//a+1:  下一元素的地址，即a[1]  (对象是 int类型，长度单位是 4)    
	//*(a+1): a地址偏移1个单位(int 是4)，再取里面内容(int型), 是2
	//ptr: 是指向int型的指针
	//ptr-1: 地址减1个单位(int型是4)
	//*(ptr-1): 取里面内容(int型)   是5
```

##### 6、
