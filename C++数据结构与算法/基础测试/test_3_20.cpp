#include <iostream>
#include <unordered_map>
using namespace std;

int main() {
    // 初始化哈希表
    unordered_map<int, string> hashmap{{1, "one"}, {2, "two"}, {3, "three"}};

    // 检查哈希表是否为空，输出：0 (false)
    cout << hashmap.empty() << endl;

    // 获取哈希表的大小，输出：3
    cout << hashmap.size() << endl;

    // 获取指定键对应的值，若不存在会返回默认构造的值
    // 输出空字符串
    cout << hashmap[4] << endl;

    // 插入一个新的键值对
    hashmap[4] = "four";

    // 获取新插入的值，输出：four
    cout << hashmap[4] << endl;

    // 删除键值对
    hashmap.erase(3);


    // 遍历哈希表
    // 输出（顺序可能不同）：
    // 4 -> four
    // 2 -> two
    // 1 -> one
    for (const auto &pair: hashmap) {
        cout << pair.first << " -> " << pair.second << endl;
    }

    // 特别注意，访问不存在的键会自动创建这个键
    unordered_map<int, string> hashmap2;

    // 键值对的数量是 0
    cout << hashmap2.size() << endl; // 0

    // 访问不存在的键，会自动创建这个键，对应的值是默认构造的值
    cout << hashmap2[1] << endl; // empty string
    cout << hashmap2[2] << endl; // empty string

    // 现在键值对的数量是 2
    cout << hashmap2.size() << endl; // 2

    return 0;
}