#ifndef _MY_HASHTABLE_H_
#define _MY_HASHTABLE_H_

#include <cstdint>
#include <iostream>
#include <functional>
#include <mutex>
#include <shared_mutex>
// #include "HashNode.h"
using namespace std;

// mutex wrapper since creating vector of shared_time_mutex was not possible.
class mutex_wrapper
{
public:
    mutable shared_timed_mutex mu;
};

//Node that holds key and value
template <typename K, typename V>
class HashNode
{
public:
    HashNode(K key_, V value_) : next(nullptr), key(key_), value(value_)
    {
    }
    ~HashNode()
    {
        next = nullptr;
    }

    const K &getKey() const { return key; }
    void setValue(V value_) { value = value_; }
    const V &getValue() const { return value; }

    HashNode *next;

private:
    K key;
    V value;
};

//bucket which holds linked list of HashNodes whose hash of key collided.
template <typename K, typename V>
class HashBucket
{
public:
    HashBucket() : head(nullptr)
    {
    }

    V find(const K &key, shared_timed_mutex &mutex_) const
    {
        shared_lock<shared_timed_mutex> lock(mutex_);
        HashNode<K, V> *node = head;

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
        HashNode<K, V> *prev = nullptr;
        HashNode<K, V> *node = head;

        while (node != nullptr && node->getKey() != key)
        {
            prev = node;
            node = node->next;
        }

        if (nullptr == node)
        {
            if (nullptr == head)
            {
                head = new HashNode<K, V>(key, 1);
            }
            else
            {
                prev->next = new HashNode<K, V>(key, 1);
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
        HashNode<K, V> *prev = nullptr;
        HashNode<K, V> *node = head;

        while (node != nullptr && node->getKey() != key)
        {
            prev = node;
            node = node->next;
        }

        if (nullptr == node)
        {
            if (nullptr == head)
            {
                head = new HashNode<K, V>(key, value);
            }
            else
            {
                prev->next = new HashNode<K, V>(key, value);
            }
        }
        else
        {
            int count = node->getValue();
            node->setValue(value + count);
        }
    }

    vector<HashNode<K, V>*> getAllNodes()
    {
        HashNode<K, V> *node = head;
        vector<HashNode<K, V>*> res;
        while (node != nullptr)
        {
            res.push_back(new HashNode(node->getKey(), node->getValue()));

            node = node->next;
        }
        return res;
    }

    void clear()
            {
                //Exclusive lock to enable single write in the bucket
                // std::unique_lock<std::shared_timed_mutex> lock(mutex_);
                HashNode<K, V> * prev = nullptr;
                HashNode<K, V> * node = head;
                while(node != nullptr)
                {
                    prev = node;
                    node = node->next;
                    delete prev;
                }
                head = nullptr;
            }

private:
    HashNode<K, V> *head;
};

template <typename K, typename V, typename F = std::hash<K>>
class HashTable
{
public:
    HashTable(size_t hashSize_ = 100000) : hashSize(hashSize_)
    {
        hashEntries = new HashBucket<K, V>[hashSize];
        mu_wrapper = new mutex_wrapper[MAX_MUTEX];
    }

    HashTable(const HashTable &) = delete;
    HashTable(HashTable &&) = delete;
    HashTable &operator=(const HashTable &) = delete;
    HashTable &operator=(HashTable &&) = delete;

    V get(const K &key) const
    {
        size_t hashValue = hashFn(key) % hashSize;
        V res = hashEntries[hashValue].find(key, mu_wrapper[hashValue % MAX_MUTEX].mu);
        return res;
    }

    void update(const K &key)
    {
        size_t hashValue = hashFn(key) % hashSize;
        hashEntries[hashValue].update(key, mu_wrapper[hashValue % MAX_MUTEX].mu);
    }

    void update(const K &key, const V &value)
    {
        size_t hashValue = hashFn(key) % hashSize;
        hashEntries[hashValue].update(key, value, mu_wrapper[hashValue % MAX_MUTEX].mu);
    }

    vector<HashNode<K, V>*> getEntries()
    {
        vector<HashNode<K, V>*> ret;
        for (int i = 0; i < hashSize; i++)
        {
            HashBucket<K, V> bucket = hashEntries[i];
            vector<HashNode<K, V>*> nodes = bucket.getAllNodes();
            ret.insert(ret.end(), nodes.begin(), nodes.end());
        }
        return ret;
    }

    void clear() {
for (int i = 0; i < hashSize; i++) {
    hashEntries[i].clear();
}
    }

private:
    HashBucket<K, V> *hashEntries;
    F hashFn;
    const size_t hashSize;
    size_t MAX_MUTEX = 256;
    mutex_wrapper *mu_wrapper;
};

#endif