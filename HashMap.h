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

    HashMap(Hash hash_obj = Hash());

    void insert(std::pair<KeyType, ValueType> element);

    template<typename ForwardIterator>
    HashMap(ForwardIterator start, ForwardIterator end, Hash hash_obj = Hash());

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> init_list, Hash hash_obj = Hash());

    void erase(KeyType key);

    const_iterator find(KeyType key) const;

    iterator find(KeyType key);

    size_t size() const;

    bool empty() const;


    Hash hash_function() const;

    ValueType &operator[] (KeyType key);

    const ValueType& at(KeyType key) const;

    HashMap &operator= (HashMap const &object);

    const_iterator begin() const;

    const_iterator end() const;

    iterator begin();

    iterator end();

    void clear();

    ~HashMap();

private:
    std::vector<std::list<iterator>> table_;
    std::list<std::pair<const KeyType, ValueType>> data_;
    size_t current_size_;
    size_t current_capacity_;
    Hash hash_maker_;

    void Push(std::pair<KeyType, ValueType> element, int hashed_key);

    void Initialization(int capacity_ = 32);

    int MakeHash(KeyType object) const;

    void Rehash();
};

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(Hash hash_obj) : hash_maker_(hash_obj) {
    Initialization();
}

template<class KeyType, class ValueType, class Hash>
template<typename ForwardIterator>
HashMap<KeyType, ValueType, Hash>::HashMap(ForwardIterator start, ForwardIterator end,
        Hash hash_obj) : hash_maker_(hash_obj) {
    Initialization();
    for (; start != end; ++start) {
        insert(*start);
    }
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(std::initializer_list<std::pair<KeyType,
        ValueType>> init_list, Hash hash_obj) : hash_maker_(hash_obj){
    Initialization();
    for (auto&& element : init_list)
        insert(element);
}

template<class KeyType, class ValueType, class Hash>
size_t HashMap<KeyType, ValueType, Hash>::size() const {
    return current_size_;
}

template<class KeyType, class ValueType, class Hash>
bool HashMap<KeyType, ValueType, Hash>::empty() const {
    return current_size_ == 0;
}

template<class KeyType, class ValueType, class Hash>
Hash HashMap<KeyType, ValueType, Hash>::hash_function() const {
    return hash_maker_;
}

template<class KeyType, class ValueType, class Hash>
typename std::list<std::pair<const KeyType, ValueType>>::const_iterator
HashMap<KeyType, ValueType, Hash>::begin() const {
    return const_iterator(data_.begin());
}

template<class KeyType, class ValueType, class Hash>
typename std::list<std::pair<const KeyType, ValueType>>::const_iterator
HashMap<KeyType, ValueType, Hash>::end() const {
    return const_iterator(data_.end());
}

template<class KeyType, class ValueType, class Hash>
typename std::list<std::pair<const KeyType, ValueType>>::iterator
HashMap<KeyType, ValueType, Hash>::begin() {
    return iterator(data_.begin());
}

template<class KeyType, class ValueType, class Hash>
typename std::list<std::pair<const KeyType, ValueType>>::iterator
HashMap<KeyType, ValueType, Hash>::end() {
    return iterator(data_.end());
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::insert(std::pair<KeyType, ValueType> element) {
    int id = MakeHash(element.first);
    if (!table_[id].empty()) {
        auto it = table_[id].begin();
        while (it != table_[id].end()) {
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
    auto it = table_[id].begin();
    for (; it != table_[id].end(); ++it) {
        if ((*it)->first == key) {
            data_.erase(*it);
            table_[id].erase(it);
            --current_size_;
            return;
        }
    }
}

template<class KeyType, class ValueType, class Hash>
typename std::list<std::pair<const KeyType, ValueType>>::const_iterator
        HashMap<KeyType, ValueType, Hash>::find(KeyType key) const {
    int id = MakeHash(key);
    for (auto&& element : table_[id]) {
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
    for (auto&& element : table_[id]) {
        if (element->first == key) {
            return iterator(element);
        }
    }
    return end();
}

template<class KeyType, class ValueType, class Hash>
ValueType& HashMap<KeyType, ValueType, Hash>::operator[] (KeyType key) {
    if (find(key) == end()) {
        insert({key, ValueType()});
    }
    return find(key)->second;
}

template<class KeyType, class ValueType, class Hash>
const ValueType& HashMap<KeyType, ValueType, Hash>::at(KeyType key) const {
    auto it = find(key);
    if (it != end()) {
        return it->second;
    }
    throw std::out_of_range("OutOfRangeError");
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>&
        HashMap<KeyType, ValueType, Hash>::operator= (HashMap const &object) {
    if (this == &object) {
        return *this;
    }
    clear();
    hash_maker_ = object.hash_function();
    Initialization(object.current_capacity_);
    for (auto&& element :object) {
        insert(element);
    }
    return *this;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::clear() {
    for (auto&& element : data_) {
        int id = MakeHash(element.first);
        table_[id].clear();
    }
    data_.clear();
    current_size_ = 0;
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::~HashMap() {
    clear();
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::Push(std::pair<KeyType, ValueType> element, int hashed_key) {
    data_.emplace_back(element);
    auto current_iterator = data_.end();
    table_[hashed_key].emplace_back(--current_iterator);
    ++current_size_;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::Initialization(int capacity_) {
    table_ = std::vector<std::list<iterator>> (capacity_);
    current_capacity_ = capacity_;
    current_size_ = 0;
}

template<class KeyType, class ValueType, class Hash>
int HashMap<KeyType, ValueType, Hash>::MakeHash(KeyType object) const {
    return hash_maker_(object) % current_capacity_;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::Rehash() {
    if (current_capacity_ > 2 * current_size_) {
        return;
    }
    std::forward_list<std::pair<KeyType, ValueType>> current_data_;
    for (auto&& element : data_) {
        std::pair<KeyType, ValueType> current_element =
                std::make_pair(element.first, element.second);
        current_data_.push_front(current_element);
    }
    data_.clear();
    table_.clear();
    Initialization(2 * current_capacity_);
    for (auto&& element : current_data_) {
        insert(element);
    }
    current_data_.clear();
}
