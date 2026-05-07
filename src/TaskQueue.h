#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>

class TaskQueue {
public:
    TaskQueue();

    void push(int zadacha);
    bool pop(int& zadacha);
    void stop();

private:
    std::queue<int> ochered;
    std::mutex mtx;
    std::condition_variable cv;
    bool closed;
};

#endif