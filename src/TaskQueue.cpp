#include "TaskQueue.h"

void TaskQueue::push(int zadacha) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        ochered.push(zadacha);
    }

    cv.notify_one();
}

bool TaskQueue::pop(int& zadacha) {
    std::unique_lock<std::mutex> lock(mtx);

    cv.wait(lock, [this]() {
        return !ochered.empty() || done;
    });

    if (ochered.empty() && done) {
        return false;
    }

    zadacha = ochered.front();
    ochered.pop();

    return true;
}

void TaskQueue::stop() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        done = true;
    }

    cv.notify_all();
}