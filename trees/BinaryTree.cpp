#include <iostream>
#include <functional>
#include <vector>
#include <queue>
#include <stack>
#include <iterator>
#include <algorithm>
#include <memory>

/* BinaryTree
 * Наивная реализация бинарного дерева
 *
 * @tparam T                    тип данных
 * @tparam Comparator           функтор сравнения
 */
template<class T, class Comparator>
class BinaryTree {
public:
    BinaryTree(const BinaryTree &) = delete;
    BinaryTree &operator=(const BinaryTree &) = delete;

    explicit BinaryTree();
    BinaryTree(const std::vector<T> &input);

    void insert(const T &new_element);
    int get_depth() const;

    ~BinaryTree();
private:
    struct Node {
        T data;
        Node *left;
        Node *right;
    };

    Node *root;
    Comparator cmp;
};

/* Конструктор
 *
 * @tparam T                    тип данных
 * @tparam Comparator           функтор сравнения
 */
template<class T, class Comparator>
BinaryTree<T, Comparator>::BinaryTree() : root(nullptr) {}

/* Конструктор преобразования из вектора
 *
 * @tparam T                    тип данных
 * @tparam Comparator           функтор сравнения
 * @param input                 вектор элементов, из которых строим дерево
 */
template<class T, class Comparator>
BinaryTree<T, Comparator>::BinaryTree(const std::vector<T> &input)
        : root(nullptr) {
    for (auto it : input) {
        insert(it);
    }
}

/* Наивная нерекурсивная операция вставки в бинарное дерево
 *
 * @tparam T                    тип данных
 * @tparam Comparator           функтор сравнения
 * @param new_element           элемент, который необходимо вставить в дерево
 */
template<class T, class Comparator>
void BinaryTree<T, Comparator>::insert(const T &new_element) {
    if (!root) {
        root = new Node{new_element,  nullptr, nullptr};
        return;
    }

    auto current_node = root;
    while (true) {
        if (!cmp(current_node->data, new_element)) {
            if (current_node->right) {
                current_node = current_node->right;
            } else {
                current_node->right = new Node{new_element, nullptr, nullptr};
                return;
            }
        } else {
            if (current_node->left) {
                current_node = current_node->left;
            } else {
                current_node->left = new Node{new_element, nullptr, nullptr};
                return;
            }
        }
    }
}

/* Нерекурсивное вычисление глубины дерева
 *
 * @tparam T                    тип данных
 * @tparam Comparator           функтор сравнения
 * @return                      глубина дерева
 */
template<class T, class Comparator>
int BinaryTree<T, Comparator>::get_depth() const {
    if (!root) {
        return 0;
    }

    //Поиск глубины дерева поисков в ширину
    int depth = 0;
    std::queue<Node *> queue;
    queue.push(root);

    while (!queue.empty()) {
        depth++;

        //Вынимаем из очереди весь текущий уровень и закидываем следующий
        for (int on_level = (int)queue.size(); on_level > 0; on_level--) {
            auto current_node = queue.front();
            queue.pop();

            if (current_node->left) {
                queue.push(current_node->left);
            }
            if (current_node->right) {
                queue.push(current_node->right);
            }
        }
    }

    return depth;
}

template<class T, class Comparator>
BinaryTree<T, Comparator>::~BinaryTree() {
    std::stack<Node *> dfs_stack;
    dfs_stack.push(root);

    while (!dfs_stack.empty()) {
        Node *current = dfs_stack.top();
        dfs_stack.pop();

        if (current->left) {
            dfs_stack.push(current->left);
        }

        if (current->right) {
            dfs_stack.push(current->right);
        }

        delete current;
    }
}