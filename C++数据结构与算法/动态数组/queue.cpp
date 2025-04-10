// ��������Ϊ�ײ����ݽṹʵ�ֶ���
#include <iostream>
#include <list>


template<typename E>
class MyLinkedQueue {
private:
    std::list<E> list;

public:
    // ���β����Ԫ�أ�ʱ�临�Ӷ� O(1)
    void push(const E &e) {
        list.push_back(e);
    }

    // �Ӷ�ͷɾ��Ԫ�أ�ʱ�临�Ӷ� O(1)
    E pop() {
        E front = list.front();
        list.pop_front();
        return front;
    }

    // �鿴��ͷԪ�أ�ʱ�临�Ӷ� O(1)
    E peek() {
        return list.front();
    }

    // ���ض����е�Ԫ�ظ�����ʱ�临�Ӷ� O(1)
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