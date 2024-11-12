#include <iostream>

using namespace std;

template <typename T>
struct TreeNode {
    T value;
    TreeNode* left = nullptr;
    //cout << "Node "s << left << " created"s;
    TreeNode* right = nullptr;
    //cout << "Node "s << right << " created"s;
};

template <typename T>
void DeleteTree(TreeNode<T>* node) {
    if (!node) {
        return;
    }
    DeleteTree(node->left);
    DeleteTree(node->right);
    cout << "Node "s << node << " deleted"s << endl;
    delete node;
}

int main() {
    using T = TreeNode<int>;
    // Здесь гарантируется порядок вычислений: new T{4}, new T{7}, new T{6, ...}
    T* root1 = new T{6, new T{4}, new T{7}}; // Здесь спрятались возможные утечки памяти
    DeleteTree(root1);
}