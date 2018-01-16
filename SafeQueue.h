//
// Created by ns on 15.01.18.
//

#ifndef DROPBOX_SAFEQUEUE_H
#define DROPBOX_SAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class SafeQueue
{
public:
    void enqueue(T &element);
    T dequeue();
    bool empty();

private:
    std::queue<T> q;
    std::mutex m;
    std::condition_variable c;
};

template <class T>
void SafeQueue<T>::enqueue(T &element)
{
    std::lock_guard<std::mutex> lock(m);
    q.push(element);
    c.notify_one();
}

template <class T>
T SafeQueue<T>::dequeue()
{
    std::unique_lock<std::mutex> l(m);
    while(q.empty())
    {
        c.wait(l);
    }
    T element = q.front();
    q.pop();
    return element;
}

template <class T>
bool SafeQueue<T>::empty()
{
    return q.empty();
}

#endif //DROPBOX_SAFEQUEUE_H
