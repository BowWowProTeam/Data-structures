#include <iostream>
#include <memory>
#include <assert.h>

/* AVL-дерево с методами вставки, удаления
 * и поиска по индексу в отсортированной последовательности
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 */
template<class T, class Comparator>
class AVLTree {
public:
    AVLTree(const AVLTree &) = delete;
    AVLTree &operator=(const AVLTree &) = delete;

    explicit AVLTree();
    size_t insert(const T &new_element);
    T remove(const T &key);
    T get_Kth(size_t pos);

    ~AVLTree();
private:
    //Узел AVL-дерева
    struct Node {
        T data;                     //хранимые данные
        size_t elements_subtree;    //количество элементов в поддереве,
        //с корнем в этой вершине(сама вершина не учитывается)
        unsigned char depth;        //глубина поддерева, с корнем в этомй вершине
        //(вершина учитывается)

        Node *left;                 //указатель на левое поддерево
        Node *right;                //Указатель на правое поддерево

        int balance() const {
            return (right ? right->depth : 0) - (left ? left->depth : 0);
        }

        void update_depth() {
            unsigned char depth_left = (left ? left->depth : char(0));
            unsigned char depth_right = (right ? right->depth : char(0));

            depth = (depth_left > depth_right ? depth_left : depth_right) + char(1);
        }

        size_t in_left_subtree() {
            return (left ? left->elements_subtree + 1 : 0);
        }

        size_t in_right_subtree() {
            return  (right ? right->elements_subtree + 1 : 0);
        }

        void update_elements_subtree() {
            elements_subtree = in_left_subtree() + in_right_subtree();
        };
    };


    Node *insert_rec(Node*, const T &, size_t *);
    Node *remove_rec(Node *, const T &, T*, bool &);
    void get_Kth_rec(Node*, size_t, size_t, T*);

    Node *balance_node(Node*);
    Node *rotate_right(Node*);
    Node *rotate_left(Node*);

    Node *remove_min(Node *, T *);
    Node *remove_max(Node *, T *);
    void remove_subtree(Node *);

    Comparator cmp;
    Node *root;
    size_t element_count;
};

/* Конструктор
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 */
template<class T, class Comparator>
AVLTree<T, Comparator>::AVLTree() : root(nullptr), element_count(0) {}

/* Операция вставки в AVL-дерево
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 * @param new_element       элемент, который вставляем
 * @return                  место элемента в отсортированной последовательности
 *                          вставленных элементов
 */
template<class T, class Comparator>
size_t AVLTree<T, Comparator>::insert(const T &new_element) {
    size_t pos = 0;
    element_count++;
    root = insert_rec(root, new_element, &pos);
    return pos;
}

/* Удаление элемента по ключу
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 * @param key               ключ
 * @return                  копия удаленного элемента
 */
template<class T, class Comparator>
T AVLTree<T, Comparator>::remove(const T &key) {
    bool was_deleted = false;
    T deleted;
    root = remove_rec(root, key, &deleted, was_deleted);
    if (was_deleted) {
        element_count--;
    }

    return deleted;
}

/* Получение K-ой порядковой статистики(элемента на K-ой позиции в отсортированном массиве элементов)
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 * @param pos               позиция элемента
 * @return                  копия K-ой порядковой статистики
 */
template<class T, class Comparator>
T AVLTree<T, Comparator>::get_Kth(size_t pos) {
    assert(pos < element_count);

    T value;
    get_Kth_rec(root, pos, 0, &value);
    return value;
}

/* Правый поворот поддерева с корнем в node (aka. правый малый поворот)
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 * @param node              вершина, которую поворачиваем
 * @return                  указатель на новый корень поддерева
 */
template<class T, class Comparator>
typename AVLTree<T, Comparator>::Node *AVLTree<T, Comparator>::rotate_right(Node* node) {
    assert(node && node->left);

    Node *left = node->left;
    node->left = left->right;
    left->right = node;

    node->update_depth();
    left->update_depth();

    //Обновляем количество элементов в поддеревьях,
    //порядок важен!
    node->update_elements_subtree();
    left->update_elements_subtree();

    return left;
}

/* Левый поворот поддерева с корнем в node (aka. левый малый поворот)
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 * @param node              вершина, которую поворачиваем
 * @return                  указатель на новый корень поддерева
 */
template<class T, class Comparator>
typename AVLTree<T, Comparator>::Node *
AVLTree<T, Comparator>::rotate_left(Node* node) {
    assert(node && node->right);

    Node *right = node->right;
    node->right = right->left;
    right->left = node;
    node->update_depth();
    right->update_depth();

    //Обновляем количество элементов в поддеревьях,
    //порядок важен!
    node->update_elements_subtree();
    right->update_elements_subtree();

    return right;
}

/* Балансировка поддерева с корнем в node
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 * @param node              корень поддерева, которое балансируем
 * @return                  корень сбалансированного поддерева
 */
template<class T, class Comparator>
typename AVLTree<T, Comparator>::Node *
AVLTree<T, Comparator>::balance_node(Node* node) {
    assert(node);

    node->update_depth(); //сначала нужно обновить глубину

    //aka. большой левый поворот
    if (node->balance() == 2) {
        if (node->right->balance() < 0) {
            node->right = rotate_right(node->right);
        }

        return rotate_left(node);
    }

    //aka. большой правый поворот
    if (node->balance() == -2) {
        if (node->left->balance() > 0) {
            node->left = rotate_left(node->left);
        }

        return rotate_right(node);
    }

    //могли поменятся левое и правое поддеревья без нарушения баланса => нужно тоже обновлять
    node->update_elements_subtree();
    return node;
}

/* Служебная функция рекурсивной вставки
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 * @param node              текущий узел
 * @param new_element       новые данные
 * @param pos_ptr           указатель, который хранит позицию, на которую встанет элемент
 * @return                  указатель на корень поддерева, в котором находится новый элемент
 */
template<class T, class Comparator>
typename AVLTree<T, Comparator>::Node *
AVLTree<T, Comparator>::insert_rec(Node *node, const T &new_element, size_t *pos_ptr) {
    if (!node) {
        return new Node {new_element, 0, 1, nullptr, nullptr};
    }

    if (cmp(new_element, node->data)) { // new_element < node->data
        node->left = insert_rec(node->left, new_element, pos_ptr);
    } else {
        *pos_ptr += node->in_left_subtree() + 1;    //очевидно, что позиция будет больше, чем позиция текущего на 1,
        //которая задается количеством элементов в его левом поддереве

        node->right = insert_rec(node->right, new_element, pos_ptr);
    }

    node->elements_subtree++;
    return balance_node(node);
}

/* Служебная рекурсивная функция поиска K-ой порядковой статистики
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 * @param node              текущий узел
 * @param find_pos          статистика, которую ищем
 * @param pos               текущая позиция
 * @param value             указатель на место, куда копировать удаляемое значение
 */
template<class T, class Comparator>
void AVLTree<T, Comparator>::get_Kth_rec(AVLTree<T, Comparator>::Node *node, size_t find_pos, size_t pos, T* value) {
    if (!node) {
        return;
    }

    if (pos + node->in_left_subtree() == find_pos) {
        *value = node->data;
    } else {
        if (pos + node->in_left_subtree() < find_pos) {
            //Также как и при вставке, очевидно, что элемент уже дальше, чем индекс текущего
            get_Kth_rec(node->right, find_pos, pos + node->in_left_subtree() + 1, value);
        } else {
            get_Kth_rec(node->left, find_pos, pos, value);
        }
    }
}

/* Служебная рекурсивная функция удаления по ключу
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 * @param node              текущий узел
 * @param key               ключ, по которому удаляем
 * @param value             указатель на место, куда копировать удаляемое значение
 * @param was_deleted       было ли что-либо удалено(чтобы обновить счетчики количества эл. в узлах)
 * @return                  новый корень
 */
template<class T, class Comparator>
typename AVLTree<T, Comparator>::Node *AVLTree<T, Comparator>::remove_rec(AVLTree::Node *node, const T &key, T *value, bool &was_deleted) {
    if (!node) {
        return nullptr;
    }

    if (cmp(node->data, key)) {
        node->right = remove_rec(node->right, key, value, was_deleted);
        if (was_deleted) node->elements_subtree--;
    } else if (cmp(key, node->data)) {
        node->left = remove_rec(node->left, key, value, was_deleted);
        if (was_deleted) node->elements_subtree--;
    } else {
        *value = node->data;

        unsigned char right_depth = (node->right ? node->right->depth : char(0));
        unsigned char left_depth = (node->left ? node->left->depth : char(0));

        if (!right_depth && !left_depth) {
            delete node;    //лист можно просто удалить
            return nullptr;
        }

        //брать ближайший из более глубокого
        if (right_depth >= left_depth) {
            node->right = remove_min(node->right, &node->data);
        } else {
            node->left = remove_max(node->left, &node->data);
        }
    }

    return balance_node(node);
}

/* Служебная функция удаления минимума из поддерева
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 * @param node              текущий узел
 * @return                  указатель на корень поддерева, в котором удалили минимум
 */
template<class T, class Comparator>
typename AVLTree<T, Comparator>::Node *
AVLTree<T, Comparator>::remove_min(AVLTree::Node *node, T *save_data) {
    assert(node);

    if (!node->left) {
        *save_data = node->data;
        Node *right = node->right;
        delete node;

        return right;
    }

    node->elements_subtree--;
    node->left = remove_min(node->left, save_data);
    return balance_node(node);
}

/* Служебная функция удаления максимума из поддерева
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 * @param node              текущий узел
 * @return                  указатель на корень поддерева, в котором удалили минимум
 */
template<class T, class Comparator>
typename AVLTree<T, Comparator>::Node *
AVLTree<T, Comparator>::remove_max(AVLTree::Node *node, T *save_data) {
    assert(node);

    if (!node->right) {
        *save_data = node->data;
        Node *left = node->left;
        delete node;

        return left;
    }

    node->elements_subtree--;
    node->right = remove_max(node->right, save_data);
    return balance_node(node);
}

/* Деструктор
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 */
template<class T, class Comparator>
AVLTree<T, Comparator>::~AVLTree() {
    remove_subtree(root);
}

/* Служебная функция удаления всех узлов обходом в глубину
 *
 * @tparam T                тип хранимых данных
 * @tparam Comparator       функтор сравнения данных
 * @param node              текущий узел
 */
template<class T, class Comparator>
void AVLTree<T, Comparator>::remove_subtree(AVLTree::Node *node) {
    if (!node) {
        return;
    }

    remove_subtree(node->left);
    remove_subtree(node->right);
    delete node;
}