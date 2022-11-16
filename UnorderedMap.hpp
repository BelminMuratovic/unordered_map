#pragma once
#include <iostream>
#include <iterator>
#include <list>

template <typename Key, typename Value>
class UnorderedMap
{
private:
    size_t capacity_;
    size_t size_;
    std::list<std::pair<Key, Value>> *ptr;

public:
    UnorderedMap();
    UnorderedMap(const UnorderedMap &second);
    UnorderedMap(UnorderedMap &&second);
    UnorderedMap &operator=(const UnorderedMap &second);
    UnorderedMap &operator=(UnorderedMap &&second);
    ~UnorderedMap();
    class iterator;
    Value &operator[](const Key &key);
    bool erase(const Key &key);
    void emplace(const Key &key, const Value &value);
    iterator begin() const;
    iterator end() const;
    iterator find(const Key &key);

private:
    size_t hash(const Key &index);
    void iterate_forward(std::list<std::pair<Key, Value>> *&p, typename std::list<std::pair<Key, Value>>::iterator &i, int &c);
    void iterate_backward(std::list<std::pair<Key, Value>> *&p, typename std::list<std::pair<Key, Value>>::iterator &i, int &c);
};

template <typename Key, typename Value>
UnorderedMap<Key, Value>::UnorderedMap() : capacity_{1000}, size_{0}, ptr{new std::list<std::pair<Key, Value>>[capacity_]} {}

template <typename Key, typename Value>
UnorderedMap<Key, Value>::UnorderedMap(const UnorderedMap &second)
{
    capacity_ = second.capacity_;
    size_ = second.size_;
    ptr = new std::list<std::pair<Key, Value>>[capacity_];
    std::copy(second.ptr, second.ptr + capacity_, ptr);
}

template <typename Key, typename Value>
UnorderedMap<Key, Value>::UnorderedMap(UnorderedMap &&second)
{
    capacity_ = second.capacity_;
    size_ = second.size_;
    ptr = second.ptr;
    second.capacity_ = 0;
    second.size_ = 0;
    second.ptr = nullptr;
}

template <typename Key, typename Value>
UnorderedMap<Key, Value> &UnorderedMap<Key, Value>::operator=(const UnorderedMap &second)
{
    if (&second == this)
        return *this;
    delete[] ptr;
    capacity_ = second.capacity_;
    size_ = second.size_;
    std::copy(second.ptr, second.ptr + capacity_, ptr);
    return *this;
}

template <typename Key, typename Value>
UnorderedMap<Key, Value> &UnorderedMap<Key, Value>::operator=(UnorderedMap &&second)
{
    if (&second == this)
        return *this;
    delete[] ptr;
    capacity_ = second.capacity_;
    size_ = second.size_;
    ptr = second.ptr;
    second.capacity_ = 0;
    second.size_ = 0;
    second.ptr = nullptr;
    return *this;
}

template <typename Key, typename Value>
UnorderedMap<Key, Value>::~UnorderedMap()
{
    delete[] ptr;
}

template <typename Key, typename Value>
class UnorderedMap<Key, Value>::iterator : public std::iterator<std::bidirectional_iterator_tag, std::pair<Key, Value>>
{
private:
    std::list<std::pair<Key, Value>> *p = nullptr;
    typename std::list<std::pair<Key, Value>>::iterator it;
    int counter = 0;

public:
    iterator(std::list<std::pair<Key, Value>> *pp, typename std::list<std::pair<Key, Value>>::iterator i, int c) : p{pp}, it{i}, counter{c} {}
    std::pair<Key, Value> &operator*() { return *it; }
    std::pair<Key, Value> *operator->() { return &(*it); }
    iterator &operator++()
    {
        UnorderedMap{}.iterate_forward(p, it, counter);
        return *this;
    }
    iterator operator++(int)
    {
        iterator temp = *this;
        UnorderedMap{}.iterate_forward(p, it, counter);
        return temp;
    }
    iterator &operator--()
    {
        UnorderedMap{}.iterate_backward(p, it, counter);
        return *this;
    }
    iterator operator--(int)
    {
        iterator temp = *this;
        UnorderedMap{}.iterate_backward(p, it, counter);
        return temp;
    }
    bool operator==(const iterator &i) const { return it == i.it; }
    bool operator!=(const iterator &i) const { return it != i.it; }
};

template <typename Key, typename Value>
typename UnorderedMap<Key, Value>::iterator UnorderedMap<Key, Value>::begin() const
{
    size_t i = 0;
    while (i < (capacity_ - 1))                          // u metodu iterate_backward je uradjeno tako
    {                                                    // da ukoliko se na begin-u pozove operator--
        if ((ptr[i]).size() > 0)                         // da stanje ne bude nedefinisano, nego da
            return iterator(ptr + i, ptr[i].begin(), i); // ostane na begin-u
        ++i;
    }
    return end();
}

template <typename Key, typename Value>                                              // kao i u metodu begin, ukoliko
typename UnorderedMap<Key, Value>::iterator UnorderedMap<Key, Value>::end() const    // se na end-u pozove operator++,
{                                                                                    // ostaje na end-u (uradjeno u
    return iterator(ptr + capacity_ - 1, ptr[capacity_ - 1].begin(), capacity_ - 1); // metodu iterate_forward)
}

template <typename Key, typename Value>
typename UnorderedMap<Key, Value>::iterator UnorderedMap<Key, Value>::find(const Key &key)
{
    int hash_ = hash(key) % (capacity_ - 1);
    if (ptr[hash_].size() > 0)
    {
        for (auto i = ptr[hash_].begin(); i != ptr[hash_].end(); ++i)
        {
            if (i->first == key)
                return iterator(ptr + hash_, i, hash_);
        }
    }
    return end();
}

template <typename Key, typename Value>
void UnorderedMap<Key, Value>::iterate_forward(std::list<std::pair<Key, Value>> *&p, typename std::list<std::pair<Key, Value>>::iterator &i, int &c)
{
    if (c == (capacity_ - 1))
        return;
    ++i;
    if (i == p->end())
    {
        ++c;
        ++p;
        while (c < (capacity_ - 1))
        {
            if (p->size() > 0)
            {
                i = p->begin();
                return;
            }
            ++c;
            ++p;
        }
        i = p->begin();
    }
}

template <typename Key, typename Value>
void UnorderedMap<Key, Value>::iterate_backward(std::list<std::pair<Key, Value>> *&p, typename std::list<std::pair<Key, Value>>::iterator &i, int &c)
{
    if (i == p->begin())
    {
        int a = c - 1;
        auto p_ = p;
        --p_;
        while (a > -1)
        {
            if (p_->size() > 0)
            {
                i = --(p_->end());
                c = a;
                p = p_;
                break;
            }
            --a;
            --p_;
        }
    }
    else
        --i;
}

template <typename Key, typename Value>
Value &UnorderedMap<Key, Value>::operator[](const Key &key)
{
    iterator found = find(key);
    if (found != end())
        return found->second;
    else
    {
        emplace(key, Value{});
        auto found = find(key);
        return found->second;
    }
}

template <typename Key, typename Value>              // metodi erase i emplace su takodjer mogli biti
bool UnorderedMap<Key, Value>::erase(const Key &key) // uradjeni pomocu metoda find (da se provjeri da li
{                                                    // proslijedjeni kljuc vec postoji), medjutim to bi
    size_t hash_ = hash(key);                        // malo usporilo program za unos velike kolicine podataka
    hash_ = hash_ % (capacity_ - 1);
    if (ptr[hash_].size() > 0)
    {
        for (auto i = ptr[hash_].begin(); i != ptr[hash_].end(); ++i)
        {
            if (i->first == key)
            {
                ptr[hash_].erase(i);
                --size_;
                return true;
            }
        }
    }
    return false;
}

template <typename Key, typename Value>
void UnorderedMap<Key, Value>::emplace(const Key &key, const Value &value)
{
    size_t hash_ = hash(key);
    hash_ = hash_ % (capacity_ - 1);
    for (auto i = ptr[hash_].begin(); i != ptr[hash_].end(); ++i)
    {
        if (i->first == key)
            throw std::invalid_argument("This key already exist");
    }
    std::pair<Key, Value> pair = std::make_pair(key, value);
    ptr[hash_].push_back(pair);
    ++size_;
}

template <typename Key, typename Value>
size_t UnorderedMap<Key, Value>::hash(const Key &index)
{
    std::size_t hash_ = std::hash<Key>{}(index);
    return hash_;
}
