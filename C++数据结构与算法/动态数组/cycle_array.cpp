#include <iostream>
#include <stdexcept>
#include <ostream>

template<typename T>
class CycleArray {
    std::unique_ptr<T[]> arr; // ʹ������ָ�����̬����(����������ʱ�Զ����� delete[]���ͷ������ڴ�)
    int start; // ����ͷ��ָ��
    int end; // ����β��ָ��
    int count;
    int size;

    // �Զ������ݸ�������
    void resize(int newSize) {
        // �����µ�����
        std::unique_ptr<T[]> newArr = std::make_unique<T[]>(newSize);
        // ���������Ԫ�ظ��Ƶ���������
        for (int i = 0; i < count; ++i) {
            newArr[i] = arr[(start + i) % size];
        }
        arr = std::move(newArr); // �������������Ȩת�Ƶ� arr
        // ���� start �� end ָ��
        start = 0;
        end = count;
        size = newSize;
    }

public:
    CycleArray() : CycleArray(1) { //�޲ι��캯��, ������һ�����캯��(��ʼ��ʱ�Զ�����)
        // Ĭ�ϴ�СΪ 1
    }

    //��Ա��ʼ���б�
    explicit CycleArray(int size) : start(0), end(0), count(0), size(size) {
        arr = std::make_unique<T[]>(size);
    }

    // ������ͷ�����Ԫ�أ�ʱ�临�Ӷ� O(1)
    void addFirst(const T &val) {
        // ��������ʱ������Ϊԭ��������
        if (isFull()) {
            resize(size * 2);
        }
        // ��Ϊ start �Ǳ����䣬���������ƣ��ٸ�ֵ
        start = (start - 1 + size) % size;
        arr[start] = val;
        count++;
    }

    // ɾ������ͷ��Ԫ�أ�ʱ�临�Ӷ� O(1)
    void removeFirst() {
        if (isEmpty()) {
            throw std::runtime_error("Array is empty");
        }
        // ��Ϊ start �Ǳ����䣬�����ȸ�ֵ��������
        arr[start] = T();
        start = (start + 1) % size;
        count--;
        // �������Ԫ���������ٵ�ԭ��С���ķ�֮һ�����С�����СΪһ��
        if (count > 0 && count == size / 4) {
            resize(size / 2);
        }
    }

    // ������β�����Ԫ�أ�ʱ�临�Ӷ� O(1)
    void addLast(const T &val) {
        if (isFull()) {
            resize(size * 2);
        }
        // ��Ϊ end �ǿ����䣬�������ȸ�ֵ��������
        arr[end] = val;
        end = (end + 1) % size;
        count++;
    }

    // ɾ������β��Ԫ�أ�ʱ�临�Ӷ� O(1)
    void removeLast() {
        if (isEmpty()) {
            throw std::runtime_error("Array is empty");
        }
        // ��Ϊ end �ǿ����䣬���������ƣ��ٸ�ֵ
        end = (end - 1 + size) % size;
        arr[end] = T();
        count--;
        // ����
        if (count > 0 && count == size / 4) {
            resize(size / 2);
        }
    }

    // ��ȡ����ͷ��Ԫ�أ�ʱ�临�Ӷ� O(1)
    T getFirst() const {
        if (isEmpty()) {
            throw std::runtime_error("Array is empty");
        }
        return arr[start];
    }

    // ��ȡ����β��Ԫ�أ�ʱ�临�Ӷ� O(1)
    T getLast() const {
        if (isEmpty()) {
            throw std::runtime_error("Array is empty");
        }
        // end �ǿ����䣬ָ�������һ��Ԫ�ص�λ�ã�����Ҫ�� 1
        return arr[(end - 1 + size) % size];
    }

    bool isFull() const {
        return count == size;
    }

    int getSize() const {
        return count;
    }

    bool isEmpty() const {
        return count == 0;
    }
};