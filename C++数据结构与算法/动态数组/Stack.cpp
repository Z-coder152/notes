// 用链表作为底层数据结构实现栈
#include <list>
#include <iostream>

template<typename E>
class MyLinkedStack {
private:
    std::list<E> list;

public:
    // 向栈顶加入元素，时间复杂度 O(1)
    void push(const E &e) {
        list.push_back(e);
    }

    // 从栈顶弹出元素，时间复杂度 O(1)
    E pop() {
        E value = list.back();
        list.pop_back();
        return value;
    }

    // 查看栈顶元素，时间复杂度 O(1)
    E peek() const { // const 修饰成员函数，表示该函数不会修改类的成员变量
        return list.back();
    }

    // 返回栈中的元素个数，时间复杂度 O(1)
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