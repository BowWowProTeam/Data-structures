#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

/* HashTable
 * Хэш таблица с использованием открытой адресации
 * @tparam T        Тип данных в таблице
 * @tparam Hash     Хеш-функция(функтор)
 * @tparam Equal    Проверка равенства(функтор)
 */
template<class T, class Hash, class Equal = std::equal_to<T>>
class HashTable {
public:
    HashTable(const HashTable &hash_table) = delete;

    HashTable &operator=(const HashTable &hash_table) = delete;

    explicit HashTable(const T &deleted, const T &empty,
                       double max_load_factor, double eps = 1e-9,
                       size_t capacity = 8);

    bool insert(const T &new_element);

    bool erase(const T &key);

    bool has(const T &key) const;

private:
    typename std::vector<T>::iterator look_up(const T &key) const;

    void rehash(size_t cap);

    const T deleted;
    const T empty;

    Hash hash;
    Equal equal;

    std::shared_ptr<std::vector<T>> hash_container_ptr;

    double eps;
    double max_load_factor;

    size_t elements_count;
    size_t capacity;
};


/* Конструтор
 *
 * @tparam T                Тип данных в таблице
 * @tparam Hash             Хеш-функция(функтор)
 * @tparam Equal            Проверка равенства(функтор)
 * @param max_load_factor   Максимальный коэффициент нагрузки(при превышении происходит перехеширование)
 * @param eps               Точность проверки достижения max_load_factor
 * @param capacity          Начальный размер хеш-таблицы
 */

template<class T, class Hash, class Equal>
HashTable<T, Hash, Equal>::HashTable(const T &deleted, const T &empty,
                                     double max_load_factor, double eps, size_t capacity)
        : deleted(deleted), empty(empty),
          hash_container_ptr(new std::vector<T>(capacity, empty)),
          max_load_factor(max_load_factor), eps(eps), elements_count(0),
          capacity(capacity) {}

/* Вставка элемента в таблицу, работает за амортизированную константу
 * Дубликаты не допускаются
 *
 * @tparam T                Тип данных в таблице
 * @tparam Hash             Хеш-функция(функтор)
 * @tparam Equal            Проверка равенства(функтор)
 * @param new_element       Элемент, который вставляем
 * @return                  true - вставка прошла успешно;
 *                          false - дубликат;
 */
template<class T, class Hash, class Equal>
bool HashTable<T, Hash, Equal>::insert(const T &new_element) {
    if (fabs((double(elements_count) / capacity) - max_load_factor) < eps) {
        rehash(capacity * 2);
    }

    auto insert_it = look_up(new_element);
    if (*insert_it != deleted && *insert_it != empty) {
        return false;
    }

    *insert_it = new_element;
    elements_count++;

    return true;
}

/* Удаляет элемент из таблицы.
 *
 * @tparam T                Тип данных в таблице
 * @tparam Hash             Хеш-функция(функтор)
 * @tparam Equal            Проверка равенства(функтор)
 * @param key               Ключ, по которому удаляем
 * @return                  true - удаление прошло успешно;
 *                          false - элемент не найден в таблице
 */
template<class T, class Hash, class Equal>
bool HashTable<T, Hash, Equal>::erase(const T &key) {
    if (elements_count == 0) {
        return false;
    }

    auto it = look_up(key);
    if (*it == deleted || *it == empty) {
        return false;
    }

    elements_count--;
    *it = deleted;
    return true;
}

/* Проверка наличия элемента в хеш-таблице
 *
 * @tparam T                Тип данных в таблице
 * @tparam Hash             Хеш-функция(функтор)
 * @tparam Equal            Проверка равенства(функтор)
 * @param key               Ключ, который ищем
 * @return                  true - ключ найден;
 *                          false - ключ не найден;
 */
template<class T, class Hash, class Equal>
bool HashTable<T, Hash, Equal>::has(const T &key) const {
    auto it = look_up(key);
    return (*it != deleted && *it != empty);
}

/*  Выполняет увеличение размера таблицы и перехеширует элементы из старой таблицы;
 * Работает за один вызов конструктора вектора и n - вставок ключей;
 *
 * @tparam T                Тип данных в таблице
 * @tparam Hash             Хеш-функция(функтор)
 * @tparam Equal            Проверка равенства(функтор)
 * @param key               Ключ, который ищем
 * @param cap               Новый размер таблицы
 */
template<class T, class Hash, class Equal>
void HashTable<T, Hash, Equal>::rehash(size_t cap) {
    auto old_hash_container_ptr = hash_container_ptr;

    capacity = cap;
    elements_count = 0;
    hash_container_ptr.reset(new std::vector<T>(capacity, empty));

    for (const auto &it : *old_hash_container_ptr) {
        if (it != deleted && it != empty) {
            insert(it);
        }
    }
}

/*  Выполняет поиск ключа в хеш-таблице;
 *
 * @tparam T                Тип данных в таблице
 * @tparam Hash             Хеш-функция(функтор)
 * @tparam Equal            Проверка равенства(функтор)
 * @param key               Ключ, который ищем
 * @return                  Итератор с найденным элементом(second = true)
 *                          Итератор на пустое место за группой занятых хешей,
 *                          т.е. место для вставки нового элемента(second = false)
 */
template<class T, class Hash, class Equal>
typename std::vector<T>::iterator
HashTable<T, Hash, Equal>::look_up(const T &key) const {
    size_t i = 1;
    size_t hash_index = hash(key, capacity);
    size_t original_hash_index = hash_index;

    bool was_empty = false;
    size_t first_empty_index = 0;
    while ((*hash_container_ptr)[hash_index] != empty) {
        //запоминаем первую пустую ячейку для вставки
        if ((*hash_container_ptr)[hash_index] == deleted && !was_empty) {
            was_empty = true;
            first_empty_index = hash_index;
        }

        if (equal((*hash_container_ptr)[hash_index], key)) {
            return hash_container_ptr->begin() + hash_index;
        }

        hash_index = (hash_index + i++) % capacity;
        if (original_hash_index == hash_index) {
            break;
        }
    }

    return hash_container_ptr->begin() + (was_empty ? first_empty_index : hash_index);
}

/* Функтор, представляющий хеш-функцию строки реализованую
 * с помощью вычисления значения многочлена методом Горнера.
 *
 * @tparam A    точка, в которой считаем многочлен.
 *              Для корректной работы таблицы необходимо GCD(A, M) = 1,
 *              где M - размер хеш-таблицы
 */
template<int A>
struct hash {
    size_t operator()(const std::string &key, size_t m) const {
        size_t hash = 0;
        for (char symbol : key) {
            hash = (hash * A + symbol) % m;
        }

        return hash;
    }
};

//Example
//HashTable<std::string, hash<127>> hash_table("DELETED", "EMPTY", 0.75);