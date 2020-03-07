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

    void erase(KeyType key);

    const_iterator find(const KeyType key) const;

    iterator find(const KeyType key);

    size_t size() const;

    bool empty() const;

    Hash hash_function() const;

    ValueType &operator[] (const KeyType key);

    const ValueType& at(const KeyType key) const;

    HashMap &operator= (HashMap const &other);

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

    void InsertInternal(KeyValuePair element, int hashed_key);

    void Initialization(int capacity_ = 32);

    int MakeHash(const KeyType object) const;

    void RehashIfNeeded();
};

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::HashMap(Hash hash_obj)
: hash_maker_(hash_obj) {
    Initialization();
}

template<class KeyType, class ValueType, class Hash>
template<typename ForwardIterator>
HashMap<KeyType, ValueType, Hash>::HashMap(ForwardIterator start,
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
void HashMap<KeyType, ValueType, Hash>::insert(KeyValuePair element) {
    size_t hashTableCellId = MakeHash(element.first);
    if (!table_[hashTableCellId].empty()) {
        auto it = table_[hashTableCellId].begin();
        while (it != table_[hashTableCellId].end()) {
            if ((*it)->first == element.first) {
                return;
            }
            ++it;
        }
    }
    InsertInternal(element, hashTableCellId);
    RehashIfNeeded();
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::erase(KeyType key) {
    size_t hashTableCellId = MakeHash(key);
    auto it = table_[hashTableCellId].begin();
    for (; it != table_[hashTableCellId].end(); ++it) {
        if ((*it)->first == key) {
            data_.erase(*it);
            table_[hashTableCellId].erase(it);
            --current_size_;
            return;
        }
    }
}

template<class KeyType, class ValueType, class Hash>
typename std::list<std::pair<const KeyType, ValueType>>::const_iterator
        HashMap<KeyType, ValueType, Hash>::find(const KeyType key) const {
    size_t hashTableCellId = MakeHash(key);
    for (auto&& element : table_[hashTableCellId]) {
        if (element->first == key) {
            return const_iterator(element);
        }
    }
    return end();
}


template<class KeyType, class ValueType, class Hash>
typename std::list<std::pair<const KeyType, ValueType>>::iterator
HashMap<KeyType, ValueType, Hash>::find(const KeyType key) {
    size_t hashTableCellId = MakeHash(key);
    for (auto&& element : table_[hashTableCellId]) {
        if (element->first == key) {
            return iterator(element);
        }
    }
    return end();
}

template<class KeyType, class ValueType, class Hash>
ValueType& HashMap<KeyType, ValueType, Hash>::operator[] (const KeyType key) {
    if (find(key) == end()) {
        insert({key, ValueType()});
    }
    return find(key)->second;
}

template<class KeyType, class ValueType, class Hash>
const ValueType& HashMap<KeyType, ValueType,
Hash>::at(const KeyType key) const {
    auto it = find(key);
    if (it != end()) {
        return it->second;
    }
    throw std::out_of_range("OutOfRangeError");
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>&
        HashMap<KeyType, ValueType, Hash>::operator= (HashMap const &other) {
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
        size_t hashTableCellId = MakeHash(element.first);
        table_[hashTableCellId].clear();
    }
    data_.clear();
    current_size_ = 0;
}

template<class KeyType, class ValueType, class Hash>
HashMap<KeyType, ValueType, Hash>::~HashMap() {
    clear();
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::InsertInternal(std::pair<const KeyType,
        ValueType> element, int hashed_key) {
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
int HashMap<KeyType, ValueType, Hash>::MakeHash(const KeyType object) const {
    return hash_maker_(object) % current_capacity_;
}

template<class KeyType, class ValueType, class Hash>
void HashMap<KeyType, ValueType, Hash>::RehashIfNeeded() {
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
