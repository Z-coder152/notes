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
    TreeNode* root = new TreeNode(0); // 创建根节点
    root->left = new TreeNode(1); // 创建左子节点
    root->right = new TreeNode(2); // 创建右子节点
    root->left->left = new TreeNode(3); // 创建左子节点的左子节点
    root->left->right = new TreeNode(4); // 创建左子节点的右子节点
    root->right->left = new TreeNode(5); // 创建右子节点的左子节点
    root->right->right = new TreeNode(6); // 创建右子节点的右子节点

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
