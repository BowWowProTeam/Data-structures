#include <functional>
#include <vector>
#include <queue>
#include <stack>
#include <iterator>
#include <algorithm>
#include <memory>

/* Treap
 * Декартово дерево
 *
 * @tparam T                тип хранимых данных
 * @tparam P                тип приоритетов
 * @tparam ComparatorT      функтор сравнения данных
 * @tparam ComparatorP      функтор сравнения приоритетов
 */
template<class T, class P, class ComparatorT, class ComparatorP>
class Treap {
public:
    Treap(const Treap &) = delete;
    Treap &operator=(const Treap &) = delete;

    explicit Treap();

    void insert(const T &new_element, const P &new_priority);
    size_t get_depth() const;
private:
    struct Node {
        T data;
        P priority;

        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
    };

    size_t get_binary_tree_depth_rec(const std::shared_ptr<Node> &) const;
    std::pair<std::shared_ptr<Node>, std::shared_ptr<Node>> split(std::shared_ptr<Treap::Node> &t, const T &key);

    std::shared_ptr<Node> root;
    ComparatorT cmp_data;
    ComparatorP cmp_priority;
};

/* Дефолтный конструктор
 *
 * @tparam T                тип хранимых данных
 * @tparam P                тип приоритетов
 * @tparam ComparatorT      функтор сравнения данных
 * @tparam ComparatorP      функтор сравнения приоритетов
 */
template<class T, class P, class ComparatorT, class ComparatorP>
Treap<T, P, ComparatorT, ComparatorP>::Treap() : root(nullptr) {}

/*  Вставка элемента с приоритетом
 *
 * @tparam T                тип хранимых данных
 * @tparam P                тип приоритетов
 * @tparam ComparatorT      функтор сравнения данных
 * @tparam ComparatorP      функтор сравнения приоритетов
 * @param new_element       новый элемент
 * @param new_priority      приоритет нового элемента
 */
template<class T, class P, class ComparatorT, class ComparatorP>
void Treap<T, P, ComparatorT, ComparatorP>::insert(const T &new_element, const P &new_priority) {
    //Обновляем корень
    if (!root || cmp_priority(root->priority, new_priority)) {
        auto left_right = split(root, new_element);
        root.reset(new Node {new_element, new_priority, left_right.first, left_right.second});
        return;
    }

    //Спуск, как в бинарном дереве
    auto current_node = root;
    while (true) {
        if (cmp_data(current_node->data, new_element)) { // current_node->data < new_element
            if (current_node->right) {
                // Проверка приоритетов, как в куче
                // current_node->right->priority < new_priority
                if (cmp_priority(current_node->right->priority, new_priority)) {
                    auto left_right = split(current_node->right, new_element);
                    current_node->right = std::shared_ptr<Node>(new Node{
                            new_element, new_priority, left_right.first, left_right.second});
                    return;
                } else {
                    current_node = current_node->right;
                }
            } else {
                current_node->right = std::shared_ptr<Node>(new Node{
                        new_element, new_priority, nullptr, nullptr});
                return;
            }
        } else {
            if (current_node->left) {
                // current_node->left->priority < new_priority
                if (cmp_priority(current_node->left->priority, new_priority)) {
                    auto left_right = split(current_node->left, new_element);
                    current_node->left = std::shared_ptr<Node>(new Node{
                            new_element, new_priority, left_right.first, left_right.second});
                    return;
                } else {
                    current_node = current_node->left;
                }
            } else {
                current_node->left = std::shared_ptr<Node>(new Node{
                        new_element, new_priority, nullptr, nullptr});
                return;
            }
        }
    }
}

/* Операция разрезания дерева по ключу, в результате получаем два дерева:
 * В левом все значения меньше ключа;
 * В право все значения больше либо равны ключу;
 *
 * @tparam T                тип хранимых данных
 * @tparam P                тип приоритетов
 * @tparam ComparatorT      функтор сравнения данных
 * @tparam ComparatorP      функтор сравнения приоритетов
 * @param t                 дерево, которое разрезаем
 * @param key               ключ, по которому разрезаем
 * @return                  пара: левое и правое дерево, описанные выше
 */
template<class T, class P, class ComparatorT, class ComparatorP>
std::pair<std::shared_ptr<typename Treap<T, P, ComparatorT, ComparatorP>::Node>,
        std::shared_ptr<typename Treap<T, P, ComparatorT, ComparatorP>::Node>>
Treap<T, P, ComparatorT, ComparatorP>::split(std::shared_ptr<Treap::Node> &t, const T &key) {
    if (!t) {
        return std::pair<std::shared_ptr<Treap::Node>, std::shared_ptr<Treap::Node>>({nullptr, nullptr});
    }

    if (cmp_data(t->data, key)) { //t->data < key
        auto right_nodes = split(t->right, key);
        t->right = right_nodes.first;

        return std::pair<std::shared_ptr<Treap::Node>, std::shared_ptr<Treap::Node>>({
            t,
            right_nodes.second,
        });
    } else {
        auto left_nodes = split(t->left, key);
        t->left = left_nodes.second;

        return std::pair<std::shared_ptr<Treap::Node>, std::shared_ptr<Treap::Node>>({
            left_nodes.first,
            t
        });
    }
}

/* Рекурсивный поиск глубины дерева
 *
 * @tparam T                тип хранимых данных
 * @tparam P                тип приоритетов
 * @tparam ComparatorT      функтор сравнения данных
 * @tparam ComparatorP      функтор сравнения приоритетов
 * @return                  глубина дерева
 */
template<class T, class P, class ComparatorT, class ComparatorP>
size_t Treap<T, P, ComparatorT, ComparatorP>::get_depth() const {
    return get_binary_tree_depth_rec(root);
}

/* Внутренняя рекурсия поиска глубины
 *
 * @tparam T                тип хранимых данных
 * @tparam P                тип приоритетов
 * @tparam ComparatorT      функтор сравнения данных
 * @tparam ComparatorP      функтор сравнения приоритетов
 * @param current_node      вершина, вершина от которой идём в глубину
 * @return                  глубина дерева
 */
template<class T, class P, class ComparatorT, class ComparatorP>
size_t Treap<T, P, ComparatorT, ComparatorP>::get_binary_tree_depth_rec(const std::shared_ptr<Treap::Node> &current_node) const {
    if (!current_node) {
        return 0;
    }
    return std::max(get_binary_tree_depth_rec(current_node->left), get_binary_tree_depth_rec(current_node->right)) + 1;
}