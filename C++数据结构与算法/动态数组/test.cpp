#include <iostream>
#include <stdexcept>
#include <vector>

/* 一个模板声明，用于定义一个 类模板 或 函数模板。
 *在这里，它声明了一个泛型类型 E，允许类或函数在定义时不指定具体的数据类型，
 *而是在使用时由用户指定具体的类型*/
template<typename E> 
class MyArrayList {
private:
    // 真正存储数据的底层数组
    E* data;
    // 记录当前元素个数
    int size;

    // 最大元素容量
    int cap;

    // 默认初始容量
    static const int INIT_CAP = 1;

public:
    //构造函数，初始化工作由编译器自动完成
    MyArrayList() {
        this->data = new E[INIT_CAP]; //分配一个大小为 INIT_CAP 的数组，INIT_CAP 是一个静态常量，初始值为 1
        this->size = 0;
        this->cap = INIT_CAP;
    }

    MyArrayList(int initCapacity) {
        this->data = new E[initCapacity];
        this->size = 0;
        this->cap = initCapacity;
    }

    // 增
    void addLast(E e) {
        // 看 data 数组容量够不够
        if (size == cap) {
            resize(2 * cap);
        }
        // 在尾部插入元素
        data[size] = e;
        size++;
    }

    void add(int index, E e) {
        // 检查索引越界
        checkPositionIndex(index);

        // 看 data 数组容量够不够
        if (size == cap) {
            resize(2 * cap);
        }

        // 搬移数据 data[index..] -> data[index+1..]
        // 给新元素腾出位置
        for (int i = size - 1; i >= index; i--) {
            data[i + 1] = data[i];
        }

        // 插入新元素
        data[index] = e;

        size++;
    }

    void addFirst(E e) {
        add(0, e);
    }

    // 删
    E removeLast() {
        if (size == 0) {
            throw std::out_of_range("NoSuchElementException");
        }
        // 可以缩容，节约空间
        if (size == cap / 4) {
            resize(cap / 2);
        }

        E deletedVal = data[size - 1];
        // 删除最后一个元素
        // 必须给最后一个元素置为 null，否则会内存泄漏
        data[size - 1] = E();
        size--;

        return deletedVal;
    }

    E remove(int index) {
        // 检查索引越界
        checkElementIndex(index);

        // 可以缩容，节约空间
        if (size == cap / 4) {
            resize(cap / 2);
        }

        E deletedVal = data[index];

        // 搬移数据 data[index+1..] -> data[index..]
        for (int i = index + 1; i < size; i++) {
            data[i - 1] = data[i];
        }

        data[size - 1] = E();
        size--;

        return deletedVal;
    }

    E removeFirst() {
        return remove(0);
    }

    // 查
    E get(int index) {
        // 检查索引越界
        checkElementIndex(index);

        return data[index];
    }

    // 改
    E set(int index, E element) {
        // 检查索引越界
        checkElementIndex(index);
        // 修改数据
        E oldVal = data[index];
        data[index] = element;
        return oldVal;
    }

    // 工具方法
    int getSize() {
        return size;
    }

    bool isEmpty() {
        return size == 0;
    }

    // 将 data 的容量改为 newCap
    void resize(int newCap) {
        E* temp = new E[newCap];

        for (int i = 0; i < size; i++) {
            temp[i] = data[i];
        }

        // 释放原数组内存
        delete[] data;

        data = temp;
        cap = newCap;
    }

    bool isElementIndex(int index) {
        return index >= 0 && index < size;
    }

    bool isPositionIndex(int index) {
        return index >= 0 && index <= size;
    }

    // 检查 index 索引位置是否可以存在元素
    void checkElementIndex(int index) {
        if (!isElementIndex(index)) {
            throw std::out_of_range("Index out of bounds");
        }
    }

    // 检查 index 索引位置是否可以添加元素
    void checkPositionIndex(int index) {
        if (!isPositionIndex(index)) {
            throw std::out_of_range("Index out of bounds");
        }
    }

    void display() {
        std::cout << "size = " << size << " cap = " << cap << std::endl;
        for (int i = 0; i < size; i++) {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;
    }

    ~MyArrayList() {
        delete[] data;
    }
};


int main() {
    // 初始容量设置为 3
    MyArrayList<int> arr(3);

    // 添加 5 个元素
    for (int i = 1; i <= 5; i++) {
        arr.addLast(i);
    }

    arr.remove(3);
    arr.add(1, 9);
    arr.addFirst(100);
    int val = arr.removeLast();

    // 100 1 9 2 3
    for (int i = 0; i < arr.getSize(); i++) {
        std::cout << arr.get(i) << std::endl;
    }

    return 0;
}