// ��������Ϊ�ײ����ݽṹʵ��ջ
#include <list>
#include <iostream>

template<typename E>
class MyLinkedStack {
private:
    std::list<E> list;

public:
    // ��ջ������Ԫ�أ�ʱ�临�Ӷ� O(1)
    void push(const E &e) {
        list.push_back(e);
    }

    // ��ջ������Ԫ�أ�ʱ�临�Ӷ� O(1)
    E pop() {
        E value = list.back();
        list.pop_back();
        return value;
    }

    // �鿴ջ��Ԫ�أ�ʱ�临�Ӷ� O(1)
    E peek() const { // const ���γ�Ա��������ʾ�ú��������޸���ĳ�Ա����
        return list.back();
    }

    // ����ջ�е�Ԫ�ظ�����ʱ�临�Ӷ� O(1)
    int size() const {
        return list.size();
    }
};

int main() {
    MyLinkedStack<int> stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);
    stack.push(4);
    while (stack.size() > 0) {
        std::cout << stack.pop() << std::endl;
    }
    return 0;
}