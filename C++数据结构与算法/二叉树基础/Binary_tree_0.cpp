#include <iostream>
#include <vector>
#include <new>

class TreeNode{
public: 
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x): val(x), left(nullptr), right(nullptr){}
};


void preOrder(TreeNode* node) {
    if (node == nullptr) return;
    std::cout << node->val << " ";
    preOrder(node->left);
    preOrder(node->right);
}
void inOrder(TreeNode* node) {
    if (node == nullptr) return;
    inOrder(node->left);
    std::cout << node->val << " ";
    inOrder(node->right);

}

void postOrder(TreeNode* node) {
    if (node == nullptr) return;
    postOrder(node->left);
    postOrder(node->right);
    std::cout << node->val << " ";
}

int main() {
    TreeNode* root = new TreeNode(0); // �������ڵ�
    root->left = new TreeNode(1); // �������ӽڵ�
    root->right = new TreeNode(2); // �������ӽڵ�
    root->left->left = new TreeNode(3); // �������ӽڵ�����ӽڵ�
    root->left->right = new TreeNode(4); // �������ӽڵ�����ӽڵ�
    root->right->left = new TreeNode(5); // �������ӽڵ�����ӽڵ�
    root->right->right = new TreeNode(6); // �������ӽڵ�����ӽڵ�

    std::cout << "Pre-order Traversal: ";
    preOrder(root);
    std::cout << std::endl;

    std::cout << "In-order Traversal: ";
    inOrder(root);
    std::cout << std::endl;

    std::cout << "Post-order Traversal: ";
    postOrder(root);
    std::cout << std::endl;

    return 0;
}
