#ifndef THREADSAFEQUEUE_HPP
#define THREADSAFEQUEUE_HPP
//Thread safe queue for prod/consumer 
 
#include <queue>
#include <mutex>
#include <condition_variable>
 
template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;
    //queues by reference
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
 
    /*! \brief Push an item into the queue. */
    void push(T item) {
        std::unique_lock<std::mutex> lock(mtx);
         q.push(std::move(item));
        cv.notify_one();
    }
 
    /*! \brief Pops an item, blocking until one is available. */
    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
         cv.wait(lock, [this]{ return !q.empty(); });
        T item = std::move(q.front());
        q.pop();
        return item;
    }
    /*! \brief True if the queue is empty. */
    bool empty() const {
         std::unique_lock<std::mutex> lock(mtx);
        return q.empty();
    }
    /*! \brief Size of the queue. */
    size_t size() const {
        std::unique_lock<std::mutex> lock(mtx);
         return q.size();
    }
 
private:
    mutable std::mutex mtx; // mutable so empty() and size() can stay const
     std::condition_variable cv;
    std::queue<T> q;
};
 
#endif // THREADSAFEQUEUE_HPP