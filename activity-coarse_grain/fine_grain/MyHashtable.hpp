#ifndef _MY_HASHTABLE_H_
#define _MY_HASHTABLE_H_

#include <cstdint>
#include <iostream>
#include <functional>
#include <mutex>
#include <shared_mutex>
using namespace std;


template <typename K, typename V>
class Node
{
public:
    Node(K key_, V value_) : next(nullptr), key(key_), value(value_)
    {
    }
    ~Node()
    {
        next = nullptr;
    }

    const K &getKey() const { return key; }
    void setValue(V value_) { value = value_; }
    const V &getValue() const { return value; }

    Node *next;

private:
    K key;
    V value;
};

class mutex_class
{
public:
    mutable shared_timed_mutex mu;
};

//collision handle using LL
template <typename K, typename V>
class HashLL
{
public:
    HashLL() : head(nullptr)
    {
    }

    V find(const K &key, shared_timed_mutex &mutex_) const
    {
        shared_lock<shared_timed_mutex> lock(mutex_);
        Node<K, V> *node = head;

        while (node != nullptr)
        {
            if (node->getKey() == key)
            {
                return node->getValue();
            }
            node = node->next;
        }
        return 0;
    }

    void update(const K &key, shared_timed_mutex &mutex_)
    {
        unique_lock<shared_timed_mutex> lock(mutex_);
        Node<K, V> *prev = nullptr;
        Node<K, V> *node = head;

        while (node != nullptr && node->getKey() != key)
        {
            prev = node;
            node = node->next;
        }

        if (nullptr == node)
        {
            if (nullptr == head)
            {
                head = new Node<K, V>(key, 1);
            }
            else
            {
                prev->next = new Node<K, V>(key, 1);
            }
        }
        else
        {
            int count = node->getValue();
            node->setValue(++count);
        }
    }

    void update(const K &key, const V &value, shared_timed_mutex &mutex_)
    {
        unique_lock<shared_timed_mutex> lock(mutex_);
        Node<K, V> *prev = nullptr;
        Node<K, V> *node = head;

        while (node != nullptr && node->getKey() != key)
        {
            prev = node;
            node = node->next;
        }

        if (nullptr == node)
        {
            if (nullptr == head)
            {
                head = new Node<K, V>(key, value);
            }
            else
            {
                prev->next = new Node<K, V>(key, value);
            }
        }
        else
        {
            int count = node->getValue();
            node->setValue(value + count);
        }
    }

    vector<Node<K, V>*> getNodes()
    {
        Node<K, V> *node = head;
        vector<Node<K, V>*> res;
        while (node != nullptr)
        {
            res.push_back(new Node(node->getKey(), node->getValue()));

            node = node->next;
        }
        return res;
    }

    void clear()
            {
                
                Node<K, V> * prev = nullptr;
                Node<K, V> * node = head;
                while(node != nullptr)
                {
                    prev = node;
                    node = node->next;
                    delete prev;
                }
                head = nullptr;
            }

private:
    Node<K, V> *head;
};

template <typename K, typename V, typename F = std::hash<K>>
class HTable
{
    private:
    HashLL<K, V> *hashEntries;
    F hashFunc;
    const size_t hashSize;
    size_t MAX_MUTEX = 256;
    mutex_class *mu_wrapper;
public:
    HTable(size_t hashSize_ = 100000) : hashSize(hashSize_)
    {
        hashEntries = new HashLL<K, V>[hashSize];
        mu_wrapper = new mutex_class[MAX_MUTEX];
    }

    HTable(const HTable &) = delete;
    HTable(HTable &&) = delete;
    HTable &operator=(const HTable &) = delete;
    HTable &operator=(HTable &&) = delete;

    V get(const K &key) const
    {
        size_t hashValue = hashFunc(key) % hashSize;
        V res = hashEntries[hashValue].find(key, mu_wrapper[hashValue % MAX_MUTEX].mu);
        return res;
    }

    void update(const K &key)
    {
        size_t hashValue = hashFunc(key) % hashSize;
        hashEntries[hashValue].update(key, mu_wrapper[hashValue % MAX_MUTEX].mu);
    }

    void update(const K &key, const V &value)
    {
        size_t hashValue = hashFunc(key) % hashSize;
        hashEntries[hashValue].update(key, value, mu_wrapper[hashValue % MAX_MUTEX].mu);
    }

    vector<Node<K, V>*> getEntries()
    {
        vector<Node<K, V>*> ret;
        for (int i = 0; i < hashSize; i++)
        {
            HashLL<K, V> bucket = hashEntries[i];
            vector<Node<K, V>*> nodes = bucket.getNodes();
            ret.insert(ret.end(), nodes.begin(), nodes.end());
        }
        return ret;
    }

    void clear() {
for (int i = 0; i < hashSize; i++) {
    hashEntries[i].clear();
    }
}

};

#endif