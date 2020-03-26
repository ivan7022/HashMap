#pragma once

#include <forward_list>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <list>
#include <utility>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
 public:
    using KeyValuePair = std::pair<const KeyType, ValueType>;
    using const_iterator = typename std::list<KeyValuePair>::const_iterator;
    using iterator = typename std::list<KeyValuePair>::iterator;

    HashMap(Hash hash_obj = Hash());

    void insert(KeyValuePair element);

    template<typename ForwardIterator>
    HashMap(ForwardIterator start, ForwardIterator end, Hash hash_obj = Hash());

    HashMap(std::initializer_list<KeyValuePair> init_list,
            Hash hash_obj = Hash());

    void erase(const KeyType& key);

    const_iterator find(const KeyType& key) const;

    iterator find(const KeyType& key);

    size_t size() const;

    bool empty() const;

    Hash hash_function() const;

    ValueType &operator[] (const KeyType& key);

    const ValueType& at(const KeyType& key) const;

    HashMap &operator= (const HashMap &other);

    const_iterator begin() const;

    const_iterator end() const;

    iterator begin();

    iterator end();

    void clear();

    ~HashMap();

 private:
    std::vector<std::list<iterator>> table_;
    std::list<KeyValuePair> data_;
    size_t current_size_;
    size_t current_capacity_;
    Hash hash_maker_;

    void InsertInternal(KeyValuePair& element, size_t hashed_key);

    void Initialization(size_t capacity_ = 32);

    size_t MakeHash(const KeyType& object) const;

    void RehashIfNeeded();
};

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(Hash hash_obj)
        : hash_maker_(hash_obj) {
    Initialization();
}

template<class KeyType, class ValueType, class Hash>
template<typename ForwardIterator>
HashMap<KeyType, ValueType, Hash>::HashMap(
        ForwardIterator start,
        ForwardIterator end,
        Hash hash_obj) : hash_maker_(hash_obj) {
    Initialization();
    for (; start != end; ++start) {
        insert(*start);
    }
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap
(std::initializer_list<KeyValuePair> init_list, Hash hash_obj)
: hash_maker_(hash_obj) {
    Initialization();
    for (auto&& element : init_list) {
        insert(element);
    }
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
auto HashMap<KeyType, ValueType, Hash>::begin() const -> const_iterator {
    return const_iterator(data_.begin());
}

template<class KeyType, class ValueType, class Hash>
auto HashMap<KeyType, ValueType, Hash>::end() const -> const_iterator {
    return const_iterator(data_.end());
}

template<class KeyType, class ValueType, class Hash>
auto HashMap<KeyType, ValueType, Hash>::begin() -> iterator {
    return iterator(data_.begin());
}

template<class KeyType, class ValueType, class Hash>
auto HashMap<KeyType, ValueType, Hash>::end() -> iterator {
    return iterator(data_.end());
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::insert(KeyValuePair element) {
    size_t hash_table_cell_id = MakeHash(element.first);
    if (!table_[hash_table_cell_id].empty()) {
        auto it = table_[hash_table_cell_id].begin();
        while (it != table_[hash_table_cell_id].end()) {
            if ((*it)->first == element.first) {
                return;
            }
            ++it;
        }
    }
    InsertInternal(element, hash_table_cell_id);
    RehashIfNeeded();
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::erase(const KeyType& key) {
    size_t hash_table_cell_id = MakeHash(key);
    auto it = table_[hash_table_cell_id].begin();
    for (; it != table_[hash_table_cell_id].end(); ++it) {
        if ((*it)->first == key) {
            data_.erase(*it);
            table_[hash_table_cell_id].erase(it);
            --current_size_;
            return;
        }
    }
}

template<class KeyType, class ValueType, class Hash>
auto HashMap<KeyType, ValueType, Hash>::find(
        const KeyType& key) const -> const_iterator {
    size_t hash_table_cell_id = MakeHash(key);
    for (auto&& element : table_[hash_table_cell_id]) {
        if (element->first == key) {
            return const_iterator(element);
        }
    }
    return end();
}


template<class KeyType, class ValueType, class Hash>
auto HashMap<KeyType, ValueType, Hash>::find(const KeyType& key) -> iterator {
    size_t hash_table_cell_id = MakeHash(key);
    for (auto&& element : table_[hash_table_cell_id]) {
        if (element->first == key) {
            return iterator(element);
        }
    }
    return end();
}

template<class KeyType, class ValueType, class Hash>
ValueType& HashMap<KeyType, ValueType, Hash>::operator[] (const KeyType& key) {
    if (find(key) == end()) {
        insert({key, ValueType()});
    }
    return find(key)->second;
}

template<class KeyType, class ValueType, class Hash>
const ValueType& HashMap<KeyType, ValueType, Hash>::at(
        const KeyType& key) const {
    auto it = find(key);
    if (it != end()) {
        return it->second;
    }
    throw std::out_of_range("OutOfRangeError");
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>&HashMap<KeyType, ValueType, Hash>::operator= (
        const HashMap &other) {
    if (this == &other) {
        return *this;
    }
    clear();
    hash_maker_ = other.hash_function();
    Initialization(other.current_capacity_);
    for (auto&& element : other) {
        insert(element);
    }
    return *this;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::clear() {
    for (auto&& element : data_) {
        size_t hash_table_cell_id = MakeHash(element.first);
        table_[hash_table_cell_id].clear();
    }
    data_.clear();
    current_size_ = 0;
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::~HashMap() {
    clear();
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::InsertInternal(KeyValuePair& element,
        size_t hashed_key) {
    data_.emplace_back(element);
    auto current_iterator = data_.end();
    table_[hashed_key].emplace_back(--current_iterator);
    ++current_size_;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::Initialization(size_t capacity_) {
    table_ = std::vector<std::list<iterator>> (capacity_);
    current_capacity_ = capacity_;
    current_size_ = 0;
}

template<class KeyType, class ValueType, class Hash>
size_t HashMap<KeyType, ValueType, Hash>::MakeHash(
        const KeyType& object) const {
    return hash_maker_(object) % current_capacity_;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::RehashIfNeeded() {
    if (current_capacity_ > 2 * current_size_) {
        return;
    }
    std::forward_list<KeyValuePair> current_data_;
    for (auto it = data_.begin(); it != data_.end(); ++it, data_.pop_front()) {
        KeyValuePair current_element =
                std::make_pair(it->first, it->second);
        current_data_.push_front(current_element);
    }
    table_.clear();
    Initialization(2 * current_capacity_);
    for (auto&& element : current_data_) {
        insert(element);
    }
    current_data_.clear();
}
