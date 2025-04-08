#include <iostream>
#include <unordered_map>
using namespace std;

int main() {
    // ��ʼ����ϣ��
    unordered_map<int, string> hashmap{{1, "one"}, {2, "two"}, {3, "three"}};

    // ����ϣ���Ƿ�Ϊ�գ������0 (false)
    cout << hashmap.empty() << endl;

    // ��ȡ��ϣ��Ĵ�С�������3
    cout << hashmap.size() << endl;

    // ��ȡָ������Ӧ��ֵ���������ڻ᷵��Ĭ�Ϲ����ֵ
    // ������ַ���
    cout << hashmap[4] << endl;

    // ����һ���µļ�ֵ��
    hashmap[4] = "four";

    // ��ȡ�²����ֵ�������four
    cout << hashmap[4] << endl;

    // ɾ����ֵ��
    hashmap.erase(3);


    // ������ϣ��
    // �����˳����ܲ�ͬ����
    // 4 -> four
    // 2 -> two
    // 1 -> one
    for (const auto &pair: hashmap) {
        cout << pair.first << " -> " << pair.second << endl;
    }

    // �ر�ע�⣬���ʲ����ڵļ����Զ����������
    unordered_map<int, string> hashmap2;

    // ��ֵ�Ե������� 0
    cout << hashmap2.size() << endl; // 0

    // ���ʲ����ڵļ������Զ��������������Ӧ��ֵ��Ĭ�Ϲ����ֵ
    cout << hashmap2[1] << endl; // empty string
    cout << hashmap2[2] << endl; // empty string

    // ���ڼ�ֵ�Ե������� 2
    cout << hashmap2.size() << endl; // 2

    return 0;
}