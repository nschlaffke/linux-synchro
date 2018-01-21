//
// Created by piotr on 21.01.18.
//

#ifndef DROPBOX_SAFESET_H
#define DROPBOX_SAFESET_H

#include <set>
#include <mutex>

template<class T>
class SafeSet
{
public:
    void insert(T &element);
    void erase(T val);
    void erase(typename std::set<T>::iterator it);
    bool empty();
    unsigned long size();
    typename std::set<T>::iterator begin();
    typename std::set<T>::iterator end();

private:
    std::set<T> s;
    std::mutex m;
};

template<class T>
void SafeSet<T>::insert(T &element)
{
    std::lock_guard<std::mutex> lock(m);
    s.insert(element);
}

template<class T>
void SafeSet<T>::erase(T val) {
    std::lock_guard<std::mutex> lock(m);
    s.erase(val);
}

template<class T>
void SafeSet<T>::erase(typename std::set<T>::iterator it) {
    std::lock_guard<std::mutex> l(m);
    s.erase(it);
}

template<class T>
bool SafeSet<T>::empty() {
    std::lock_guard<std::mutex> l(m);
    return s.empty();
}

template<class T>
unsigned long SafeSet<T>::size() {
    std::lock_guard<std::mutex> l(m);
    return s.size();
}

template<class T>
typename std::set<T>::iterator SafeSet<T>::begin() {
    std::lock_guard<std::mutex> l(m);
    return s.begin();
}

template<class T>
typename std::set<T>::iterator SafeSet<T>::end() {
    std::lock_guard<std::mutex> l(m);
    return s.end();
}

#endif //DROPBOX_SAFESET_H
