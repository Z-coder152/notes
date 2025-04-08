#include <stdio.h>
#include <stdlib.h>
/**
 * container_of - 通过结构体成员指针获取包含该成员的结构体指针
 * @ptr:	指向结构体成员的指针.
 * @type:	是包含该成员的结构体类型.
 * @member:	是结构体成员的名称.
 */

/* 
 *offsetof(type, member) 计算成员在结构体中的偏移量
 *((TYPE *)0) 将 0 转换为 TYPE 类型的指针。
 * &((TYPE *)0)->MEMBER 获取 MEMBER 成员的地址。
 * 由于基地址是 0，所以成员的地址实际上就是成员的偏移量。
 */
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)//运算符优先级为：'->' 优先级高于 '&'

/* 
 * typeof( ((type *)0)->member ) 获取成员的类型
 * (char *)__mptr - offsetof(type, member) 计算结构体的起始地址
 * (type *)(...) 将计算出的地址转换为 type 类型的指针
 * 注意，这种宏定义 使用 ({}), 称为语句表达式，可以在表达式中执行多条语句，其值为最后一条语句的值
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

struct class_member {
	int name_1;
	int name_2;
	int name_3;
};

struct member_grade {
	int math;
	int english;
	int chinese;
	int physics;
	int chemistry;
	int biology;
	struct class_member * member;
};

struct member_grade * member_to_grade(struct class_member * member)
{
	return container_of(member, struct member_grade, member);
}

int main(void)
{
	struct class_member * member = malloc(sizeof(struct class_member));
	printf("member address: %p\n", member); //000001e4c6d12430
	struct member_grade * grade = member_to_grade(member);
	printf("grade address: %p\n", grade); //000001e4c6d12420
	free(member);
	return 0;
}