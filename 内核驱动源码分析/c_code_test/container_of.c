#include <stdio.h>
#include <stdlib.h>
/**
 * container_of - ͨ���ṹ���Աָ���ȡ�����ó�Ա�Ľṹ��ָ��
 * @ptr:	ָ��ṹ���Ա��ָ��.
 * @type:	�ǰ����ó�Ա�Ľṹ������.
 * @member:	�ǽṹ���Ա������.
 */

/* 
 *offsetof(type, member) �����Ա�ڽṹ���е�ƫ����
 *((TYPE *)0) �� 0 ת��Ϊ TYPE ���͵�ָ�롣
 * &((TYPE *)0)->MEMBER ��ȡ MEMBER ��Ա�ĵ�ַ��
 * ���ڻ���ַ�� 0�����Գ�Ա�ĵ�ַʵ���Ͼ��ǳ�Ա��ƫ������
 */
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)//��������ȼ�Ϊ��'->' ���ȼ����� '&'

/* 
 * typeof( ((type *)0)->member ) ��ȡ��Ա������
 * (char *)__mptr - offsetof(type, member) ����ṹ�����ʼ��ַ
 * (type *)(...) ��������ĵ�ַת��Ϊ type ���͵�ָ��
 * ע�⣬���ֺ궨�� ʹ�� ({}), ��Ϊ�����ʽ�������ڱ��ʽ��ִ�ж�����䣬��ֵΪ���һ������ֵ
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