#include <iostream>
#include <stdexcept>
#include <vector>

/* һ��ģ�����������ڶ���һ�� ��ģ�� �� ����ģ�塣
 *�������������һ���������� E������������ڶ���ʱ��ָ��������������ͣ�
 *������ʹ��ʱ���û�ָ�����������*/
template<typename E> 
class MyArrayList {
private:
    // �����洢���ݵĵײ�����
    E* data;
    // ��¼��ǰԪ�ظ���
    int size;

    // ���Ԫ������
    int cap;

    // Ĭ�ϳ�ʼ����
    static const int INIT_CAP = 1;

public:
    //���캯������ʼ�������ɱ������Զ����
    MyArrayList() {
        this->data = new E[INIT_CAP]; //����һ����СΪ INIT_CAP �����飬INIT_CAP ��һ����̬��������ʼֵΪ 1
        this->size = 0;
        this->cap = INIT_CAP;
    }

    MyArrayList(int initCapacity) {
        this->data = new E[initCapacity];
        this->size = 0;
        this->cap = initCapacity;
    }

    // ��
    void addLast(E e) {
        // �� data ��������������
        if (size == cap) {
            resize(2 * cap);
        }
        // ��β������Ԫ��
        data[size] = e;
        size++;
    }

    void add(int index, E e) {
        // �������Խ��
        checkPositionIndex(index);

        // �� data ��������������
        if (size == cap) {
            resize(2 * cap);
        }

        // �������� data[index..] -> data[index+1..]
        // ����Ԫ���ڳ�λ��
        for (int i = size - 1; i >= index; i--) {
            data[i + 1] = data[i];
        }

        // ������Ԫ��
        data[index] = e;

        size++;
    }

    void addFirst(E e) {
        add(0, e);
    }

    // ɾ
    E removeLast() {
        if (size == 0) {
            throw std::out_of_range("NoSuchElementException");
        }
        // �������ݣ���Լ�ռ�
        if (size == cap / 4) {
            resize(cap / 2);
        }

        E deletedVal = data[size - 1];
        // ɾ�����һ��Ԫ��
        // ��������һ��Ԫ����Ϊ null��������ڴ�й©
        data[size - 1] = E();
        size--;

        return deletedVal;
    }

    E remove(int index) {
        // �������Խ��
        checkElementIndex(index);

        // �������ݣ���Լ�ռ�
        if (size == cap / 4) {
            resize(cap / 2);
        }

        E deletedVal = data[index];

        // �������� data[index+1..] -> data[index..]
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

    // ��
    E get(int index) {
        // �������Խ��
        checkElementIndex(index);

        return data[index];
    }

    // ��
    E set(int index, E element) {
        // �������Խ��
        checkElementIndex(index);
        // �޸�����
        E oldVal = data[index];
        data[index] = element;
        return oldVal;
    }

    // ���߷���
    int getSize() {
        return size;
    }

    bool isEmpty() {
        return size == 0;
    }

    // �� data ��������Ϊ newCap
    void resize(int newCap) {
        E* temp = new E[newCap];

        for (int i = 0; i < size; i++) {
            temp[i] = data[i];
        }

        // �ͷ�ԭ�����ڴ�
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

    // ��� index ����λ���Ƿ���Դ���Ԫ��
    void checkElementIndex(int index) {
        if (!isElementIndex(index)) {
            throw std::out_of_range("Index out of bounds");
        }
    }

    // ��� index ����λ���Ƿ�������Ԫ��
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
    // ��ʼ��������Ϊ 3
    MyArrayList<int> arr(3);

    // ��� 5 ��Ԫ��
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