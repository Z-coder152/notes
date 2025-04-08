#include <iostream>
#include <vector>

class DoubleListNode {
public:
    int val;
    DoubleListNode* prev, *next;
    DoubleListNode(int x) : val(x), prev(nullptr), next(nullptr) {};
};

DoubleListNode* creatDoubleLinkedList(std::vector<int>& nums)
{
    DoubleListNode* head = new DoubleListNode(nums[0]);
    DoubleListNode* cur = head;
    for(int i = 1; i < nums.size(); i++)
    {
        DoubleListNode* Node = new DoubleListNode(nums[i]);
        cur->next = Node;
        Node->prev = cur;
        cur = cur->next;
    }
    return head;
}

void order_printDoubleLinkedList(DoubleListNode* head)
{
    DoubleListNode* cur = head;
    while(cur != nullptr)
    {
        std::cout << cur->val << "->";
        cur = cur->next;
    }
    std::cout << "nullptr" << std::endl;
}

void reverse_printDoubleLinkedList(DoubleListNode* head)
{
    DoubleListNode* cur = head;
    while(cur->next != nullptr)
    {
        cur = cur->next;
    }
    while(cur != nullptr)
    {
        std::cout << cur->val << "->";
        cur = cur->prev;
    }
    std::cout << "nullptr" << std::endl;
}

int main()
{
    std::vector<int> nums = {1, 2, 3, 4, 5};
    DoubleListNode* head = creatDoubleLinkedList(nums);
    order_printDoubleLinkedList(head);
    reverse_printDoubleLinkedList(head);
    return 0;
}