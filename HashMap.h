#pragma once

#include <iostream>
#include <vector>
#include <forward_list>
#include <algorithm>
#include <stdexcept>
#include <list>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
    using const_iterator = typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;
    using iterator = typename std::list<std::pair<const KeyType, ValueType>>::iterator;

    HashMap(Hash hash_obj = Hash()) : hash_maker(hash_obj) {
        Initialization();
    }

    void insert(std::pair<KeyType, ValueType> element);

    template<typename ForwardIterator>
    HashMap(ForwardIterator start, ForwardIterator end,
            Hash hash_obj = Hash()) : hash_maker(hash_obj) {
        Initialization();
        for (; start != end; ++start) {
            insert(*start);
        }
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> init_list,
            Hash hash_obj = Hash()) : hash_maker(hash_obj){
        Initialization();
        for (auto&& element : init_list)
            insert(element);
    }

    void erase(KeyType key);

    const_iterator find(KeyType key) const;

    iterator find(KeyType key);

    size_t size() const {
        return current_size;
    }

    bool empty() const {
        return current_size == 0;
    }

    Hash hash_function() const {
        return hash_maker;
    }

    ValueType &operator[] (KeyType key) {
        if (find(key) == end()) {
            insert({key, ValueType()});
        }
        return find(key)->second;
    }

    const ValueType &at(KeyType key) const {
        auto it = find(key);
        if (it != end()) {
            return it->second;
        }
        throw std::out_of_range("OutOfRangeError");
    }

    HashMap &operator= (HashMap const &object) {
        if (this == &object) {
            return *this;
        }
        clear();
        hash_maker = object.hash_function();
        Initialization(object.current_capacity);
        for (auto&& element :object) {
            insert(element);
        }
        return *this;
    }

    const_iterator begin() const {
        return const_iterator(data.begin());
    }

    const_iterator end() const {
        return const_iterator(data.end());
    }

    iterator begin() {
        return iterator(data.begin());
    }

    iterator end() {
        return iterator(data.end());
    }

    void clear() {
        for (auto&& element : data) {
            int id = MakeHash(element.first);
            table[id].clear();
        }
        data.clear();
        current_size = 0;
    }


    ~HashMap() {
        clear();
    }

private:
    std::vector<std::list<iterator>> table;
    std::list<std::pair<const KeyType, ValueType>> data;
    size_t current_size;
    size_t current_capacity;
    Hash hash_maker;

    void Push(std::pair<KeyType, ValueType> element, int hashed_key) {
        data.emplace_back(element);
        auto current_iterator = data.end();
        table[hashed_key].emplace_back(--current_iterator);
        ++current_size;
    }

    void Initialization(int capacity = 32) {
        table = std::vector<std::list<iterator>> (capacity);
        current_capacity = capacity;
        current_size = 0;
    }

    int MakeHash(KeyType object) const {
        return hash_maker(object) % current_capacity;
    }

    void Rehash() {
        if (current_capacity > 2 * current_size) {
            return;
        }
        std::forward_list<std::pair<KeyType, ValueType>> current_data;
        for (auto&& element : data) {
            std::pair<KeyType, ValueType> current_element =
                    std::make_pair(element.first, element.second);
            current_data.push_front(current_element);
        }
        data.clear();
        table.clear();
        Initialization(2 * current_capacity);
        for (auto&& element : current_data) {
            insert(element);
        }
        current_data.clear();
    }
};

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::insert(std::pair<KeyType, ValueType> element) {
    int id = MakeHash(element.first);
    if (!table[id].empty()) {
        auto it = table[id].begin();
        while (it != table[id].end()) {
            if ((*it)->first == element.first) {
                return;
            }
            ++it;
        }
    }
    Push(element, id);
    Rehash();
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::erase(KeyType key) {
    int id = MakeHash(key);
    auto it = table[id].begin();
    for (; it != table[id].end(); ++it) {
        if ((*it)->first == key) {
            data.erase(*it);
            table[id].erase(it);
            --current_size;
            return;
        }
    }
}

template<class KeyType, class ValueType, class Hash>
typename std::list<std::pair<const KeyType, ValueType>>::const_iterator
        HashMap<KeyType, ValueType, Hash>::find(KeyType key) const {
    int id = MakeHash(key);
    for (auto&& element : table[id]) {
        if (element->first == key) {
            return const_iterator(element);
        }
    }
    return end();
}


template<class KeyType, class ValueType, class Hash>
typename std::list<std::pair<const KeyType, ValueType>>::iterator
HashMap<KeyType, ValueType, Hash>::find(KeyType key) {
    int id = MakeHash(key);
    for (auto&& element : table[id]) {
        if (element->first == key) {
            return iterator(element);
        }
    }
    return end();
}