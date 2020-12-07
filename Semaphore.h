
#ifndef H
#define H
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <list>
#include <mutex>
#include <condition_variable>
class Semaphore
{
public:
    explicit Semaphore(int count = 0) : count_(count) {}
    void signal()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
        cv_.notify_one();
    }
    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock,[=] {return count_ > 0; });
        --count_;
    }
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};
#endif