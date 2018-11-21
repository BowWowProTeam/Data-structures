#include <iostream>
#include <functional>
#include <vector>
#include <queue>
#include <stack>
#include <iterator>
#include <algorithm>
#include <memory>

/* Поиск ширины дерева
 *
 * @tparam E                тип хранимого node
 * @param root              корень дерева
 * @return                  максимальная дерева
 */
template<class E>
int get_max_width(E root) {
    if (!root) {
        return 0;
    }

    //Поиск ширины дерева поиском в ширину
    int max_width = 1;
    std::queue<E> queue;
    queue.push(root);

    while (!queue.empty()) {
        max_width = std::max(max_width, (int)queue.size());

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

    return max_width;
}