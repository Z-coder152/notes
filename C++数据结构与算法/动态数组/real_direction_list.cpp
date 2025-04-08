#include <iostream>
#include <stdexcept>

template<typename E>
class MyLinkedList{
    //虚拟头尾节点
    struct Node{
        E val;
        Node* next;
        Node* prev;
        Node(E value):val(value),next(nullptr),prev(nullptr){}
    };
    Node* head;
    Node* tail;
    int size;

public:
    //构造函数初始化虚拟头尾节点
    MyLinkedList(){
        head = new Node(E());
        tail = new Node(E());
        head->next = tail;
        tail->prev = head;
        size = 0;
    }
    //下面为链表的基本操作（方法）
    //添加节点到"链表尾部"
    void addLast(E e){
        Node* x = new Node(e);
        Node* temp = tail->prev; //注意 temp就是链表 tail 前一个节点

        temp->next = x;
        x->prev = temp;

        x->next = tail;
        tail->prev = x;

        size++;
    }
    //添加节点到"链表头部"
    void addFirst(E e) {
        Node* x = new Node(e);
        Node* temp = head->next;
        // head <-> temp
        temp->prev = x;
        x->next = temp;

        head->next = x;
        x->prev = head;
        // head <-> x <-> temp
        size++;
    }

    //在非头尾节点， index 处插入节点
    void add(int index, E element) {
        checkPositionIndex(index);
        if (index == size) {
            addLast(element);
            return;
        }

        // 找到 index 对应的 Node
        Node* p = getNode(index);
        Node* temp = p->prev;
        // temp <-> p

        // 新要插入的 Node
        Node* x = new Node(element);

        p->prev = x;
        temp->next = x;

        x->prev = temp;
        x->next = p;

        // temp <-> x <-> p

        size++;
    }

    // ***** 删 *****
    E removeFirst() {
        if (size < 1) {
            throw std::out_of_range("No elements to remove");
        }
        // 虚拟节点的存在是我们不用考虑空指针的问题
        Node* x = head->next;
        Node* temp = x->next;
        // head <-> x <-> temp
        head->next = temp;
        temp->prev = head;

        E val = x->val;  // 保存要返回的值
        delete x;
        // head <-> temp

        size--;
        return val;  // 返回被删除节点的值
    }

    E removeLast() {
        if (size < 1) {
            throw std::out_of_range("No elements to remove");
        }
        Node* x = tail->prev;
        Node* temp = tail->prev->prev;
        // temp <-> x <-> tail

        tail->prev = temp;
        temp->next = tail;

        E val = x->val;
        x->prev = nullptr;
        x->next = nullptr;
        delete x;
        // temp <-> tail

        size--;
        return val;
    }

    E remove(int index) {
        checkElementIndex(index);
        // 找到 index 对应的 Node
        Node* x = getNode(index);
        Node* prev = x->prev;
        Node* next = x->next;
        // prev <-> x <-> next
        prev->next = next;
        next->prev = prev;

        E val = x->val;
        x->prev = nullptr;
        x->next = nullptr;
        delete x;
        // prev <-> next

        size--;
        return val;  // 返回被删除节点的值
    }
    // ***** 查 *****

    E get(int index) {
        checkElementIndex(index);
        // 找到 index 对应的 Node
        Node* p = getNode(index);

        return p->val;
    }

    E getFirst() {
        if (size < 1) {
            throw std::out_of_range("No elements in the list");
        }

        return head->next->val;
    }

    E getLast() {
        if (size < 1) {
            throw std::out_of_range("No elements in the list");
        }

        return tail->prev->val;
    }

    // ***** 改 *****

    E set(int index, E val) {
        checkElementIndex(index);
        // 找到 index 对应的 Node
        Node* p = getNode(index);

        E oldVal = p->val;
        p->val = val;

        return oldVal;
    }

    // ***** 其他工具函数 *****

    int getSize() const {
        return size;
    }

    bool isEmpty() const {
        return size == 0;
    }

    void display() {
        std::cout << "size = " << size << std::endl;
        for (Node* p = head->next; p != tail; p = p->next) {
            std::cout << p->val << " <-> ";
        }
        std::cout << "null" << std::endl;
        std::cout << std::endl;
    }
private:
    Node* getNode(int index) {
        checkElementIndex(index);
        Node* p = head->next;
        // TODO: 可以优化，通过 index 判断从 head 还是 tail 开始遍历
        for (int i = 0; i < index; i++) {
            p = p->next;
        }
        return p;
    }

    bool isElementIndex(int index) const {
        return index >= 0 && index < size;
    }

    bool isPositionIndex(int index) const {
        return index >= 0 && index <= size;
    }

    // 检查 index 索引位置是否可以存在元素
    void checkElementIndex(int index) const {
        if (!isElementIndex(index))
            throw std::out_of_range("Index: " + std::to_string(index) + ", Size: " + std::to_string(size));
    }

    // 检查 index 索引位置是否可以添加元素
    void checkPositionIndex(int index) const {
        if (!isPositionIndex(index))
            throw std::out_of_range("Index: " + std::to_string(index) + ", Size: " + std::to_string(size));
    }

public:
    ~MyLinkedList() {
        while (size > 0) {
            removeFirst();
        }
        delete head;
        delete tail;
    }
        
};

int main() {
    MyLinkedList<int> list;
    list.addLast(1);
    list.addLast(2);
    list.addLast(3);
    list.addFirst(0);
    list.add(2, 5);
    list.display();

    std::cout << "Removed: " << list.removeFirst() << std::endl;
    list.display();

    std::cout << "Removed: " << list.removeLast() << std::endl;
    list.display();

    std::cout << "Removed: " << list.remove(1) << std::endl;
    list.display();

    return 0;
}
