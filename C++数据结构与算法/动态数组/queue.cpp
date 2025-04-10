// 用链表作为底层数据结构实现队列
#include <iostream>
#include <list>


template<typename E>
class MyLinkedQueue {
private:
    std::list<E> list;

public:
    // 向队尾插入元素，时间复杂度 O(1)
    void push(const E &e) {
        list.push_back(e);
    }

    // 从队头删除元素，时间复杂度 O(1)
    E pop() {
        E front = list.front();
        list.pop_front();
        return front;
    }

    // 查看队头元素，时间复杂度 O(1)
    E peek() {
        return list.front();
    }

    // 返回队列中的元素个数，时间复杂度 O(1)
    int size() {
        return list.size();
    }
};

int main() {
    MyLinkedQueue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);

    std::cout << queue.peek() << std::endl; // 1
    std::cout << queue.pop() << std::endl; // 1
    std::cout << queue.pop() << std::endl; // 2
    std::cout << queue.peek() << std::endl; // 3

    return 0;
}