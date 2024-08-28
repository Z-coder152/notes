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

