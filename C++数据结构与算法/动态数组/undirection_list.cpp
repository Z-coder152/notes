#include <iostream>
#include <vector>

//链表节点定义及初始化（成员初始化列表）
class Listnode{
public:
    int val;
    Listnode *next;
    Listnode(int x): val(x), next(nullptr) {}
};

Listnode * creatLinkedList(std::vector<int> &nums)
{
    Listnode *head = new Listnode(0);
    Listnode *cur = head;
    for(int i = 0; i < nums.size(); i++)
    {
        Listnode *node = new Listnode(nums[i]);
        cur->next = node;
        cur = cur->next;
    }
    return head->next;
}

void printLinkedList(Listnode *head)
{
    Listnode *cur = head;
    while(cur != nullptr)
    {
        std::cout << cur->val << "->";
        cur = cur->next;
    }
}

int main()
{
    std::vector<int> nums = {1, 2, 3, 4, 5};
    Listnode *head = creatLinkedList(nums);
    printLinkedList(head);
    return 0;
}